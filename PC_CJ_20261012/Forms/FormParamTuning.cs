using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using LINMaster.LIN;

namespace LINMaster.Forms
{
    // =========================================================================
    // FormParamTuning – ELF(.out) / IAR MAP(.map) 기반 파라미터 튜닝
    //
    // [지원 파일]
    //   .out  : ARM ELF 바이너리 → nm.exe 또는 arm-none-eabi-nm 으로 심볼 추출
    //           (nm이 없으면 ELF 섹션 직접 파싱 fallback)
    //   .map  : IAR/GCC 링커 MAP 텍스트 파일 → 정규식 파싱
    //
    // [프로토콜 - DataDump 확장]
    //   READ_RAM  CMD=0xAB : [NAD][06][B4][AB][A0][A1][A2][A3]
    //     A0~A3 : 32bit 주소 (Little-Endian)
    //     응답   : [NAD][07][F4][AB][D0][D1][D2][D3]
    //
    //   WRITE_RAM CMD=0xAC : [NAD][06][B4][AC][A0][A1][D0][D1]
    //     A0~A1 : 하위 16bit 주소 (상위 0x2000 고정)
    //     D0~D1 : 쓸 데이터 (최대 2바이트)
    // =========================================================================
    public partial class FormParamTuning : Form
    {
        // ─────────────────────────────────────────────────────────────────
        // 상수
        // ─────────────────────────────────────────────────────────────────
        private const int  DIAG_TIMEOUT_MS     = 150;
        private const int  MAX_DIAG_RETRY      = 2;
        private const int  DIAG_RETRY_DELAY_MS = 20;
        private const int  MAX_ARRAY_EXPANSION = 64;
        private const int  XCP_MAX_BLOCK_READ  = 256;
        private const byte CMD_READ_RAM        = 0xAB;
        private const byte CMD_WRITE_RAM       = 0xAC;
        private const byte DEFAULT_DIAG_TX_ID  = 0x3C;
        private const byte DEFAULT_DIAG_RX_ID  = 0x3D;
        private const int  ALL_F_MAX_RETRY     = 3;   // 0xFF/0xFFFF 등 전-F 값 재전송 최대 횟수

        private const byte XCP_PID_RES           = 0xFF;
        private const byte XCP_PID_ERR           = 0xFE;
        private const byte XCP_CMD_CONNECT       = 0xFF;
        private const byte XCP_CMD_DISCONNECT    = 0xFE;
        private const byte XCP_CMD_SET_MTA       = 0xF6;
        private const byte XCP_CMD_SHORT_UPLOAD  = 0xF4;
        private const byte XCP_CMD_DOWNLOAD      = 0xF0;

        // ─────────────────────────────────────────────────────────────────
        // 내부 클래스 – 심볼
        // ─────────────────────────────────────────────────────────────────
        private class MapSymbol
        {
            public string Name    { get; set; }
            public uint   Address { get; set; }
            public int    Size    { get; set; }
            public string Source  { get; set; }
            public string Value   { get; set; }
            public string AddressHex => $"0x{Address:X8}";
            public string SizeStr    => Size > 0 ? $"{Size}" : "?";
        }

        private sealed class DwarfAbbrevAttr
        {
            public ulong Name { get; set; }
            public ulong Form { get; set; }
        }

        private sealed class DwarfAbbrevEntry
        {
            public ulong Code { get; set; }
            public ulong Tag { get; set; }
            public bool HasChildren { get; set; }
            public List<DwarfAbbrevAttr> Attributes { get; } = new List<DwarfAbbrevAttr>();
        }

        private sealed class DwarfDie
        {
            public ulong Offset { get; set; }
            public ulong Tag { get; set; }
            public Dictionary<ulong, object> Attributes { get; } = new Dictionary<ulong, object>();
            public List<DwarfDie> Children { get; } = new List<DwarfDie>();
        }

        private sealed class DwarfTypeInfo
        {
            public string Name { get; set; }
            public string Kind { get; set; }
            public int ByteSize { get; set; }
            public List<DwarfMemberInfo> Members { get; } = new List<DwarfMemberInfo>();
            public DwarfTypeInfo ElementType { get; set; }
            public int ArrayCount { get; set; }
        }

        private sealed class DwarfMemberInfo
        {
            public string Name { get; set; }
            public int Offset { get; set; }
            public DwarfTypeInfo Type { get; set; }
        }

        private sealed class DwarfVariableInfo
        {
            public string Name { get; set; }
            public uint Address { get; set; }
            public DwarfTypeInfo Type { get; set; }
        }

        private sealed class DwarfReader
        {
            private const ulong DW_TAG_ARRAY_TYPE = 0x01;
            private const ulong DW_TAG_CLASS_TYPE = 0x02;
            private const ulong DW_TAG_ENUMERATION_TYPE = 0x04;
            private const ulong DW_TAG_MEMBER = 0x0D;
            private const ulong DW_TAG_POINTER_TYPE = 0x0F;
            private const ulong DW_TAG_STRUCTURE_TYPE = 0x13;
            private const ulong DW_TAG_TYPEDEF = 0x16;
            private const ulong DW_TAG_UNION_TYPE = 0x17;
            private const ulong DW_TAG_COMPILE_UNIT = 0x11;
            private const ulong DW_TAG_SUBRANGE_TYPE = 0x21;
            private const ulong DW_TAG_BASE_TYPE = 0x24;
            private const ulong DW_TAG_CONST_TYPE = 0x26;
            private const ulong DW_TAG_VARIABLE = 0x34;
            private const ulong DW_TAG_VOLATILE_TYPE = 0x35;

            private const ulong DW_AT_LOCATION = 0x02;
            private const ulong DW_AT_NAME = 0x03;
            private const ulong DW_AT_BYTE_SIZE = 0x0B;
            private const ulong DW_AT_BIT_SIZE = 0x0D;
            private const ulong DW_AT_TYPE = 0x49;
            private const ulong DW_AT_DATA_MEMBER_LOCATION = 0x38;
            private const ulong DW_AT_SPECIFICATION = 0x47;
            private const ulong DW_AT_COUNT = 0x37;
            private const ulong DW_AT_UPPER_BOUND = 0x2F;
            private const ulong DW_AT_ABSTRACT_ORIGIN = 0x31;

            private const ulong DW_FORM_ADDR = 0x01;
            private const ulong DW_FORM_BLOCK2 = 0x03;
            private const ulong DW_FORM_BLOCK4 = 0x04;
            private const ulong DW_FORM_DATA2 = 0x05;
            private const ulong DW_FORM_DATA4 = 0x06;
            private const ulong DW_FORM_DATA8 = 0x07;
            private const ulong DW_FORM_STRING = 0x08;
            private const ulong DW_FORM_BLOCK = 0x09;
            private const ulong DW_FORM_BLOCK1 = 0x0A;
            private const ulong DW_FORM_DATA1 = 0x0B;
            private const ulong DW_FORM_FLAG = 0x0C;
            private const ulong DW_FORM_SDATA = 0x0D;
            private const ulong DW_FORM_STRP = 0x0E;
            private const ulong DW_FORM_UDATA = 0x0F;
            private const ulong DW_FORM_REF_ADDR = 0x10;
            private const ulong DW_FORM_REF1 = 0x11;
            private const ulong DW_FORM_REF2 = 0x12;
            private const ulong DW_FORM_REF4 = 0x13;
            private const ulong DW_FORM_REF8 = 0x14;
            private const ulong DW_FORM_REF_UDATA = 0x15;
            private const ulong DW_FORM_INDIRECT = 0x16;
            private const ulong DW_FORM_SEC_OFFSET = 0x17;
            private const ulong DW_FORM_EXPRLOC = 0x18;
            private const ulong DW_FORM_FLAG_PRESENT = 0x19;

            private const byte DW_OP_ADDR = 0x03;
            private const byte DW_OP_PLUS_UCONST = 0x23;

            private readonly Dictionary<string, byte[]> _sections;
            private readonly Dictionary<ulong, DwarfAbbrevEntry> _abbrevCache = new Dictionary<ulong, DwarfAbbrevEntry>();
            private readonly Dictionary<ulong, DwarfDie> _dies = new Dictionary<ulong, DwarfDie>();
            private readonly Dictionary<ulong, DwarfTypeInfo> _typeCache = new Dictionary<ulong, DwarfTypeInfo>();
            private readonly HashSet<ulong> _typeStack = new HashSet<ulong>();
            private readonly byte[] _debugInfo;
            private readonly byte[] _debugAbbrev;
            private readonly byte[] _debugStr;

            public DwarfReader(Dictionary<string, byte[]> sections)
            {
                _sections = sections;
                _sections.TryGetValue(".debug_info", out _debugInfo);
                _sections.TryGetValue(".debug_abbrev", out _debugAbbrev);
                _sections.TryGetValue(".debug_str", out _debugStr);
            }

            public bool CanParse =>
                _debugInfo != null && _debugInfo.Length > 0 &&
                _debugAbbrev != null && _debugAbbrev.Length > 0;

            public List<DwarfVariableInfo> ParseVariables()
            {
                var variables = new List<DwarfVariableInfo>();
                if (!CanParse) return variables;

                int cuOffset = 0;
                while (cuOffset + 11 < _debugInfo.Length)
                {
                    uint unitLength = ReadU32(_debugInfo, cuOffset);
                    if (unitLength == 0 || cuOffset + 4 + unitLength > _debugInfo.Length) break;

                    int unitEnd = cuOffset + 4 + (int)unitLength;
                    ushort version = ReadU16(_debugInfo, cuOffset + 4);
                    if (version < 2 || version > 4)
                    {
                        cuOffset = unitEnd;
                        continue;
                    }

                    uint abbrevOffset = ReadU32(_debugInfo, cuOffset + 6);
                    byte addressSize = _debugInfo[cuOffset + 10];
                    int cursor = cuOffset + 11;
                    ParseCompileUnit(cuOffset, unitEnd, abbrevOffset, addressSize, ref cursor, variables);
                    cuOffset = unitEnd;
                }

                return variables;
            }

            private void ParseCompileUnit(int cuOffset, int unitEnd, uint abbrevOffset, byte addressSize, ref int cursor, List<DwarfVariableInfo> variables)
            {
                var stack = new Stack<DwarfDie>();
                while (cursor < unitEnd)
                {
                    int entryOffset = cursor;
                    ulong abbrevCode = ReadULEB128(_debugInfo, ref cursor);
                    if (abbrevCode == 0)
                    {
                        if (stack.Count > 0) stack.Pop();
                        continue;
                    }

                    var abbrev = ReadAbbrev(abbrevOffset, abbrevCode);
                    if (abbrev == null) break;

                    var die = new DwarfDie { Offset = (ulong)entryOffset, Tag = abbrev.Tag };
                    foreach (var attr in abbrev.Attributes)
                    {
                        die.Attributes[attr.Name] = ReadFormValue(attr.Form, cuOffset, addressSize, ref cursor);
                    }

                    _dies[die.Offset] = die;
                    if (stack.Count > 0) stack.Peek().Children.Add(die);
                    if (abbrev.HasChildren) stack.Push(die);

                    if (die.Tag == DW_TAG_VARIABLE)
                    {
                        DwarfVariableInfo variable = BuildVariable(die);
                        if (variable != null) variables.Add(variable);
                    }
                }
            }

