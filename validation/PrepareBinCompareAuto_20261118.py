"""Build a console adapter around the current PC automatic-verification code.
No GUI assembly, screen access, firmware editing, or production PC editing.
"""
from pathlib import Path
import hashlib
import json
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parent.parent
PC = Path(r'D:\jhhan\program\PC\PC_Lan8661_20261111')
TEMPLATE = Path(r'C:\Users\HKKIM\AppData\Local\Temp\CJ_FullBoard_20261111_qsckv8zq')
work = Path(tempfile.mkdtemp(prefix='CJ_BinCompare_20261118_'))
source_path = PC / 'Forms/FormUDS_Verification.cs'
source = source_path.read_text(encoding='utf-8-sig')
source = source.replace('using System.Windows.Forms;', 'using Application = CJConsoleHarness.ConsolePump;').replace('namespace LINMaster.Forms', 'namespace CJConsoleHarness')
source = source.replace('VerificationLevels = { 10, 50, 100 }', 'VerificationLevels = { 2, 5, 10 }')
source = source.replace('new byte[] { 10, 50 }', 'new byte[] { 10, 20, 30 }')
source = source.replace('128, 128, 128', '26, 26, 26').replace('128 * 257', '26 * 257').replace('128 * 32767', '26 * 32767')
source = source.replace('80, 255, 1, 1', '80, 26, 1, 1').replace('80, 255, 1, 10', '80, 26, 1, 10')
source = source.replace('"W", 50,', '"W", 10,').replace('"CIE1931", 100,', '"CIE1931", 10,').replace('step + 1), 100,', 'step + 1), 10,')
source = re.sub(r'(Math\.Abs\([^\n;]*?\) <= )1(?= &&|;)', r'\g<1>2', source).replace('±1ms', '±2ms')
needle = '            LINFrame frame = new LINFrame { FrameID = CJLinProtocol.ControlFrameId,'
assert source.count(needle) == 1
source = source.replace(needle, '            if (((control & 16) != 0) && (((control >> 6) == 0) ? (Math.Max(component1, Math.Max(component2, component3)) > 26) : (component3 > 26))) throw new InvalidOperationException("Brightness exceeds 10 percent test limit");\n' + needle)
needle = '            if (_verificationRows.Count % 12 == 0 || !pass)'
assert source.count(needle) == 1
source = source.replace(needle, '            if (test != "Flicker") StopOnFailure(_verificationRows[_verificationRows.Count - 1]);\n' + needle)
source = source.replace('row.HardwarePassed = hardwarePassed;', 'row.HardwarePassed = hardwarePassed;\n                        StopOnFailure(row);')
needle = 'Detail = detail + VerificationReadState(read) });\n        }'
assert source.count(needle) == 1
source = source.replace(needle, 'Detail = detail + VerificationReadState(read) });\n            StopOnFailure(_verificationDetailRows[_verificationDetailRows.Count - 1]);\n        }')
assert '128, 128, 128' not in source and '80, 255' not in source and 'System.Windows.Forms' not in source
(work / 'Verification.cs').write_text(source, encoding='utf-8')

harness = (TEMPLATE / 'SeparatedHarness.cs').read_text(encoding='utf-8-sig')
harness = harness.replace('private bool _verifying;', 'private bool _verifying;\n        private bool _ledModuleChecking = false;\n        private readonly ConsoleField btnLedModule = new ConsoleField();')
harness = harness.replace('rows = 114, detailRows = 696', 'rows = 126, detailRows = 696')
(work / 'Harness.cs').write_text(harness, encoding='utf-8')
report = (TEMPLATE / 'SeparatedReportData.cs').read_text(encoding='utf-8-sig').replace('rows.Count == 114', 'rows.Count == 126').replace('10/30Hz', '10/20/30Hz')
(work / 'Report.cs').write_text(report, encoding='utf-8')
interface = (PC / 'LIN/ILINGateway.cs').read_text(encoding='utf-8-sig').split('    public class FT4222Gateway')[0] + '}\n'
(work / 'ILINGateway.cs').write_text(interface, encoding='utf-8')
files = [PC / 'LIN' / name for name in ['LINFrame.cs', 'LINProtocol.cs', 'CJLinProtocol.cs', 'CJVerificationProtocol.cs', 'UDSProtocol.cs', 'PeakLINGateway.cs']]
exe = work / 'BinCompareAutomatic.exe'
command = [r'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\Roslyn\csc.exe', '/nologo', '/noconfig', '/target:exe', '/out:' + str(exe), '/r:System.dll', '/r:System.Core.dll', '/r:System.Drawing.dll', '/r:System.Web.Extensions.dll']
command += [str(p) for p in files] + [str(work / name) for name in ['ILINGateway.cs', 'Verification.cs', 'Harness.cs', 'Report.cs']]
compiled = subprocess.run(command, capture_output=True, text=True)
(work / 'build.txt').write_text(compiled.stdout + compiled.stderr, encoding='utf-8')
if compiled.returncode:
    raise RuntimeError(compiled.stdout + compiled.stderr)
manifest = dict(executable=str(exe), source=str(source_path), source_sha256=hashlib.sha256(source_path.read_bytes()).hexdigest(), source_files={str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in files}, adaptations=['Console fields/event polling instead of GUI; no Windows Forms/Excel COM', 'Brightness 2/5/10%, zone/fade/flicker maximum 10%', 'Flicker 10/20/30Hz; previously accepted tolerance 2ms', 'Stop on first failed verdict', 'JSON rows instead of Excel COM; report exported separately'], expected_rows=126, expected_detail_rows=696)
path = ROOT / 'validation/BinCompare_20261118_auto_manifest.json'
assert not path.exists(), 'Do not overwrite an existing campaign harness'
path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(manifest, ensure_ascii=False, indent=2))