            private DwarfVariableInfo BuildVariable(DwarfDie die)
            {
                string name = ResolveName(die);
                uint? address = ResolveAddress(die);
                ulong? typeRef = GetRefAttr(die, DW_AT_TYPE);
                if (string.IsNullOrEmpty(name) || address == null || typeRef == null) return null;
                if (address < 0x20000000 || address > 0x2000FFFF) return null;

                DwarfTypeInfo type = ResolveType(typeRef.Value);
                if (type == null) return null;

                return new DwarfVariableInfo
                {
                    Name = name,
                    Address = address.Value,
                    Type = type
                };
            }

            private string ResolveName(DwarfDie die)
            {
                if (TryGetStringAttr(die, DW_AT_NAME, out string name)) return name;

                ulong? specRef = GetRefAttr(die, DW_AT_SPECIFICATION) ?? GetRefAttr(die, DW_AT_ABSTRACT_ORIGIN);
                if (specRef != null && _dies.TryGetValue(specRef.Value, out DwarfDie specDie))
                {
                    if (TryGetStringAttr(specDie, DW_AT_NAME, out name)) return name;
                }

                return null;
            }

            private uint? ResolveAddress(DwarfDie die)
            {
                if (!die.Attributes.TryGetValue(DW_AT_LOCATION, out object raw) || raw == null) return null;
                if (raw is uint addr32) return addr32;
                if (raw is ulong addr64) return (uint)addr64;
                if (raw is byte[] block)
                {
                    if (block.Length >= 5 && block[0] == DW_OP_ADDR)
                        return (uint)(block[1] | (block[2] << 8) | (block[3] << 16) | (block[4] << 24));
                }
                return null;
            }

            private DwarfTypeInfo ResolveType(ulong dieOffset)
            {
                if (_typeCache.TryGetValue(dieOffset, out DwarfTypeInfo cached)) return cached;
                if (_typeStack.Contains(dieOffset)) return null;
                if (!_dies.TryGetValue(dieOffset, out DwarfDie die)) return null;

                _typeStack.Add(dieOffset);
                try
                {
                    DwarfTypeInfo result = null;
                    switch (die.Tag)
                    {
                        case DW_TAG_TYPEDEF:
                        case DW_TAG_CONST_TYPE:
                        case DW_TAG_VOLATILE_TYPE:
                            result = ResolveWrappedType(die);
                            break;

                        case DW_TAG_BASE_TYPE:
                        case DW_TAG_ENUMERATION_TYPE:
                        case DW_TAG_POINTER_TYPE:
                            result = new DwarfTypeInfo
                            {
                                Name = GetStringAttr(die, DW_AT_NAME),
                                Kind = "scalar",
                                ByteSize = GetIntAttr(die, DW_AT_BYTE_SIZE) ?? 0
                            };
                            break;

                        case DW_TAG_STRUCTURE_TYPE:
                        case DW_TAG_CLASS_TYPE:
                        case DW_TAG_UNION_TYPE:
                            result = ResolveAggregateType(die);
                            break;

                        case DW_TAG_ARRAY_TYPE:
                            result = ResolveArrayType(die);
                            break;
                    }

                    if (result != null) _typeCache[dieOffset] = result;
                    return result;
                }
                finally
                {
                    _typeStack.Remove(dieOffset);
                }
            }

            private DwarfTypeInfo ResolveWrappedType(DwarfDie die)
            {
                ulong? targetRef = GetRefAttr(die, DW_AT_TYPE);
                DwarfTypeInfo inner = targetRef != null ? ResolveType(targetRef.Value) : null;
                if (inner == null) return null;

                if (die.Tag == DW_TAG_TYPEDEF && TryGetStringAttr(die, DW_AT_NAME, out string alias))
                {
                    var aliasType = new DwarfTypeInfo
                    {
                        Name = alias,
                        Kind = inner.Kind,
                        ByteSize = inner.ByteSize,
                        ElementType = inner.ElementType,
                        ArrayCount = inner.ArrayCount
                    };
                    foreach (var member in inner.Members) aliasType.Members.Add(member);
                    return aliasType;
                }

                var wrapped = new DwarfTypeInfo
                {
                    Name = inner.Name,
                    Kind = inner.Kind,
                    ByteSize = inner.ByteSize,
                    ElementType = inner.ElementType,
                    ArrayCount = inner.ArrayCount
                };
                foreach (var member in inner.Members) wrapped.Members.Add(member);
                return wrapped;
            }

            private DwarfTypeInfo ResolveAggregateType(DwarfDie die)
            {
                var type = new DwarfTypeInfo
                {
                    Name = GetStringAttr(die, DW_AT_NAME),
                    Kind = die.Tag == DW_TAG_UNION_TYPE ? "union" : "struct",
                    ByteSize = GetIntAttr(die, DW_AT_BYTE_SIZE) ?? 0
                };

                foreach (var child in die.Children)
                {
                    if (child.Tag != DW_TAG_MEMBER) continue;
                    if (child.Attributes.ContainsKey(DW_AT_BIT_SIZE)) continue;

                    ulong? memberTypeRef = GetRefAttr(child, DW_AT_TYPE);
                    DwarfTypeInfo memberType = memberTypeRef != null ? ResolveType(memberTypeRef.Value) : null;
                    if (memberType == null) continue;

                    int offset = 0;
                    if (die.Tag != DW_TAG_UNION_TYPE)
                    {
                        int? loc = GetDataMemberOffset(child);
                        if (loc == null) continue;
                        offset = loc.Value;
                    }

                    type.Members.Add(new DwarfMemberInfo
                    {
                        Name = GetStringAttr(child, DW_AT_NAME) ?? "member",
                        Offset = offset,
                        Type = memberType
                    });
                }

                return type;
            }

            private DwarfTypeInfo ResolveArrayType(DwarfDie die)
            {
                ulong? elemRef = GetRefAttr(die, DW_AT_TYPE);
                DwarfTypeInfo elemType = elemRef != null ? ResolveType(elemRef.Value) : null;
                if (elemType == null) return null;

                int count = 0;
                foreach (var child in die.Children)
                {
                    if (child.Tag != DW_TAG_SUBRANGE_TYPE) continue;
                    int? directCount = GetIntAttr(child, DW_AT_COUNT);
                    if (directCount != null)
                    {
                        count = directCount.Value;
                        break;
                    }

                    int? upperBound = GetIntAttr(child, DW_AT_UPPER_BOUND);
                    if (upperBound != null)
                    {
                        count = upperBound.Value + 1;
                        break;
                    }
                }

                int size = GetIntAttr(die, DW_AT_BYTE_SIZE) ?? 0;
                if (size == 0 && count > 0 && elemType.ByteSize > 0) size = elemType.ByteSize * count;

                return new DwarfTypeInfo
                {
                    Name = GetStringAttr(die, DW_AT_NAME),
                    Kind = "array",
                    ByteSize = size,
                    ElementType = elemType,
                    ArrayCount = count
                };
            }

            private int? GetDataMemberOffset(DwarfDie die)
            {
                if (!die.Attributes.TryGetValue(DW_AT_DATA_MEMBER_LOCATION, out object raw) || raw == null) return null;
                if (raw is byte[] block)
                {
                    if (block.Length > 0 && block[0] == DW_OP_PLUS_UCONST)
                    {
                        int pos = 1;
                        return (int)ReadULEB128(block, ref pos);
                    }
                    return null;
                }

                if (raw is long s) return (int)s;
                if (raw is int i) return i;
                if (raw is uint u) return (int)u;
                if (raw is ulong ul) return (int)ul;
                return null;
            }

            private DwarfAbbrevEntry ReadAbbrev(uint abbrevOffset, ulong targetCode)
            {
                ulong cacheKey = ((ulong)abbrevOffset << 32) | targetCode;
                if (_abbrevCache.TryGetValue(cacheKey, out DwarfAbbrevEntry cached)) return cached;

                int cursor = (int)abbrevOffset;
                while (cursor < _debugAbbrev.Length)
                {
                    ulong code = ReadULEB128(_debugAbbrev, ref cursor);
                    if (code == 0) break;

                    ulong tag = ReadULEB128(_debugAbbrev, ref cursor);
                    bool hasChildren = _debugAbbrev[cursor++] != 0;
                    var entry = new DwarfAbbrevEntry
                    {
                        Code = code,
                        Tag = tag,
                        HasChildren = hasChildren
                    };

                    while (true)
                    {
                        ulong name = ReadULEB128(_debugAbbrev, ref cursor);
                        ulong form = ReadULEB128(_debugAbbrev, ref cursor);
                        if (name == 0 && form == 0) break;
                        entry.Attributes.Add(new DwarfAbbrevAttr { Name = name, Form = form });
                    }

                    _abbrevCache[((ulong)abbrevOffset << 32) | code] = entry;
                    if (code == targetCode) return entry;
                }

                return null;
            }

            private object ReadFormValue(ulong form, int cuOffset, byte addressSize, ref int cursor)
            {
                switch (form)
                {
                    case DW_FORM_ADDR:
                        if (addressSize == 4)
                        {
                            uint addr = ReadU32(_debugInfo, cursor);
                            cursor += 4;
                            return addr;
                        }
                        ulong addr64 = ReadU64(_debugInfo, cursor);
                        cursor += 8;
                        return addr64;

                    case DW_FORM_DATA1:
                        return _debugInfo[cursor++];
                    case DW_FORM_DATA2:
                        ushort d2 = ReadU16(_debugInfo, cursor);
                        cursor += 2;
                        return (ulong)d2;
                    case DW_FORM_DATA4:
                    case DW_FORM_SEC_OFFSET:
                    case DW_FORM_STRP:
                    {
                        uint d4 = ReadU32(_debugInfo, cursor);
                        cursor += 4;
                        if (form == DW_FORM_STRP) return ReadDebugString(d4);
                        return (ulong)d4;
                    }
                    case DW_FORM_DATA8:
                    {
                        ulong d8 = ReadU64(_debugInfo, cursor);
                        cursor += 8;
                        return d8;
                    }
                    case DW_FORM_STRING:
                    {
                        string s = ReadCString(_debugInfo, ref cursor);
                        return s;
                    }
                    case DW_FORM_FLAG:
                        return _debugInfo[cursor++] != 0;
                    case DW_FORM_FLAG_PRESENT:
                        return true;
                    case DW_FORM_SDATA:
                        return ReadSLEB128(_debugInfo, ref cursor);
                    case DW_FORM_UDATA:
                        return ReadULEB128(_debugInfo, ref cursor);
                    case DW_FORM_REF1:
                        return (ulong)(cuOffset + _debugInfo[cursor++]);
                    case DW_FORM_REF2:
                    {
                        ushort r2 = ReadU16(_debugInfo, cursor);
                        cursor += 2;
                        return (ulong)(cuOffset + r2);
                    }
                    case DW_FORM_REF4:
                    {
                        uint r4 = ReadU32(_debugInfo, cursor);
                        cursor += 4;
                        return (ulong)(cuOffset + r4);
                    }
                    case DW_FORM_REF8:
                    {
                        ulong r8 = ReadU64(_debugInfo, cursor);
                        cursor += 8;
                        return (ulong)cuOffset + r8;
                    }
                    case DW_FORM_REF_UDATA:
                        return (ulong)cuOffset + ReadULEB128(_debugInfo, ref cursor);
                    case DW_FORM_REF_ADDR:
                    {
                        uint ra = ReadU32(_debugInfo, cursor);
                        cursor += 4;
                        return (ulong)ra;
                    }
                    case DW_FORM_BLOCK1:
                    {
                        int len = _debugInfo[cursor++];
                        byte[] block = new byte[len];
                        Array.Copy(_debugInfo, cursor, block, 0, len);
                        cursor += len;
                        return block;
                    }
                    case DW_FORM_BLOCK2:
                    {
                        int len = ReadU16(_debugInfo, cursor);
                        cursor += 2;
                        byte[] block = new byte[len];
                        Array.Copy(_debugInfo, cursor, block, 0, len);
                        cursor += len;
                        return block;
                    }
                    case DW_FORM_BLOCK4:
                    {
                        int len = (int)ReadU32(_debugInfo, cursor);
                        cursor += 4;
                        byte[] block = new byte[len];
                        Array.Copy(_debugInfo, cursor, block, 0, len);
                        cursor += len;
                        return block;
                    }
                    case DW_FORM_BLOCK:
                    case DW_FORM_EXPRLOC:
                    {
                        int len = (int)ReadULEB128(_debugInfo, ref cursor);
                        byte[] block = new byte[len];
                        Array.Copy(_debugInfo, cursor, block, 0, len);
                        cursor += len;
                        return block;
                    }
                    case DW_FORM_INDIRECT:
                    {
                        ulong actual = ReadULEB128(_debugInfo, ref cursor);
                        return ReadFormValue(actual, cuOffset, addressSize, ref cursor);
                    }
                }

                throw new NotSupportedException($"Unsupported DWARF form: 0x{form:X}");
            }

            private bool TryGetStringAttr(DwarfDie die, ulong attr, out string value)
            {
                value = GetStringAttr(die, attr);
                return !string.IsNullOrEmpty(value);
            }

            private string GetStringAttr(DwarfDie die, ulong attr)
            {
                if (!die.Attributes.TryGetValue(attr, out object raw) || raw == null) return null;
                return raw as string;
            }

            private int? GetIntAttr(DwarfDie die, ulong attr)
            {
                if (!die.Attributes.TryGetValue(attr, out object raw) || raw == null) return null;
                if (raw is byte b) return b;
                if (raw is ushort us) return us;
                if (raw is uint ui) return (int)ui;
                if (raw is ulong ul) return (int)ul;
                if (raw is sbyte sb) return sb;
                if (raw is short s) return s;
                if (raw is int i) return i;
                if (raw is long l) return (int)l;
                return null;
            }

            private ulong? GetRefAttr(DwarfDie die, ulong attr)
            {
                if (!die.Attributes.TryGetValue(attr, out object raw) || raw == null) return null;
                if (raw is ulong ul) return ul;
                if (raw is uint ui) return ui;
                return null;
            }

            private string ReadDebugString(uint offset)
            {
                if (_debugStr == null || offset >= _debugStr.Length) return null;
                int pos = (int)offset;
                return ReadCString(_debugStr, ref pos);
            }

            private static string ReadCString(byte[] data, ref int cursor)
            {
                int start = cursor;
                while (cursor < data.Length && data[cursor] != 0) cursor++;
                string value = Encoding.ASCII.GetString(data, start, cursor - start);
                if (cursor < data.Length) cursor++;
                return value;
            }

            private static ushort ReadU16(byte[] data, int offset) =>
                (ushort)(data[offset] | (data[offset + 1] << 8));

            private static uint ReadU32(byte[] data, int offset) =>
                (uint)(data[offset] | (data[offset + 1] << 8) | (data[offset + 2] << 16) | (data[offset + 3] << 24));

            private static ulong ReadU64(byte[] data, int offset) =>
                ReadU32(data, offset) | ((ulong)ReadU32(data, offset + 4) << 32);

            private static ulong ReadULEB128(byte[] data, ref int cursor)
            {
                ulong value = 0;
                int shift = 0;
                while (cursor < data.Length)
                {
                    byte b = data[cursor++];
                    value |= ((ulong)(b & 0x7F)) << shift;
                    if ((b & 0x80) == 0) break;
                    shift += 7;
                }
                return value;
            }

            private static long ReadSLEB128(byte[] data, ref int cursor)
            {
                long value = 0;
                int shift = 0;
                byte b;
                do
                {
                    b = data[cursor++];
                    value |= ((long)(b & 0x7F)) << shift;
                    shift += 7;
                } while ((b & 0x80) != 0);

                if ((shift < 64) && ((b & 0x40) != 0))
                    value |= -1L << shift;

                return value;
            }
        }

        // ─────────────────────────────────────────────────────────────────
        // 멤버 변수
        // ─────────────────────────────────────────────────────────────────
        private readonly ILINGateway     _gw;
        private readonly Queue<LINFrame> _rxQueue = new Queue<LINFrame>();
        private readonly object          _rxLock  = new object();

        private List<MapSymbol> _symbols   = new List<MapSymbol>();
        private List<MapSymbol> _filtered  = new List<MapSymbol>();
        private List<MapSymbol> _favorites = new List<MapSymbol>();
        private string          _filePath  = null;
        private bool            _xcpConnected;
        private readonly byte   _cjChairId;


        // =========================================================================
        // 생성자 / 종료
        // =========================================================================
        public FormParamTuning(ILINGateway gw) : this(gw, 0)
        {
        }

        public FormParamTuning(ILINGateway gw, byte cjChairId)
        {
            _gw = gw;
            _cjChairId = cjChairId;
            InitializeComponent();
            if (_cjChairId >= 0x0A && _cjChairId <= 0x0D)
            {
                chkUseXcp.Checked = true;
                chkUseXcp.Enabled = false;
                nudNAD.Value = _cjChairId - 0x09;
                nudNAD.Enabled = false;
                nudXcpTxId.Value = GetXcpTxFrameId();
                nudXcpRxId.Value = GetXcpRxFrameId();
                nudXcpTxId.Enabled = false;
                nudXcpRxId.Enabled = false;
                nudXcpAddrExt.Value = 0;
                nudXcpAddrExt.Enabled = false;
                lblNADCap.Visible = false;
                nudNAD.Visible = false;
                chkUseXcp.Visible = false;
                lblXcpTx.Visible = false;
                nudXcpTxId.Visible = false;
                lblXcpRx.Visible = false;
                nudXcpRxId.Visible = false;
                lblXcpExt.Visible = false;
                nudXcpAddrExt.Visible = false;
                lblConnStatus.Location = new System.Drawing.Point(570, 12);
                lblMapStatus.Width = 390;
                btnWrite.Enabled = false;
                btnWrite.Visible = false;
                txtWriteVal.Visible = false;
                btnHexDec.Visible = false;
                lblWriteCap.Visible = false;
                grpReadWrite.Text = " CJ XCP READ ";
                tabXcpMain.Visible = false;
                MinimumSize = new System.Drawing.Size(735, 600);
                ClientSize = new System.Drawing.Size(725, 1116);
                Text = string.Format("CJ XCP Read - Chair 0x{0:X2} / TX 0x{1:X2} / RX 0x{2:X2}", _cjChairId, GetXcpTxFrameId(), GetXcpRxFrameId());
            }
            // DAQ 값 컬럼 색상 (Designer에서 지원 안 되는 중첩 속성)
            //daqColValue.DefaultCellStyle.ForeColor = System.Drawing.Color.Lime;
            if (_gw != null) _gw.FrameReceived += OnGwFrame;
            UpdateConnectionStatus();
            lblMapStatus.Text = ".out 또는 .map 파일을 열어주세요.";
            UpdateAccessHint();

            // ── pnlGraph 더블버퍼 활성화 (깜빡임 제거) ────────────────────
            // Panel.DoubleBuffered 는 protected 이므로 리플렉션으로 접근
            typeof(System.Windows.Forms.Panel).InvokeMember(
                "DoubleBuffered",
                System.Reflection.BindingFlags.SetProperty |
                System.Reflection.BindingFlags.Instance   |
                System.Reflection.BindingFlags.NonPublic,
                null, pnlGraph, new object[] { true });

            // ── XCP 확장 기능: 타이머 초기화 ──────────────────────────────
            _daqTimer = new System.Windows.Forms.Timer { Interval = 200 };
            _daqTimer.Tick += DaqTimer_Tick;
            _graphTimer = new System.Windows.Forms.Timer { Interval = 500 };
            _graphTimer.Tick += GraphTimer_Tick;
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            // X 버튼 또는 어떤 경로로 닫혀도 그래프/DAQ 먼저 정지 후 초기화
            _graphRunning = false;
            _graphTimer?.Stop();
            _daqRunning = false;
            _daqTimer?.Stop();

            // XCP DAQ 하드웨어 정지 시도 (연결돼 있을 때만)
            if (_gw?.IsConnected == true && UseXcpMode && _cjChairId == 0)
            {
                try { XcpStartStopSynch(0x01); } catch { }
            }

            // 그래프/DAQ 상태 초기화
            _graphSeries?.Clear();
            _daqEntries?.Clear();
            _graphFavMode = false;
            _graphTickBusy = false;

            base.OnFormClosing(e);
        }

        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            // DAQ 정지 후 XCP 해제
            if (_daqRunning) StopDaq();
            _daqTimer?.Stop();
            _daqTimer?.Dispose();
            _graphTimer?.Stop();
            _graphTimer?.Dispose();
            TryDisconnectXcp();
            if (_gw != null) _gw.FrameReceived -= OnGwFrame;
            base.OnFormClosed(e);
        }


        private bool UseXcpMode => chkUseXcp != null && chkUseXcp.Checked;

        // =========================================================================
        // XCP 플로팅 탭 패널 빌드 (드래그 이동 + 아이콘 이미지 + 탭 분리)
        // =========================================================================
        // 드래그용 상태 변수

        private byte GetDiagRxFrameId() => DEFAULT_DIAG_RX_ID;
        private byte GetXcpTxFrameId() => _cjChairId >= 0x0A && _cjChairId <= 0x0D ? (byte)(0x20 + _cjChairId - 0x0A) : nudXcpTxId != null ? (byte)nudXcpTxId.Value : DEFAULT_DIAG_TX_ID;
        private byte GetXcpRxFrameId() => _cjChairId >= 0x0A && _cjChairId <= 0x0D ? (byte)(0x24 + _cjChairId - 0x0A) : nudXcpRxId != null ? (byte)nudXcpRxId.Value : DEFAULT_DIAG_RX_ID;
        private byte GetXcpAddrExt() => nudXcpAddrExt != null ? (byte)nudXcpAddrExt.Value : (byte)0x00;

        private void UpdateAccessHint()
        {
            if (lblHint == null) return;
            lblHint.Text = _cjChairId != 0 ? "CJ XCP: 선택 의자 LED 변수 읽기 전용 (.out 권장).\r\n큰 배열은 4바이트씩 자동으로 나누어 읽습니다." : UseXcpMode
                ? "XCP mode: CONNECT / SHORT_UPLOAD / SET_MTA / DOWNLOAD\r\nTransport는 현재 LIN 8-byte CTO 왕복으로 사용합니다."
                : "※ WRITE는 최대 2바이트(uint16).\r\n큰 구조체는 개별 오프셋을 사용하세요.";
        }

        // Designer 이벤트 핸들러 (도구상자에서 이벤트 탭으로 연결 가능)
        private void chkUseXcp_CheckedChanged(object sender, EventArgs e)
        {
            _xcpConnected = false;
            UpdateAccessHint();
        }

        private void nudXcpTxId_ValueChanged(object sender, EventArgs e)  { _xcpConnected = false; }
        private void nudXcpRxId_ValueChanged(object sender, EventArgs e)  { _xcpConnected = false; }
        private void nudXcpAddrExt_ValueChanged(object sender, EventArgs e) { _xcpConnected = false; }

        // =========================================================================
        // RX 수신
        // =========================================================================
        private void OnGwFrame(LINFrame f)
        {
            if (f.Direction == LINDirection.RX &&
                (((_cjChairId == 0) && f.FrameID == GetDiagRxFrameId() && f.DataLength >= 5) ||
                 (UseXcpMode && f.FrameID == GetXcpRxFrameId() && f.DataLength >= 1)))
                lock (_rxLock) { _rxQueue.Enqueue(f); }
        }

        private LINFrame WaitRx(int timeoutMs = DIAG_TIMEOUT_MS)
        {
            DateTime deadline = DateTime.Now.AddMilliseconds(timeoutMs);
            while (DateTime.Now < deadline)
            {
                Application.DoEvents();
                lock (_rxLock) { if (_rxQueue.Count > 0) return _rxQueue.Dequeue(); }
                Thread.Sleep(1);
            }
            return null;
        }

        private static void Delay(int ms)
        {
            DateTime until = DateTime.Now.AddMilliseconds(ms);
            while (DateTime.Now < until) Application.DoEvents();
        }

        // ── 비동기 버전 (그래프 전용) ────────────────────────────────────
        private async Task<LINFrame> WaitRxAsync(int timeoutMs = DIAG_TIMEOUT_MS)
        {
            DateTime deadline = DateTime.Now.AddMilliseconds(timeoutMs);
            while (DateTime.Now < deadline)
            {
                lock (_rxLock) { if (_rxQueue.Count > 0) return _rxQueue.Dequeue(); }
                await Task.Delay(2).ConfigureAwait(true);
            }
            return null;
        }

        private async Task<LINFrame> SendDiagAsync(byte[] payload8)
        {
            lock (_rxLock) { _rxQueue.Clear(); }
            var f3C = new LINFrame
            {
                FrameID      = 0x3C,
                DataLength   = 8,
                ChecksumType = LINChecksumType.Classic
            };
            Array.Copy(payload8, f3C.Data, 8);
            _gw.SendPublisher(f3C);
            await Task.Delay(3).ConfigureAwait(true);
            _gw.SendSubscriberRequest(0x3D, 8, LINChecksumType.Classic);
            return await WaitRxAsync(DIAG_TIMEOUT_MS).ConfigureAwait(true);
        }

        private async Task<LINFrame> SendDiagWithRetryAsync(byte[] payload8)
        {
            for (int i = 0; i <= MAX_DIAG_RETRY; i++)
            {
                LINFrame rx = await SendDiagAsync(payload8).ConfigureAwait(true);
                if (rx != null) return rx;
                if (i < MAX_DIAG_RETRY) await Task.Delay(DIAG_RETRY_DELAY_MS).ConfigureAwait(true);
            }
            return null;
        }

        private async Task<LINFrame> SendXcpCtoAsync(byte[] cto)
        {
            lock (_rxLock) { _rxQueue.Clear(); }
            var tx = new LINFrame
            {
                FrameID      = GetXcpTxFrameId(),
                DataLength   = 8,
                ChecksumType = _cjChairId == 0 ? LINChecksumType.Classic : LINChecksumType.Enhanced
            };
            Array.Clear(tx.Data, 0, tx.Data.Length);
            Array.Copy(cto, tx.Data, Math.Min(8, cto.Length));
            _gw.SendPublisher(tx);
            await Task.Delay(_cjChairId == 0 ? 3 : 20).ConfigureAwait(true);
            _gw.SendSubscriberRequest(GetXcpRxFrameId(), 8, _cjChairId == 0 ? LINChecksumType.Classic : LINChecksumType.Enhanced);
            return await WaitRxAsync(DIAG_TIMEOUT_MS).ConfigureAwait(true);
        }

        private async Task<LINFrame> SendXcpCtoWithRetryAsync(byte[] cto)
        {
            for (int i = 0; i <= MAX_DIAG_RETRY; i++)
            {
                LINFrame rx = await SendXcpCtoAsync(cto).ConfigureAwait(true);
                if (rx != null) return rx;
                if (i < MAX_DIAG_RETRY) await Task.Delay(DIAG_RETRY_DELAY_MS).ConfigureAwait(true);
            }
            return null;
        }

        private async Task<bool> EnsureXcpConnectedAsync()
        {
            if (_xcpConnected) return true;
            var cmd = new byte[8];
            cmd[0] = XCP_CMD_CONNECT;
            cmd[1] = 0x00;
            LINFrame rx = await SendXcpCtoWithRetryAsync(cmd).ConfigureAwait(true);
            if (!IsPositiveXcpResponse(rx, out string err))
            {
                SetStatus("XCP CONNECT 실패" + (string.IsNullOrEmpty(err) ? "" : $": {err}"));
                _xcpConnected = false;
                return false;
            }
            _xcpConnected = true;
            return true;
        }

        private async Task<(bool ok, uint value)> XcpShortUploadAsync(uint address, int size)
        {
            int readLen = Math.Max(1, Math.Min(size > 0 ? size : 4, 4));
            if (!await EnsureXcpConnectedAsync().ConfigureAwait(true)) return (false, 0);

            var cmd = new byte[8];
            cmd[0] = XCP_CMD_SHORT_UPLOAD;
            cmd[1] = (byte)readLen;
            cmd[2] = 0x00;
            cmd[3] = GetXcpAddrExt();
            cmd[4] = (byte)(address & 0xFF);
            cmd[5] = (byte)((address >> 8) & 0xFF);
            cmd[6] = (byte)((address >> 16) & 0xFF);
            cmd[7] = (byte)((address >> 24) & 0xFF);

            for (int allFTry = 0; allFTry < ALL_F_MAX_RETRY; allFTry++)
            {
                LINFrame rx = await SendXcpCtoWithRetryAsync(cmd).ConfigureAwait(true);
                if (!IsPositiveXcpResponse(rx, out string err)) return (false, 0);

                uint tmpVal = 0;
                for (int i = 0; i < readLen && (i + 1) < rx.DataLength; i++)
                    tmpVal |= (uint)rx.Data[i + 1] << (8 * i);
                tmpVal = MaskValueForSize(tmpVal, size);

                if (!IsAllFValue(tmpVal, size) || allFTry == ALL_F_MAX_RETRY - 1)
                    return (true, tmpVal);

                await Task.Delay(DIAG_RETRY_DELAY_MS).ConfigureAwait(true);
            }
            return (true, MaskValueForSize(0xFFFFFFFF, size));
        }

        // =========================================================================
        // LIN 전송 헬퍼
        // =========================================================================
        private LINFrame SendDiag(byte nad, byte[] payload8)
        {
            lock (_rxLock) { _rxQueue.Clear(); }
            var f3C = new LINFrame
            {
                FrameID      = 0x3C,
                DataLength   = 8,
                ChecksumType = LINChecksumType.Classic
            };
            Array.Copy(payload8, f3C.Data, 8);
            _gw.SendPublisher(f3C);
            Delay(10);
            _gw.SendSubscriberRequest(0x3D, 8, LINChecksumType.Classic);
            return WaitRx(DIAG_TIMEOUT_MS);
        }

        private LINFrame SendDiagWithRetry(byte nad, byte[] payload8)
        {
            for (int i = 0; i <= MAX_DIAG_RETRY; i++)
            {
                LINFrame rx = SendDiag(nad, payload8);
                if (rx != null) return rx;
                if (i < MAX_DIAG_RETRY) Delay(DIAG_RETRY_DELAY_MS);
            }
            return null;
        }

        private LINFrame SendXcpCto(byte[] cto)
        {
            lock (_rxLock) { _rxQueue.Clear(); }

            var tx = new LINFrame
            {
                FrameID = GetXcpTxFrameId(),
                DataLength = 8,
                ChecksumType = _cjChairId == 0 ? LINChecksumType.Classic : LINChecksumType.Enhanced
            };
            Array.Clear(tx.Data, 0, tx.Data.Length);
            Array.Copy(cto, tx.Data, Math.Min(8, cto.Length));

            _gw.SendPublisher(tx);
            Delay(_cjChairId == 0 ? 10 : 20);
            _gw.SendSubscriberRequest(GetXcpRxFrameId(), 8, _cjChairId == 0 ? LINChecksumType.Classic : LINChecksumType.Enhanced);
            return WaitRx(DIAG_TIMEOUT_MS);
        }

        private LINFrame SendXcpCtoWithRetry(byte[] cto)
        {
            for (int i = 0; i <= MAX_DIAG_RETRY; i++)
            {
                LINFrame rx = SendXcpCto(cto);
                if (rx != null) return rx;
                if (i < MAX_DIAG_RETRY) Delay(DIAG_RETRY_DELAY_MS);
            }
            return null;
        }

        private bool EnsureXcpConnected()
        {
            if (_xcpConnected) return true;

            var cmd = new byte[8];
            cmd[0] = XCP_CMD_CONNECT;
            cmd[1] = 0x00;

            LINFrame rx = SendXcpCtoWithRetry(cmd);
            if (!IsPositiveXcpResponse(rx, out string err))
            {
                SetStatus("XCP CONNECT 실패" + (string.IsNullOrEmpty(err) ? "" : $": {err}"));
                _xcpConnected = false;
                return false;
            }

            _xcpConnected = true;
            SetStatus("XCP CONNECT 완료");
            return true;
        }

        private void TryDisconnectXcp()
        {
            if (!_xcpConnected || _gw == null || !_gw.IsConnected) return;

            try
            {
                var cmd = new byte[8];
                cmd[0] = XCP_CMD_DISCONNECT;
                SendXcpCtoWithRetry(cmd);
            }
            catch
            {
            }
            finally
            {
                _xcpConnected = false;
            }
        }

        private bool IsPositiveXcpResponse(LINFrame rx, out string error)
        {
            error = null;
            if (rx == null)
            {
                error = "timeout";
                return false;
            }

            if (rx.DataLength < 1)
            {
                error = "empty CTO";
                return false;
            }

            if (rx.Data[0] == XCP_PID_RES) return true;
            if (rx.Data[0] == XCP_PID_ERR)
            {
                error = rx.DataLength > 1 ? $"ERR=0x{rx.Data[1]:X2}" : "ERR";
                return false;
            }

            error = $"PID=0x{rx.Data[0]:X2}";
            return false;
        }

        private bool XcpShortUpload(uint address, int size, out uint value, out string hexText)
        {
            value = 0;
            hexText = null;

            if (_cjChairId != 0 && ((size < 1) || (size > 4)))
            {
                SetStatus("CJ XCP 단일 요청은 1~4바이트입니다.");
                return false;
            }

            int readLen = Math.Max(1, Math.Min(size > 0 ? size : 4, 4));
            if (!EnsureXcpConnected()) return false;

            var cmd = new byte[8];
            cmd[0] = XCP_CMD_SHORT_UPLOAD;
            cmd[1] = (byte)readLen;
            cmd[2] = 0x00;
            cmd[3] = GetXcpAddrExt();
            cmd[4] = (byte)(address & 0xFF);
            cmd[5] = (byte)((address >> 8) & 0xFF);
            cmd[6] = (byte)((address >> 16) & 0xFF);
            cmd[7] = (byte)((address >> 24) & 0xFF);

            LINFrame rx = null;
            // 전-F 값(0xFF, 0xFFFF 등)은 통신 오류일 수 있으므로 최대 ALL_F_MAX_RETRY회 재시도
            for (int allFTry = 0; allFTry < ALL_F_MAX_RETRY; allFTry++)
            {
                rx = SendXcpCtoWithRetry(cmd);
                if (!IsPositiveXcpResponse(rx, out string err))
                {
                    SetStatus("XCP SHORT_UPLOAD 실패" + (string.IsNullOrEmpty(err) ? "" : $": {err}"));
                    return false;
                }

                uint tmpVal = 0;
                for (int i = 0; i < readLen && (i + 1) < rx.DataLength; i++)
                    tmpVal |= (uint)rx.Data[i + 1] << (8 * i);
                tmpVal = MaskValueForSize(tmpVal, size);

                if (!IsAllFValue(tmpVal, size) || allFTry == ALL_F_MAX_RETRY - 1)
                {
                    // 정상 값이거나 재시도를 모두 소진한 경우
                    value   = tmpVal;
                    hexText = FormatHexValue(value, size);
                    if (allFTry > 0)
                        SetStatus($"XCP READ: 전-F 값 {allFTry+1}회 시도 후 확정 ({hexText})");
                    return true;
                }
                // 전-F 값 → 재시도
                Delay(DIAG_RETRY_DELAY_MS);
            }

            // 여기까지 오면 모든 시도가 전-F (위 루프에서 return)
            value   = MaskValueForSize(0xFFFFFFFF, size);
            hexText = FormatHexValue(value, size);
            return true;
        }

        private bool XcpBlockUpload(uint address, int size, out byte[] data, out string hexText)
        {
            int offset;
            int readSize;
            uint value;

            data = null;
            hexText = null;
            if ((size <= 0) || (size > XCP_MAX_BLOCK_READ))
            {
                SetStatus($"CJ XCP 연속 읽기는 최대 {XCP_MAX_BLOCK_READ}바이트입니다.");
                return false;
            }

            data = new byte[size];
            for (offset = 0; offset < size; offset += 4)
            {
                readSize = Math.Min(4, size - offset);
                if (!XcpShortUpload(address + (uint)offset, readSize, out value, out string _))
                {
                    return false;
                }
                for (int index = 0; index < readSize; index++)
                {
                    data[offset + index] = (byte)(value >> (index * 8));
                }
                SetStatus($"XCP READ {offset + readSize}/{size} bytes");
                Application.DoEvents();
            }
            hexText = BitConverter.ToString(data).Replace("-", " ");
            return true;
        }

        private bool XcpWriteMemory(uint address, int size, uint rawValue)
        {
            int writeLen = size <= 1 ? 1 : (size == 2 ? 2 : 4);
            writeLen = Math.Min(writeLen, 4);
            if (!EnsureXcpConnected()) return false;

            var setMta = new byte[8];
            setMta[0] = XCP_CMD_SET_MTA;
            setMta[1] = 0x00;
            setMta[2] = 0x00;
            setMta[3] = GetXcpAddrExt();
            setMta[4] = (byte)(address & 0xFF);
            setMta[5] = (byte)((address >> 8) & 0xFF);
            setMta[6] = (byte)((address >> 16) & 0xFF);
            setMta[7] = (byte)((address >> 24) & 0xFF);

            LINFrame setMtaRx = SendXcpCtoWithRetry(setMta);
            if (!IsPositiveXcpResponse(setMtaRx, out string setErr))
            {
                SetStatus("XCP SET_MTA 실패" + (string.IsNullOrEmpty(setErr) ? "" : $": {setErr}"));
                return false;
            }

            var download = new byte[8];
            download[0] = XCP_CMD_DOWNLOAD;
            download[1] = (byte)writeLen;
            for (int i = 0; i < writeLen; i++)
                download[2 + i] = (byte)((rawValue >> (8 * i)) & 0xFF);

            LINFrame dlRx = SendXcpCtoWithRetry(download);
            if (!IsPositiveXcpResponse(dlRx, out string dlErr))
            {
                SetStatus("XCP DOWNLOAD 실패" + (string.IsNullOrEmpty(dlErr) ? "" : $": {dlErr}"));
                return false;
            }

            return true;
        }

        private byte[] BuildReadRam(byte nad, uint addr)
        {
            return new byte[]
            {
                nad, 0x06, 0xB4, CMD_READ_RAM,
                (byte)(addr & 0xFF), (byte)((addr >> 8) & 0xFF),
                (byte)((addr >> 16) & 0xFF), (byte)((addr >> 24) & 0xFF)
            };
        }

        private byte[] BuildWriteRam(byte nad, uint addr, byte d0, byte d1)
        {
            return new byte[]
            {
                nad, 0x06, 0xB4, CMD_WRITE_RAM,
                (byte)(addr & 0xFF), (byte)((addr >> 8) & 0xFF), d0, d1
            };
        }

        // =========================================================================
        // 파일 열기 – .out / .map 자동 판별
        // =========================================================================
        private void btnLoadMap_Click(object sender, EventArgs e)
        {
            using (var dlg = new OpenFileDialog())
            {
                dlg.Filter = "심볼 파일 (*.out;*.map)|*.out;*.map|ELF 바이너리 (*.out)|*.out|IAR MAP (*.map)|*.map|모든 파일 (*.*)|*.*";
                dlg.Title  = "심볼 파일 선택 (.out 또는 .map)";
                if (dlg.ShowDialog() != DialogResult.OK) return;
                _filePath = dlg.FileName;
            }

            string ext = Path.GetExtension(_filePath).ToLower();
            if (ext == ".out" || ext == ".elf" || ext == ".axf")
                LoadOutFile(_filePath);
            else
                LoadMapFile(_filePath);
        }

        // =========================================================================
        // .out (ELF) 파싱
        //   방법1: arm-none-eabi-nm or nm 호출 → stdout 파싱
        //   방법2: ELF 헤더를 직접 읽어 심볼 테이블 추출 (fallback)
        // =========================================================================
        private void LoadOutFile(string path)
        {
            try
            {
                _symbols.Clear();

                // ── 방법1: nm 호출 ───────────────────────────────────────
                string[] nmCandidates = new[]
                {
                    "arm-none-eabi-nm", "arm-none-eabi-nm.exe",
                    "nm", "nm.exe"
                };

                string nmOutput = null;
                foreach (string nm in nmCandidates)
                {
                    nmOutput = TryRunNm(nm, path);
                    if (nmOutput != null) break;
                }

                if (nmOutput != null)
                {
                    ParseNmOutput(nmOutput);
                }
                else
                {
                    // ── 방법2: ELF 직접 파싱 ────────────────────────────
                    ParseElfDirect(path);
                }

                TryAppendDwarfMembers(path);
                FinishLoad(path);
            }
            catch (Exception ex)
            {
                lblMapStatus.Text      = "✘ .out 파일 파싱 실패: " + ex.Message;
                lblMapStatus.ForeColor = Color.Red;
            }
        }

        private string TryRunNm(string nm, string elfPath)
        {
            try
            {
                var psi = new System.Diagnostics.ProcessStartInfo
                {
                    FileName               = nm,
                    // --print-size : 크기 컬럼 출력  --size-sort : 선택적
                    Arguments              = $"--print-size --radix=x \"{elfPath}\"",
                    UseShellExecute        = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError  = true,
                    CreateNoWindow         = true
                };
                using (var proc = System.Diagnostics.Process.Start(psi))
                {
                    string output = proc.StandardOutput.ReadToEnd();
                    proc.WaitForExit(5000);
                    return string.IsNullOrWhiteSpace(output) ? null : output;
                }
            }
            catch { return null; }
        }

        // nm 출력 형식: "00000000 00000004 B variable_name   [file.o]"
        //   address  size  type  name
        //   type: B/b=BSS  D/d=Data  (RAM 변수)
        private void ParseNmOutput(string output)
        {
            // GNU nm --print-size --radix=x:
            //   xxxxxxxx xxxxxxxx [BbDdRrAaT] name\t[file]
            var re = new Regex(
                @"^([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([BbDd])\s+(\S+)",
                RegexOptions.Multiline | RegexOptions.Compiled);

            foreach (Match m in re.Matches(output))
            {
                uint addr = Convert.ToUInt32(m.Groups[1].Value, 16);
                int  size = (int)Convert.ToUInt32(m.Groups[2].Value, 16);
                string name = m.Groups[4].Value;

                // RAM 범위 필터 (STM32: 0x2000_0000 ~ 0x2000_FFFF)
                if (addr < 0x20000000 || addr > 0x2000FFFF) continue;
                if (size == 0) continue;
                // 컴파일러 내부 심볼 제거
                if (name.StartsWith("__") || name.StartsWith("$")) continue;

                _symbols.Add(new MapSymbol
                {
                    Name    = name,
                    Address = addr,
                    Size    = size,
                    Source  = "nm"
                });
            }
        }

        // ELF 직접 파싱 (nm이 없을 때 fallback)
        // 32bit Little-Endian ELF만 지원
        private void ParseElfDirect(string path)
        {
            byte[] data = File.ReadAllBytes(path);

            // ELF 매직 확인
            if (data.Length < 52 || data[0] != 0x7F || data[1] != 'E' ||
                data[2] != 'L' || data[3] != 'F')
                throw new Exception("ELF 파일이 아닙니다.");

            bool le = data[5] == 1; // Little-Endian
            if (!le) throw new Exception("Big-Endian ELF는 지원되지 않습니다.");

            uint ReadU32(int offset) =>
                (uint)(data[offset] | data[offset+1]<<8 | data[offset+2]<<16 | data[offset+3]<<24);
            ushort ReadU16(int offset) =>
                (ushort)(data[offset] | data[offset+1]<<8);

            // ELF 헤더
            uint  e_shoff     = ReadU32(32);   // 섹션 헤더 테이블 오프셋
            ushort e_shentsize = ReadU16(46);  // 섹션 헤더 크기
            ushort e_shnum     = ReadU16(48);  // 섹션 헤더 개수
            ushort e_shstrndx  = ReadU16(50);  // 섹션 이름 문자열 테이블 인덱스

            // 섹션 헤더 읽기
            var sections = new List<(string name, uint type, uint offset, uint size, uint addr, uint link, uint entsize)>();
            for (int i = 0; i < e_shnum; i++)
            {
                int sh = (int)(e_shoff + i * e_shentsize);
                if (sh + 40 > data.Length) break;
                uint nameIdx = ReadU32(sh);
                uint type    = ReadU32(sh + 4);
                uint saddr   = ReadU32(sh + 12);
                uint offset  = ReadU32(sh + 16);
                uint size    = ReadU32(sh + 20);
                uint link    = ReadU32(sh + 24);
                uint entsize = ReadU32(sh + 36);
                sections.Add(("", type, offset, size, saddr, link, entsize));
            }

            // 섹션 이름 문자열 테이블 (shstrtab)
            string ReadStr(byte[] buf, int start)
            {
                int end = start;
                while (end < buf.Length && buf[end] != 0) end++;
                return System.Text.Encoding.ASCII.GetString(buf, start, end - start);
            }

            var shstrSec = sections[e_shstrndx];
            byte[] shstrBuf = new byte[shstrSec.size];
            Array.Copy(data, shstrSec.offset, shstrBuf, 0, (int)shstrSec.size);

            // 섹션 이름을 제대로 가져오기
            for (int i = 0; i < e_shnum; i++)
            {
                int sh = (int)(e_shoff + i * e_shentsize);
                uint nameIdx = ReadU32(sh);
                string secName = nameIdx < shstrBuf.Length ? ReadStr(shstrBuf, (int)nameIdx) : "";
                var s = sections[i];
                sections[i] = (secName, s.type, s.offset, s.size, s.addr, s.link, s.entsize);
            }

            // SYMTAB 또는 DYNSYM 섹션 찾기 (type=2 SYMTAB, type=11 DYNSYM)
            var symtab = sections.FirstOrDefault(s => s.type == 2);
            if (symtab.type == 0) symtab = sections.FirstOrDefault(s => s.type == 11);
            if (symtab.type == 0) throw new Exception("ELF 심볼 테이블을 찾을 수 없습니다. 스트립(strip)된 바이너리입니다.");

            // 심볼 이름 문자열 테이블 (strtab)
            int strtabIdx = (int)symtab.link;
            var strtabSec = sections[strtabIdx];
            byte[] strtabBuf = new byte[strtabSec.size];
            Array.Copy(data, strtabSec.offset, strtabBuf, 0, (int)strtabSec.size);

            // 심볼 엔트리 파싱 (Elf32_Sym: 16바이트)
            // st_name(4) st_value(4) st_size(4) st_info(1) st_other(1) st_shndx(2)
            uint entSize = symtab.entsize > 0 ? symtab.entsize : 16;
            int symCount = (int)(symtab.size / entSize);
            for (int i = 0; i < symCount; i++)
            {
                int soff = (int)(symtab.offset + i * entSize);
                if (soff + 16 > data.Length) break;

                uint nameOff = ReadU32(soff);
                uint value   = ReadU32(soff + 4);
                uint size    = ReadU32(soff + 8);
                byte info    = data[soff + 12];
                // byte type  = (byte)(info & 0x0F);  // 1=OBJECT, 2=FUNC
                // byte bind  = (byte)(info >> 4);     // 1=GLOBAL, 0=LOCAL

                byte symType = (byte)(info & 0x0F);
                byte symBind = (byte)(info >> 4);

                // OBJECT 타입(변수)만, LOCAL/GLOBAL 모두
                if (symType != 1) continue;
                if (size == 0) continue;
                if (value < 0x20000000 || value > 0x2000FFFF) continue;

                string symName = nameOff < strtabBuf.Length ? ReadStr(strtabBuf, (int)nameOff) : "";
                if (string.IsNullOrEmpty(symName)) continue;
                if (symName.StartsWith("__") || symName.StartsWith("$")) continue;

                _symbols.Add(new MapSymbol
                {
                    Name    = symName,
                    Address = value,
                    Size    = (int)size,
                    Source  = "ELF"
                });
            }
        }

        private void TryAppendDwarfMembers(string path)
        {
            try
            {
                var sections = ReadElfSections(path);
                var reader = new DwarfReader(sections);
                if (!reader.CanParse) return;

                List<DwarfVariableInfo> variables = reader.ParseVariables();
                int added = 0;
                foreach (var variable in variables)
                {
                    if (_cjChairId != 0 && !_symbols.Any(s => s.Name == variable.Name && s.Address == variable.Address)) continue;
                    if (!_symbols.Any(s => s.Address == variable.Address && s.Name == variable.Name))
                    {
                        _symbols.Add(new MapSymbol
                        {
                            Name = variable.Name,
                            Address = variable.Address,
                            Size = variable.Type?.ByteSize ?? 0,
                            Source = "DWARF"
                        });
                        added++;
                    }

                    added += ExpandDwarfMembers(variable.Name, variable.Address, variable.Type, 0);
                }

                if (added > 0) SetStatus($"DWARF 멤버 확장: {added}개 추가");
            }
            catch
            {
                // 디버그 정보가 없는 .out 파일도 있으므로 조용히 fallback
            }
        }

        private int ExpandDwarfMembers(string prefix, uint baseAddress, DwarfTypeInfo type, int depth)
        {
            if (type == null || depth > 8) return 0;

            int added = 0;
            if ((type.Kind == "struct" || type.Kind == "union") && type.Members.Count > 0)
            {
                foreach (var member in type.Members)
                {
                    if (member.Type == null) continue;

                    uint memberAddress = baseAddress + (uint)Math.Max(0, member.Offset);
                    string memberName = string.IsNullOrWhiteSpace(prefix) ? member.Name : $"{prefix}.{member.Name}";
                    if (!_symbols.Any(s => s.Address == memberAddress && s.Name == memberName))
                    {
                        _symbols.Add(new MapSymbol
                        {
                            Name = memberName,
                            Address = memberAddress,
                            Size = member.Type.ByteSize,
                            Source = "DWARF"
                        });
                        added++;
                    }

                    added += ExpandDwarfMembers(memberName, memberAddress, member.Type, depth + 1);
                }
            }
            else if (type.Kind == "array" && type.ElementType != null && type.ArrayCount > 0 && type.ArrayCount <= MAX_ARRAY_EXPANSION)
            {
                int elementSize = type.ElementType.ByteSize;
                if (elementSize <= 0) return added;

                for (int i = 0; i < type.ArrayCount; i++)
                {
                    uint elementAddress = baseAddress + (uint)(i * elementSize);
                    string elementName = $"{prefix}[{i}]";
                    if (!_symbols.Any(s => s.Address == elementAddress && s.Name == elementName))
                    {
                        _symbols.Add(new MapSymbol
                        {
                            Name = elementName,
                            Address = elementAddress,
                            Size = elementSize,
                            Source = "DWARF"
                        });
                        added++;
                    }

                    added += ExpandDwarfMembers(elementName, elementAddress, type.ElementType, depth + 1);
                }
            }

            return added;
        }

        private Dictionary<string, byte[]> ReadElfSections(string path)
        {
            byte[] data = File.ReadAllBytes(path);
            if (data.Length < 52 || data[0] != 0x7F || data[1] != 'E' || data[2] != 'L' || data[3] != 'F')
                throw new Exception("ELF 파일이 아닙니다.");
            if (data[5] != 1) throw new Exception("Little-Endian ELF만 지원합니다.");

            uint ReadU32(int offset) =>
                (uint)(data[offset] | data[offset + 1] << 8 | data[offset + 2] << 16 | data[offset + 3] << 24);
            ushort ReadU16(int offset) =>
                (ushort)(data[offset] | data[offset + 1] << 8);

            uint e_shoff = ReadU32(32);
            ushort e_shentsize = ReadU16(46);
            ushort e_shnum = ReadU16(48);
            ushort e_shstrndx = ReadU16(50);

            var rawSections = new List<(uint NameIndex, uint Offset, uint Size)>();
            for (int i = 0; i < e_shnum; i++)
            {
                int sh = (int)(e_shoff + i * e_shentsize);
                if (sh + 40 > data.Length) break;
                rawSections.Add((ReadU32(sh), ReadU32(sh + 16), ReadU32(sh + 20)));
            }

            if (e_shstrndx >= rawSections.Count) return new Dictionary<string, byte[]>();
            var shstr = rawSections[e_shstrndx];
            byte[] shstrBuf = new byte[shstr.Size];
            Array.Copy(data, shstr.Offset, shstrBuf, 0, (int)shstr.Size);

            string ReadStr(byte[] buf, int start)
            {
                int end = start;
                while (end < buf.Length && buf[end] != 0) end++;
                return Encoding.ASCII.GetString(buf, start, end - start);
            }

            var sections = new Dictionary<string, byte[]>(StringComparer.Ordinal);
            foreach (var section in rawSections)
            {
                string name = section.NameIndex < shstrBuf.Length ? ReadStr(shstrBuf, (int)section.NameIndex) : null;
                if (string.IsNullOrEmpty(name) || section.Offset + section.Size > data.Length) continue;

                byte[] body = new byte[section.Size];
                Array.Copy(data, section.Offset, body, 0, (int)section.Size);
                sections[name] = body;
            }

            return sections;
        }

        // =========================================================================
        // .map (IAR / GCC) 파싱
        // =========================================================================
        private void LoadMapFile(string path)
        {
            try
            {
                _symbols.Clear();
                string[] lines = File.ReadAllLines(path);

                // IAR MAP: "SymbolName  0x2000'xxxx  0xSIZE  Data  Gb  file.o [N]"
                var reIAR = new Regex(
                    @"^(\S+)\s+(0x[\dA-Fa-f']+)\s+(0x[\dA-Fa-f]+)\s+Data\s+Gb\s+(\S+)",
                    RegexOptions.Compiled);

                // GCC MAP: " 0x0000000020000xxx  variableName"  (in .bss / .data sections)
                var reGCC = new Regex(
                    @"^\s+(0x[0-9a-fA-F]{8,})\s+([A-Za-z_]\w*)\s*$",
                    RegexOptions.Compiled);

                string pendingSymbol = null;
                bool gccMode = lines.Any(l => l.TrimStart().StartsWith(".bss") || l.TrimStart().StartsWith(".data"));

                foreach (string rawLine in lines)
                {
                    string line = rawLine.TrimEnd('\r', '\n');

                    // IAR 형식
                    Match m = reIAR.Match(line);
                    if (m.Success)
                    {
                        AddParsedSymbol(m.Groups[1].Value, m.Groups[2].Value,
                                        m.Groups[3].Value, m.Groups[4].Value);
                        pendingSymbol = null;
                        continue;
                    }

                    // IAR 멀티라인 (이름만 있는 줄 → 다음 줄에 주소)
                    if (pendingSymbol != null)
                    {
                        var re2 = new Regex(
                            @"^\s+(0x[\dA-Fa-f']+)\s+(0x[\dA-Fa-f]+)\s+Data\s+Gb\s+(\S+)",
                            RegexOptions.Compiled);
                        Match m2 = re2.Match(line);
                        if (m2.Success)
                            AddParsedSymbol(pendingSymbol, m2.Groups[1].Value,
                                            m2.Groups[2].Value, m2.Groups[3].Value);
                        pendingSymbol = null;
                        continue;
                    }

                    var reNameOnly = new Regex(@"^([A-Za-z_][\w:]+)\s*$", RegexOptions.Compiled);
                    Match mn = reNameOnly.Match(line);
                    if (mn.Success) { pendingSymbol = mn.Groups[1].Value; continue; }

                    // GCC MAP 형식
                    if (gccMode)
                    {
                        Match mg = reGCC.Match(line);
                        if (mg.Success)
                        {
                            string addrStr = mg.Groups[1].Value;
                            string name    = mg.Groups[2].Value;
                            if (uint.TryParse(addrStr.Substring(2),
                                System.Globalization.NumberStyles.HexNumber, null, out uint addr))
                            {
                                if (addr >= 0x20000000 && addr <= 0x2000FFFF &&
                                    !name.StartsWith("__"))
                                {
                                    _symbols.Add(new MapSymbol
                                    {
                                        Name    = name,
                                        Address = addr,
                                        Size    = 0,   // GCC MAP에는 크기 정보 없음
                                        Source  = "MAP"
                                    });
                                }
                            }
                        }
                    }
                }

                if (_cjChairId != 0)
                {
                    string fileName = Path.GetFileNameWithoutExtension(path) + ".out";
                    string pairedOut = Path.Combine(Path.GetDirectoryName(path), fileName);
                    if (!File.Exists(pairedOut))
                    {
                        DirectoryInfo parent = Directory.GetParent(Path.GetDirectoryName(path));
                        if (parent != null) pairedOut = Path.Combine(parent.FullName, "Exe", fileName);
                    }
                    if (File.Exists(pairedOut)) TryAppendDwarfMembers(pairedOut);
                }
                FinishLoad(path);
            }
            catch (Exception ex)
            {
                lblMapStatus.Text      = "✘ MAP 파일 파싱 실패: " + ex.Message;
                lblMapStatus.ForeColor = Color.Red;
            }
        }

        private void AddParsedSymbol(string name, string addrStr, string sizeStr, string source)
        {
            string cleanAddr = addrStr.Replace("'", "");
            if (!cleanAddr.StartsWith("0x", StringComparison.OrdinalIgnoreCase)) return;
            if (!uint.TryParse(cleanAddr.Substring(2), System.Globalization.NumberStyles.HexNumber,
                               null, out uint addr)) return;
            if (!int.TryParse(sizeStr.Substring(2), System.Globalization.NumberStyles.HexNumber,
                               null, out int size)) return;
            if (addr < 0x20000000 || addr > 0x2000FFFF) return;

            _symbols.Add(new MapSymbol { Name = name, Address = addr, Size = size, Source = source });
        }

        private void FinishLoad(string path)
        {
            if (_cjChairId != 0)
            {
                _symbols = _symbols.Where(s => s.Name == "gOSPControl" || s.Name.StartsWith("gOSPControl.") || s.Name == "gSRGBControl" || s.Name.StartsWith("gSRGBControl.") || s.Name == "gPowerData" || s.Name.StartsWith("gPowerData.")).ToList();
            }
            // 중복 제거 (이름 + 주소 + 크기 기준)
            _symbols = _symbols
                .GroupBy(s => new { s.Name, s.Address, s.Size })
                .Select(g => g.OrderByDescending(s => s.Size).First())
                .OrderBy(s => s.Address)
                .ThenBy(s => s.Name)
                .ToList();

            _filtered = new List<MapSymbol>(_symbols);
            RefreshSymbolList();

            string fname = Path.GetFileName(path);
            string ext   = Path.GetExtension(path).ToUpper();
            lblMapStatus.Text      = $"✔ [{ext}] {fname}  —  변수 {_symbols.Count}개 로드됨";
            lblMapStatus.ForeColor = Color.LimeGreen;
            SetStatus($"로드 완료: {_symbols.Count}개 변수");
        }

        // =========================================================================
        // 목록 갱신
        // =========================================================================
        private void RefreshSymbolList()
        {
            dgvSymbols.Rows.Clear();
            foreach (var s in _filtered)
            {
                int idx = dgvSymbols.Rows.Add(s.Name, s.AddressHex, s.SizeStr, s.Value ?? "");
                dgvSymbols.Rows[idx].Tag = s;
            }
            lblCount.Text = $"{_filtered.Count} / {_symbols.Count}개";
        }

        // =========================================================================
        // 검색 / 필터
        // =========================================================================
        private void txtSearch_TextChanged(object sender, EventArgs e)
        {
            string kw = txtSearch.Text.Trim().ToLower();
            _filtered = string.IsNullOrEmpty(kw)
                ? new List<MapSymbol>(_symbols)
                : _symbols.Where(s => s.Name.ToLower().Contains(kw) ||
                                      s.AddressHex.ToLower().Contains(kw)).ToList();
            RefreshSymbolList();
        }

        // =========================================================================
        // 선택 → 상세
        // =========================================================================
        private void dgvSymbols_SelectionChanged(object sender, EventArgs e)
        {
            if (dgvSymbols.SelectedRows.Count == 0) return;
            if (!(dgvSymbols.SelectedRows[0].Tag is MapSymbol s)) return;

            lblSelName.Text   = s.Name;
            lblSelAddr.Text   = s.AddressHex;
            lblSelSize.Text   = s.Size > 0 ? $"{s.Size} bytes" : "알 수 없음";
            lblCurrentVal.Text = string.IsNullOrEmpty(s.Value) ? "--" : s.Value;
        }

        /// <summary>읽은 값이 0xFF / 0xFFFF / 0xFFFFFFFF 처럼 전부 F인지 판단</summary>
        private static bool IsAllFValue(uint value, int size)
        {
            if (size <= 1) return (value & 0xFF)     == 0xFF;
            if (size == 2) return (value & 0xFFFF)   == 0xFFFF;
            if (size == 3) return (value & 0xFFFFFF) == 0xFFFFFF;
            return value == 0xFFFFFFFF;
        }

        private static uint MaskValueForSize(uint raw, int size)
        {
            if (size <= 1) return raw & 0xFF;
            if (size == 2) return raw & 0xFFFF;
            if (size == 3) return raw & 0xFFFFFF;
            return raw;
        }

        private static string FormatHexValue(uint value, int size)
        {
            if (size <= 1) return $"0x{value & 0xFF:X2}";
            if (size == 2) return $"0x{value & 0xFFFF:X4}";
            if (size == 3) return $"0x{value & 0xFFFFFF:X6}";
            return $"0x{value:X8}";
        }

        // =========================================================================
        // READ
        // =========================================================================
        private void btnRead_Click(object sender, EventArgs e)
        {
            var s = GetSelectedSymbol();
            if (s == null) return;

            UpdateConnectionStatus();
            if (_gw == null || !_gw.IsConnected) { SetStatus("연결 안됨"); return; }

            if (UseXcpMode)
            {
                SetStatus($"XCP READ {s.Name} @ {s.AddressHex} ...");
                if (s.Size > 4)
                {
                    if (!XcpBlockUpload(s.Address, s.Size, out byte[] blockData, out string blockHex))
                    {
                        lblCurrentVal.Text = "xcp err";
                        return;
                    }
                    lblCurrentVal.Text = blockHex;
                    foreach (DataGridViewRow row in dgvSymbols.Rows)
                    {
                        if (!(row.Tag is MapSymbol rs))
                        {
                            continue;
                        }
                        if ((rs.Name != s.Name) && !rs.Name.StartsWith(s.Name + "[") && !rs.Name.StartsWith(s.Name + "."))
                        {
                            continue;
                        }
                        if ((rs.Size <= 0) || (rs.Address < s.Address) || (((ulong)rs.Address + (ulong)rs.Size) > ((ulong)s.Address + (ulong)s.Size)))
                        {
                            continue;
                        }
                        int dataOffset = (int)(rs.Address - s.Address);
                        if (rs.Size <= 4)
                        {
                            uint rowValue = 0U;
                            for (int index = 0; index < rs.Size; index++)
                            {
                                rowValue |= (uint)blockData[dataOffset + index] << (index * 8);
                            }
                            rs.Value = FormatHexValue(rowValue, rs.Size);
                            if (rs.Name.Contains("temperatureC") && (rs.Size == 2))
                            {
                                rs.Value = $"{rs.Value}  ({unchecked((short)rowValue)} °C)";
                            }
                        }
                        else
                        {
                            rs.Value = BitConverter.ToString(blockData, dataOffset, rs.Size).Replace("-", " ");
                        }
                        row.Cells["colValue"].Value = rs.Value;
                    }
                    SetStatus($"OK XCP READ {s.Name} ({s.Size} bytes)");
                    return;
                }
                if (!XcpShortUpload(s.Address, s.Size, out uint xcpValue, out string xcpHex))
                {
                    lblCurrentVal.Text = "xcp err";
                    return;
                }

                s.Value = _cjChairId != 0 && s.Name.Contains("temperatureC") && s.Size == 2 ? $"{xcpHex}  ({unchecked((short)xcpValue)} °C)" : $"{xcpHex}  ({xcpValue})";
                lblCurrentVal.Text = s.Value;
                foreach (DataGridViewRow row in dgvSymbols.Rows)
                    if (row.Tag is MapSymbol rs && rs.Address == s.Address && rs.Name == s.Name)
                    {
                        row.Cells["colValue"].Value = s.Value;
                        break;
                    }

                SetStatus($"OK XCP READ {s.Name} = {xcpHex}");
                return;
            }

            byte nad = GetNAD();
            SetStatus($"READ {s.Name} @ {s.AddressHex} …");

            // 응답: [NAD][07][F4][AB][D0][D1][D2][D3]
            // 전-F 값은 통신 오류일 수 있으므로 최대 ALL_F_MAX_RETRY회 재시도
            LINFrame rx = null;
            uint visible = 0;
            string hex = "--";
            for (int allFTry = 0; allFTry < ALL_F_MAX_RETRY; allFTry++)
            {
                rx = SendDiagWithRetry(nad, BuildReadRam(nad, s.Address));
                if (rx == null) { SetStatus($"✘ 응답 없음 ({s.Name})"); lblCurrentVal.Text = "timeout"; return; }
                uint raw = (uint)(rx.Data[4] | rx.Data[5] << 8 | rx.Data[6] << 16 | rx.Data[7] << 24);
                visible = MaskValueForSize(raw, s.Size);
                hex     = FormatHexValue(visible, s.Size);
                if (!IsAllFValue(visible, s.Size) || allFTry == ALL_F_MAX_RETRY - 1)
                {
                    if (allFTry > 0)
                        SetStatus($"READ: 전-F 값 {allFTry+1}회 시도 후 확정 ({hex})");
                    break;
                }
                Delay(DIAG_RETRY_DELAY_MS);
            }
            string dec = visible.ToString();
            lblCurrentVal.Text = $"{hex}  ({dec})";

            // 목록에도 반영
            foreach (DataGridViewRow row in dgvSymbols.Rows)
                if (row.Tag is MapSymbol rs && rs.Address == s.Address && rs.Name == s.Name)
                {
                    row.Cells["colValue"].Value = hex;
                    break;
                }

            SetStatus($"✔ READ {s.Name} = {hex}");
        }

        // =========================================================================
        // WRITE
        // =========================================================================
        private void btnWrite_Click(object sender, EventArgs e)
        {
            var s = GetSelectedSymbol();
            if (s == null) return;

            UpdateConnectionStatus();
            if (_gw == null || !_gw.IsConnected) { SetStatus("연결 안됨"); return; }

            string raw = txtWriteVal.Text.Trim();
            if (string.IsNullOrEmpty(raw)) { SetStatus("쓸 값을 입력하세요"); return; }

            uint val;
            if (raw.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            {
                if (!uint.TryParse(raw.Substring(2),
                    System.Globalization.NumberStyles.HexNumber, null, out val))
                { SetStatus("HEX 값 파싱 실패"); return; }
            }
            else
            {
                if (!uint.TryParse(raw, out val))
                { SetStatus("DEC 값 파싱 실패"); return; }
            }

            if (UseXcpMode)
            {
                int writeSize = s.Size > 0 ? s.Size : 4;
                uint masked = MaskValueForSize(val, writeSize);
                SetStatus($"XCP WRITE {s.Name} @ {s.AddressHex} <- {FormatHexValue(masked, writeSize)} ...");
                // XCP WRITE 후 확인 READ → 값 불일치 시 최대 ALL_F_MAX_RETRY회 재시도
                for (int xwTry = 0; xwTry < ALL_F_MAX_RETRY; xwTry++)
                {
                    if (!XcpWriteMemory(s.Address, writeSize, masked)) return;
                    Delay(20);
                    if (!XcpShortUpload(s.Address, writeSize, out uint xVerify, out string xVerifyHex)) break;
                    uint xWriteVal = MaskValueForSize(masked, writeSize);
                    lblCurrentVal.Text = $"{xVerifyHex}  ({xVerify})";
                    foreach (DataGridViewRow row in dgvSymbols.Rows)
                        if (row.Tag is MapSymbol rs && rs.Address == s.Address && rs.Name == s.Name)
                        { row.Cells["colValue"].Value = xVerifyHex; break; }
                    if (xVerify == xWriteVal)
                    {
                        SetStatus(xwTry == 0
                            ? $"OK XCP WRITE {s.Name} = {FormatHexValue(masked, writeSize)}"
                            : $"OK XCP WRITE {s.Name} = {FormatHexValue(masked, writeSize)} ({xwTry+1}회 시도)");
                        break;
                    }
                    if (xwTry < ALL_F_MAX_RETRY - 1)
                        SetStatus($"⚠ XCP WRITE 불일치 재시도 [{xwTry+2}/{ALL_F_MAX_RETRY}]");
                    else
                        SetStatus($"✘ XCP WRITE 실패: {ALL_F_MAX_RETRY}회 후에도 불일치");
                    Delay(DIAG_RETRY_DELAY_MS);
                }
                return;
            }

            if (val > 0xFFFF)
            {
                if (MessageBox.Show($"값이 0xFFFF를 초과합니다 (0x{val:X}).\nWRITE는 최대 2바이트만 지원합니다.\n계속하시겠습니까?",
                    "경고", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes)
                    return;
            }

            byte d0 = (byte)(val & 0xFF);
            byte d1 = (byte)((val >> 8) & 0xFF);

            byte nad = GetNAD();
            if (s.Size == 1)
            {
                LINFrame preRead = SendDiagWithRetry(nad, BuildReadRam(nad, s.Address));
                if (preRead == null) { SetStatus($"✘ WRITE 전 보존 READ 실패 ({s.Name})"); return; }
                d1 = preRead.Data[5];
            }

            SetStatus($"WRITE {s.Name} @ {s.AddressHex} ← 0x{val:X4} …");

            // WRITE 후 확인 READ → 값이 변경 안 됐으면 최대 ALL_F_MAX_RETRY회 재시도
#pragma warning disable CS0219
            bool writeSuccess = false;
#pragma warning restore CS0219
            for (int wTry = 0; wTry < ALL_F_MAX_RETRY; wTry++)
            {
                LINFrame wRx = SendDiagWithRetry(nad, BuildWriteRam(nad, s.Address, d0, d1));
                if (wRx == null)
                { SetStatus($"✘ WRITE 응답 없음 ({s.Name}) [{wTry+1}/{ALL_F_MAX_RETRY}]"); break; }

                // 확인 READ
                Delay(20);
                LINFrame verifyRx = SendDiagWithRetry(nad, BuildReadRam(nad, s.Address));
                if (verifyRx == null)
                { SetStatus($"✘ WRITE 확인 READ 실패 ({s.Name})"); break; }

                uint readBack = (uint)(verifyRx.Data[4] | verifyRx.Data[5] << 8
                                     | verifyRx.Data[6] << 16 | verifyRx.Data[7] << 24);
                uint readVisible = MaskValueForSize(readBack, s.Size);
                uint writeVal    = MaskValueForSize(val, s.Size);

                // 현재값 UI 즉시 갱신
                string readHex = FormatHexValue(readVisible, s.Size);
                lblCurrentVal.Text = $"{readHex}  ({readVisible})";
                foreach (DataGridViewRow row in dgvSymbols.Rows)
                    if (row.Tag is MapSymbol rs && rs.Address == s.Address && rs.Name == s.Name)
                    { row.Cells["colValue"].Value = readHex; break; }

                if (readVisible == writeVal)
                {
                    SetStatus(wTry == 0
                        ? $"✔ WRITE {s.Name} = 0x{val:X4} 완료"
                        : $"✔ WRITE {s.Name} = 0x{val:X4} 완료 ({wTry+1}회 시도)");
                    writeSuccess = true;
                    break;
                }

                // 값 불일치 → 재시도
                if (wTry < ALL_F_MAX_RETRY - 1)
                    SetStatus($"⚠ WRITE 값 불일치 재시도 [{wTry+2}/{ALL_F_MAX_RETRY}] ({s.Name}: 기대=0x{writeVal:X} 실제=0x{readVisible:X})");
                else
                    SetStatus($"✘ WRITE 실패 ({s.Name}): {ALL_F_MAX_RETRY}회 후에도 값 불일치 (기대=0x{writeVal:X} 실제=0x{readVisible:X})");
                Delay(DIAG_RETRY_DELAY_MS);
            }
        }

        // =========================================================================
        // 즐겨찾기
        // =========================================================================
        private void btnFavAdd_Click(object sender, EventArgs e)
        {
            var s = GetSelectedSymbol();
            if (s == null) return;
            if (_favorites.Any(f => f.Address == s.Address && f.Name == s.Name)) { SetStatus("이미 즐겨찾기에 있습니다"); return; }
            _favorites.Add(s);
            dgvFavorites.Rows.Add(s.Name, s.AddressHex, s.SizeStr, "");
            dgvFavorites.Rows[dgvFavorites.Rows.Count - 1].Tag = s;
            SetStatus($"즐겨찾기 추가: {s.Name}");
        }

        private void btnFavRemove_Click(object sender, EventArgs e)
        {
            if (dgvFavorites.SelectedRows.Count == 0) return;
            var row = dgvFavorites.SelectedRows[0];
            if (row.Tag is MapSymbol s) _favorites.RemoveAll(f => f.Address == s.Address && f.Name == s.Name);
            dgvFavorites.Rows.Remove(row);
        }

        private void btnFavReadAll_Click(object sender, EventArgs e)
        {
            if (_gw == null || !_gw.IsConnected) { SetStatus("연결 안됨"); return; }
            byte nad = GetNAD();
            foreach (DataGridViewRow row in dgvFavorites.Rows)
            {
                if (!(row.Tag is MapSymbol s)) continue;
                if (UseXcpMode)
                {
                    string xcpHex;
                    bool readOk = s.Size > 4 ? XcpBlockUpload(s.Address, s.Size, out byte[] _, out xcpHex) : XcpShortUpload(s.Address, s.Size, out uint _, out xcpHex);
                    if (readOk)
                    {
                        s.Value = xcpHex;
                        row.Cells["favColValue"].Value = xcpHex;
                    }
                    else
                    {
                        row.Cells["favColValue"].Value = "ERR";
                    }
                    Application.DoEvents();
                    continue;
                }

                LINFrame rx = SendDiagWithRetry(nad, BuildReadRam(nad, s.Address));
                if (rx != null)
                {
                    uint raw = (uint)(rx.Data[4] | rx.Data[5] << 8 | rx.Data[6] << 16 | rx.Data[7] << 24);
                    row.Cells["favColValue"].Value = FormatHexValue(MaskValueForSize(raw, s.Size), s.Size);
                }
                else
                {
                    row.Cells["favColValue"].Value = "ERR";
                }
                Application.DoEvents();
            }
            SetStatus("즐겨찾기 전체 READ 완료");
        }

        private void dgvFavorites_SelectionChanged(object sender, EventArgs e)
        {
            if (dgvFavorites.SelectedRows.Count == 0) return;
            if (!(dgvFavorites.SelectedRows[0].Tag is MapSymbol s)) return;

            // 심볼 목록에서도 해당 행 선택
            foreach (DataGridViewRow row in dgvSymbols.Rows)
            {
                if (!(row.Tag is MapSymbol rs) || rs.Address != s.Address || rs.Name != s.Name) continue;
                dgvSymbols.ClearSelection();
                row.Selected = true;
                dgvSymbols.FirstDisplayedScrollingRowIndex = row.Index;
                break;
            }
        }

        // =========================================================================
        // 유틸리티
        // =========================================================================
        private MapSymbol GetSelectedSymbol()
        {
            if (dgvSymbols.SelectedRows.Count == 0) { SetStatus("변수를 선택하세요"); return null; }
            return dgvSymbols.SelectedRows[0].Tag as MapSymbol;
        }

        private byte GetNAD() => (byte)(nudNAD.Value);

        private void SetStatus(string msg)
        {
            lblStatus.Text      = msg;
            lblStatus.ForeColor = (msg.Contains("실패") || msg.Contains("없음") || msg.Contains("오류") || msg.StartsWith("✘"))
                ? Color.OrangeRed : Color.Cyan;
        }

        private void UpdateConnectionStatus()
        {
            bool ok = _gw?.IsConnected ?? false;
            lblConnStatus.Text      = ok ? "● 연결됨" : "○ 연결 안됨";
            lblConnStatus.ForeColor = ok ? Color.LimeGreen : Color.Gray;
        }

        private void btnClose_Click(object sender, EventArgs e) => Close();

        private void btnHexDec_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(txtWriteVal.Text)) return;
            string t = txtWriteVal.Text.Trim();
            if (t.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            {
                if (uint.TryParse(t.Substring(2), System.Globalization.NumberStyles.HexNumber,
                                  null, out uint v))
                    txtWriteVal.Text = v.ToString();
            }
            else
            {
                if (uint.TryParse(t, out uint v))
                    txtWriteVal.Text = $"0x{v:X}";
            }
        }
    }
}
