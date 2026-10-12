using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using LINMaster.LIN;
using LINMaster.CANoe;

namespace LINMaster.Forms
{
    // =========================================================================
    // [설정 상수]
    //   LED_COUNT_PER_PORT : 포트당 연결된 LED 개수 (SlaveFlag 비트 수)
    //                        변경 시 이 값만 수정하면 SF 체크박스 수가 자동 반영됨
    //                        현재 하드웨어: 12개
    // =========================================================================

    // =========================================================================
    // FormCIE – Mood Lamp CIE 색도도 TX 창
    //
    // [역할]
    //   - CIE 색도도(1931 xy / 1976 u'v') 를 렌더링하고 마우스 클릭으로 색상 선택
    //   - 이미지 파일을 불러와 픽셀 클릭으로 색상 선택
    //   - 선택된 색상을 LIN 프레임(DATA2=ColorX, DATA3=ColorY)에 반영
    //   - 단일 TX(즉시 전송) / 주기 TX(타이머 반복 전송) 지원
    //
    // [LIN 데이터 필드 구성]  ← 사양서 3.1.2 MLMMn_Comm1 기준
    //   DATA1 : SlaveFlag  [SF8..SF1] / DATA2: SF9~SF16 / DATA3: SF17~SF24 / DATA4: SF25 (비트마스크)
    //   DATA2 : LIN_ColorX (0x00~0xFA = 0.0~1.0)
    //   DATA3 : LIN_ColorY (0x00~0xFA = 0.0~1.0)
    //   DATA4 : Brightness (0x00~0xFA)
    //   DATA5 : FadeInTime  (단위: 10ms)
    //   DATA6 : FadeOutTime (단위: 10ms)
    //   DATA7 : NoFadeFlag  (bit7 = 1 이면 Fade 없음)
    //   DATA8 : SOH / LINErr 플래그
    //
    // [색상 소스]
    //   ① pnlCIE  : CIE 색도도 클릭 → CIE xy 좌표 직접 선택
    //   ② pnlImage: 이미지 픽셀 클릭 → sRGB → CIE xy 변환
    //   ③ nudColorX / nudColorY : 수치 직접 입력
    //
    // [Designer 컴포넌트]
    //   pnlCIE       : CIE 색도도 렌더링 패널
    //   pnlImage     : 이미지 피커 패널
    //   pnlColorPatch: 현재 선택 색상 미리보기 패널
    //   cmbMode      : CIE1931(xy) / CIE1976(u'v') 전환 콤보박스
    //   nudColorX/Y  : ColorX, ColorY 수치 직접 입력
    //   nudBrightness: Brightness 수치 입력
    //   nudFadeIn/Out: FadeIn / FadeOut 시간 입력
    //   nudInterval  : 주기 TX 인터벌(ms)
    //   cmbFID       : LIN Frame ID 선택 콤보박스
    //   cmbCS        : Checksum 타입 (Classic / Enhanced)
    //   chkSF1~SF25  : SlaveFlag 비트 체크박스
    //   chkNoFade    : NoFadeFlag 체크박스
    //   chkSOH       : SOH 플래그 체크박스
    //   chkLINErr    : LINErr 플래그 체크박스
    //   txtManualFID : 직접 입력 Frame ID (cmbFID = "직접입력" 선택 시 활성)
    //   txtPreview   : 최종 DATA 바이트 미리보기 텍스트박스
    //   lblCIEMode / lblSelXY / lblCursor / lblD1~D8 / lblImgColor / lblImgInfo : 상태 레이블
    //   btnTxOnce    : 즉시 TX 버튼
    //   btnTxCycle   : 주기 TX 시작/중지 버튼
    //   btnLoadImg   : 이미지 로드 버튼
    //   btnSFAll/None: SlaveFlag 전체 선택 / 해제 버튼
    // =========================================================================
    public partial class FormCIE : Form
    {
        // ──────────────────────────────────────────────────────────────────
        // [설정] 포트당 LED(SlaveFlag) 개수 – 하드웨어 구성에 맞게 수정
        // DATA1(byte1)에 SF1~SF8, DATA2(byte2)에 SF9~SF16, DATA3에 SF17~SF24, DATA4 bit0에 SF25를 담는다.
        // ──────────────────────────────────────────────────────────────────
        private const int FIO_LED_COUNT = 25;
        private const int ISE_LED_COUNT = 84;
        private const int MAX_TRACKED_LED_COUNT = 255;
        private static readonly bool OSP_VERBOSE_NODE_TX_LOG = false;
        private static readonly bool CIE_SHIFT_VERBOSE_LOG = false;

        public event Action<string> OspMessage;

        // ──────────────────────────────────────────────────────────────────
        // 멤버 변수
        // ──────────────────────────────────────────────────────────────────

        private ILINGateway _gw;  // LIN 통신 객체 (FormMain 에서 주입, 재연결 시 UpdateGateway로 갱신)
        private FT4222.FT4222Manager _ft4222;  // FT4222 LED 제어 (옵션)
        private readonly CANoeBridge _canoeBridge;
        private bool _ad3306CommandMode;
        private AD3306LedBackend _backendSelection = AD3306LedBackend.Auto;
        private bool _adiImmediateEffectRunning;
        private bool _canoeCycleActive = false;
        private int _pendingCANoeEffectMode = 0;
        private readonly Timer _canoeInitDelayTimer = new Timer();
        private Button _btnCANoeConnect;

        // [Bug Fix] FT4222 인스턴스가 존쬬하고 실제 연결된 경우에만 true.
        // _ft4222 != null 만으로 판단하면 Peak/IndieMicro/RS485 모드에서도
        // FT4222 패킷 포맷(D2=SF9~SF12 포함)으로 빌드되어 AD3301이 인식 못 하는 �b��제가 발생.
        private bool IsFT4222Active => _ft4222 != null && _ft4222.IsConnected;
        private bool IsCANoeActive => _canoeBridge != null && _canoeBridge.IsAttached && !IsFT4222Active && _gw == null;

        // CIE 색도도 상태
        private bool _is1976 = false;                         // false=CIE1931(xy), true=CIE1976(u'v')
        private PointF _selXY = new PointF(0.3127f, 0.3290f); // 현재 선택된 CIE xy 좌표 (D65 기본값)
        private bool _hasSel = false;                         // 색상이 선택된 상태인지 여부
        private bool _ospTxBusy = false;
        private Bitmap _cieBmp;                                  // CIE 색도도 캐시 비트맵
        private Size _cieCache;                                // 캐시된 비트맵의 크기 (크기 변경 감지용)
        private Rectangle _cr;                                      // CIE 렌더링 영역 (패널 내 좌표)

        // 이미지 피커 상태
        private Bitmap _imgBmp;                                     // 불러온 이미지 비트맵
        private PointF _imgSelPt = PointF.Empty;                    // 이미지 위 마지막 클릭 위치 (화면 좌표)

        // 주기 TX 타이머 → Designer(txTimer)로 이동, 여기서는 선언 불필요

        // ── CIE1931 스펙트럼 궤적 좌표 (380nm~700nm 파장별 xy) ─────────────
        // BuildCieBmp() 에서 색도도 외곽선을 그릴 때 사용
        private enum OspLedPath { Combined, FioOnly, IlasOnly }

        private int _rgbwShiftOffset = 0;
        private const int LED_SHIFT_BASE_BRIGHTNESS = 0;
        private const int LED_SHIFT_WAVE_RADIUS = 3;

        // ── LED_Shift / RGBW_Shift 공통 ──────────────────────────────────
        private int _ledShiftStep   = 1;
        private int _ledShiftLocked = 0;
        // 이전 Tick에서 ON 송신한 노드 목록 → 다음 Tick에 OFF 대상
        private readonly System.Collections.Generic.List<int> _ledShiftPrevOn
            = new System.Collections.Generic.List<int>();
        private readonly System.Collections.Generic.Dictionary<int, int> _ledShiftLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private string _ledShiftLastWaveKey = string.Empty;
        private readonly System.Collections.Generic.Dictionary<int, int> _ledShiftFioLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private readonly System.Collections.Generic.Dictionary<int, int> _ledShiftIlasLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private string _ledShiftFioLastWaveKey = string.Empty;
        private string _ledShiftIlasLastWaveKey = string.Empty;

        // ── LED_Shift_Up 상태 ─────────────────────────────────────────────
        // lo(아래)/hi(위) 방향을 각각 독립 추적. 각 끝 도달 시 locked 누적.
        private int _ledShiftUpStepLo   = 0;
        private int _ledShiftUpStepHi   = 0;
        private int _ledShiftUpLockedLo = 0;
        private int _ledShiftUpLockedHi = 0;
        private int _ledShiftUpFioStepLo = 0, _ledShiftUpFioStepHi = 0;
        private int _ledShiftUpIlasStepLo = 0, _ledShiftUpIlasStepHi = 0;
        private readonly System.Collections.Generic.List<int> _ledShiftUpPrevOn
            = new System.Collections.Generic.List<int>();
        private readonly System.Collections.Generic.Dictionary<int, int> _ledShiftUpLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private string _ledShiftUpLastWaveKey = string.Empty;
        private readonly System.Collections.Generic.Dictionary<int, int> _ledShiftUpFioLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private readonly System.Collections.Generic.Dictionary<int, int> _ledShiftUpIlasLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private string _ledShiftUpFioLastWaveKey = string.Empty;
        private string _ledShiftUpIlasLastWaveKey = string.Empty;

        // Shift Stack: LED25부터 LED1 방향으로 끝 LED를 한 개씩 누적한다.
        private int _shiftStackStep = 1;
        private int _shiftStackLocked = 0;
        private int _shiftStackFioStep = 1, _shiftStackFioLocked = 0;
        private int _shiftStackIlasStep = 1, _shiftStackIlasLocked = 0;
        private readonly System.Collections.Generic.Dictionary<int, int> _shiftStackLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private string _shiftStackLastWaveKey = string.Empty;
        private readonly System.Collections.Generic.Dictionary<int, int> _shiftStackFioLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private readonly System.Collections.Generic.Dictionary<int, int> _shiftStackIlasLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private string _shiftStackFioLastWaveKey = string.Empty;
        private string _shiftStackIlasLastWaveKey = string.Empty;

        // Shift Stack Up: 양쪽 끝에서 중앙 방향으로 각각 누적한다.
        private int _shiftStackUpStepLo = 0;
        private int _shiftStackUpStepHi = 0;
        private int _shiftStackUpLockedLo = 0;
        private int _shiftStackUpLockedHi = 0;
        private int _shiftStackUpFioStepLo = 0, _shiftStackUpFioStepHi = 0;
        private int _shiftStackUpFioLockedLo = 0, _shiftStackUpFioLockedHi = 0;
        private int _shiftStackUpIlasStepLo = 0, _shiftStackUpIlasStepHi = 0;
        private int _shiftStackUpIlasLockedLo = 0, _shiftStackUpIlasLockedHi = 0;
        private readonly System.Collections.Generic.Dictionary<int, int> _shiftStackUpLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private string _shiftStackUpLastWaveKey = string.Empty;
        private readonly System.Collections.Generic.Dictionary<int, int> _shiftStackUpFioLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private readonly System.Collections.Generic.Dictionary<int, int> _shiftStackUpIlasLastLevels
            = new System.Collections.Generic.Dictionary<int, int>();
        private string _shiftStackUpFioLastWaveKey = string.Empty;
        private string _shiftStackUpIlasLastWaveKey = string.Empty;

        // Move: 지정된 65개 CIE 좌표를 순서대로 전송하고 끝에서 처음으로 반복한다.
        /*
        private static readonly byte[,] MoveCieCoordinates =
        {
            { 0x2F, 0x33 }, { 0x2F, 0x12 }, { 0x33, 0x17 }, { 0x39, 0x1A },
            { 0x40, 0x1E }, { 0x48, 0x22 }, { 0x52, 0x27 }, { 0x5C, 0x2D },
            { 0x68, 0x34 }, { 0x79, 0x3D }, { 0x87, 0x45 }, { 0x94, 0x4C },
            { 0x9A, 0x4F }, { 0x9E, 0x52 }, { 0xA0, 0x51 }, { 0xA0, 0x52 },
            { 0xA0, 0x53 }, { 0xA0, 0x54 }, { 0x9F, 0x53 }, { 0x9E, 0x54 },
            { 0x9D, 0x55 }, { 0x9C, 0x56 }, { 0x9A, 0x58 }, { 0x98, 0x59 },
            { 0x96, 0x5B }, { 0x94, 0x5D }, { 0x91, 0x60 }, { 0x8D, 0x63 },
            { 0x89, 0x67 }, { 0x82, 0x6D }, { 0x7E, 0x71 }, { 0x79, 0x75 },
            { 0x6F, 0x7D }, { 0x69, 0x83 }, { 0x61, 0x8A }, { 0x5A, 0x90 },
            { 0x55, 0x95 }, { 0x51, 0x98 }, { 0x4C, 0x99 }, { 0x48, 0x98 },
            { 0x45, 0x94 }, { 0x41, 0x8D }, { 0x3D, 0x85 }, { 0x3A, 0x7B },
            { 0x36, 0x70 }, { 0x31, 0x65 }, { 0x2E, 0x5A }, { 0x2C, 0x52 },
            { 0x2B, 0x4A }, { 0x2A, 0x42 }, { 0x2A, 0x3B }, { 0x29, 0x33 },
            { 0x28, 0x2C }, { 0x28, 0x25 }, { 0x27, 0x1F }, { 0x26, 0x1A },
            { 0x26, 0x15 }, { 0x26, 0x12 }, { 0x26, 0x11 }, { 0x27, 0x11 },
            { 0x28, 0x11 }, { 0x29, 0x11 }, { 0x2A, 0x12 }, { 0x2C, 0x12 },
            { 0x2D, 0x13 }
        };
        */

        private static readonly byte[,] MoveCieCoordinates =
        {
            { 0x2F, 0x12 }, { 0x33, 0x17 }, { 0x39, 0x1A }, { 0x40, 0x1E },
            { 0x48, 0x22 }, { 0x52, 0x27 }, { 0x5C, 0x2D }, { 0x68, 0x34 },
            { 0x79, 0x3D }, { 0x87, 0x45 }, { 0x94, 0x4C }, { 0x9A, 0x4F },
            { 0x9E, 0x52 }, { 0xA0, 0x51 }, { 0xA0, 0x52 }, { 0xA0, 0x53 },
            { 0xA0, 0x54 }, { 0x9F, 0x53 }, { 0x9E, 0x54 }, { 0x9D, 0x55 },
            { 0x9C, 0x56 }, { 0x9A, 0x58 }, { 0x98, 0x59 }, { 0x96, 0x5B },
            { 0x94, 0x5D }, { 0x91, 0x60 }, { 0x8D, 0x63 }, { 0x89, 0x67 },
            { 0x82, 0x6D }, { 0x7E, 0x71 }, { 0x79, 0x75 }, { 0x6F, 0x7D },
            { 0x69, 0x83 }, { 0x61, 0x8A }, { 0x5A, 0x90 }, { 0x55, 0x95 },
            { 0x51, 0x98 }, { 0x4C, 0x99 }, { 0x48, 0x98 }, { 0x45, 0x94 },
            { 0x41, 0x8D }, { 0x3D, 0x85 }, { 0x3A, 0x7B }, { 0x36, 0x70 },
            { 0x31, 0x65 }, { 0x2E, 0x5A }, { 0x2C, 0x52 }, { 0x2B, 0x4A },
            { 0x2A, 0x42 }, { 0x2A, 0x3B }, { 0x29, 0x33 }, { 0x28, 0x2C },
            { 0x28, 0x25 }, { 0x27, 0x1F }, { 0x26, 0x1A }, { 0x26, 0x15 },
            { 0x26, 0x12 }, { 0x26, 0x11 }, { 0x27, 0x11 }, { 0x28, 0x11 },
            { 0x29, 0x11 }, { 0x2A, 0x12 }, { 0x2C, 0x12 }, { 0x2D, 0x13 },
        };

        private int _moveCoordinateIndex = 0;
        private bool _fadeIsMove = false;
        private bool _fadeMoveUseFidTx = false;

        // ── PC 통신 Fade 상태 ──────────────────────────────────────────────
        // KRTBI D2LM31.31 LED는 자체 Fade가 없으므로 PC에서 RGB 값을 보간해
        // FT4222/OSP로 여러 번 전송한다. 빨강 → 초록 변경 시 R은 감소하고 G는
        // 증가하는 식으로 현재 RGB에서 목표 RGB까지 직접 이동한다.
        private enum FadePhase { None, Running }
        private FadePhase _fadePhase = FadePhase.None;
        private int       _fadeTotalMs = 1;
        private const int FADE_TIME_UNIT_MS = 10;
        private const int FADE_STEP_MS = 10;  // Timer 요청 간격(ms). 실제 총 시간은 Stopwatch 기준으로 맞춘다.
        private readonly System.Diagnostics.Stopwatch _fadeStopwatch = new System.Diagnostics.Stopwatch();

        // 페이드 시작 시 캡처한 파라미터 (Tick 에서 사용)
        private byte   _fadeFid        = 0x00;
        private System.Collections.Generic.List<int> _fadeActiveNodes;
        private LedRgb[] _fadeStartRgb;
        private LedRgb[] _fadeTargetRgb;
        private readonly LedRgb[] _currentRgb = new LedRgb[MAX_TRACKED_LED_COUNT + 1];
        private readonly bool[] _currentRgbKnown = new bool[MAX_TRACKED_LED_COUNT + 1];

        private struct LedRgb
        {
            public byte R;
            public byte G;
            public byte B;

            public LedRgb(byte r, byte g, byte b)
            {
                R = r;
                G = g;
                B = b;
            }
        }

        private static readonly float[,] _spec =
        {
            {0.1741f,0.0050f},{0.1740f,0.0050f},{0.1738f,0.0049f},{0.1733f,0.0048f},
            {0.1726f,0.0051f},{0.1714f,0.0060f},{0.1689f,0.0116f},{0.1644f,0.0235f},
            {0.1566f,0.0579f},{0.1440f,0.1173f},{0.1241f,0.2080f},{0.0913f,0.3288f},
            {0.0454f,0.4950f},{0.0082f,0.5384f},{0.0139f,0.7502f},{0.0743f,0.8338f},
            {0.1547f,0.8521f},{0.2296f,0.7344f},{0.3016f,0.6923f},{0.3731f,0.6245f},
            {0.4441f,0.5547f},{0.5125f,0.4866f},{0.5752f,0.4242f},{0.6270f,0.3725f},
            {0.6658f,0.3340f},{0.6915f,0.3083f},{0.7079f,0.2920f},{0.7140f,0.2859f},
            {0.7100f,0.2900f},{0.6992f,0.3008f},{0.6916f,0.3083f},{0.6863f,0.3137f},
            {0.6830f,0.3170f},{0.6810f,0.3190f}
        };


        // =========================================================================
        // 생성자 / 초기화
        // =========================================================================

        public FormCIE()
            : this(null, null, null, false)
        {
        }

        public FormCIE(ILINGateway gw, FT4222.FT4222Manager ft4222 = null,
            CANoeBridge canoeBridge = null, bool adiLinMode = false)
        {
            _gw = gw;
            _ft4222 = ft4222;
            _canoeBridge = canoeBridge ?? new CANoeBridge();
            _ad3306CommandMode = adiLinMode;
            InitializeComponent();
            InitializeSunshineExtension();
            chkRGBWShift.CheckedChanged += ShiftMode_CheckedChanged;
            chkLedShift.CheckedChanged += ShiftMode_CheckedChanged;
            chkLedShiftUp.CheckedChanged += ShiftMode_CheckedChanged;
            chkShiftStack.CheckedChanged += ShiftMode_CheckedChanged;
            chkShiftStackUp.CheckedChanged += ShiftMode_CheckedChanged;
            chkMove.CheckedChanged += ShiftMode_CheckedChanged;
            chkTouch.CheckedChanged += ShiftMode_CheckedChanged;
            foreach (CheckBox ledBox in GetSlaveFlagBoxes())
                ledBox.Click += AD3306LedBox_Click;
            fadeTimer.Interval = FADE_STEP_MS;

            // ── 초기 UI 값 설정 ──────────────────────────────────────────
            cmbMode.SelectedIndex = 0;    // CIE1931 (xy)
            cmbCS.SelectedIndex = 1;    // Enhanced Checksum
            cmbFID.SelectedIndex = 0;    // Frame ID = 0x01 (기본)
            nudBrightness.Value = 10; // 테스트 기본 밝기: Data[4] = 10 (0x0A)
            nudFadeIn.Value = 0;    // FadeIn 없음
            nudFadeOut.Value = 0;    // FadeOut 없음
            chkNoFade.Checked = true; // Fade 비활성
            chkSOH.Checked = true; // SOH 플래그 ON
            chkLINErr.Checked = false;// LINErr 플래그 OFF
            txtManualFID.Enabled = false;// 직접입력 FID 비활성 (콤보 선택 시 활성)

            // FT4222 연결 상태에 맞춰 SF9~SF12 가시성 초기화 (IsFT4222Active 사용)
            // FT4222 연결 후 CIE 창 열기 → IsConnected=true 체크
            RefreshSlaveFlagVisibility();

            ApplySlaveFlagLimit(checkEnabledFlags: true);
            // txTimer Tick 이벤트는 Designer(FormCIE.Designer.cs)에서 연결됨

            // D65 기본 색상을 TX 필드와 패킷 미리보기에 함께 반영
            ApplyColorToTx();
        }

        private void InitializeCANoeControls()
        {
            btnTxCycle.Size = new Size(148, 36);

            _btnCANoeConnect = new Button
            {
                BackColor = Color.FromArgb(70, 90, 115),
                FlatStyle = FlatStyle.Flat,
                Font = new Font("Consolas", 8.5F, FontStyle.Bold),
                ForeColor = Color.White,
                Location = new Point(454, 13),
                Name = "btnCANoeConnect",
                Size = new Size(80, 36),
                TabIndex = 5,
                Text = "CANoe 연결",
                UseVisualStyleBackColor = false
            };
            _btnCANoeConnect.FlatAppearance.BorderSize = 0;
            _btnCANoeConnect.Click += btnCANoeConnect_Click;
            grpTX.Controls.Add(_btnCANoeConnect);
        }

        private void btnCANoeConnect_Click(object sender, EventArgs e)
        {
            string message;
            if (_canoeBridge.TrySetPanelVariable("AutoSend", 0, out message))
            {
                SetCANoeConnectButton(true);
                OspMessage?.Invoke("[CANoe] connected: " + message);
                return;
            }

            SetCANoeConnectButton(false);
            OspMessage?.Invoke("[CANoe] connect failed: " + message);
            MessageBox.Show("CANoe 연결 실패: " + message + "\r\n\r\nCANoe가 실행 중이고 AD3304LED25 시스템 변수가 로드되어 있는지 확인해 주세요.", "CANoe");
        }

        private void SetCANoeConnectButton(bool connected)
        {
            if (_btnCANoeConnect == null) return;
            _btnCANoeConnect.Text = connected ? "CANoe OK" : "CANoe 연결";
            _btnCANoeConnect.BackColor = connected ? Color.FromArgb(45, 135, 95) : Color.FromArgb(70, 90, 115);
        }
        private bool IsOspTxEnabled()
        {
            return chkOspTxEnable == null || chkOspTxEnable.Checked;
        }

        private bool IsIlasTxEnabled()
        {
            return chkIlasTxEnable == null || chkIlasTxEnable.Checked;
        }

        private bool IsRxFeedbackEnabled()
        {
            return chkRxEnable != null && chkRxEnable.Checked;
        }

        private bool IsAnyShiftModeChecked()
        {
            return (chkRGBWShift != null && chkRGBWShift.Checked) ||
                   (chkLedShift != null && chkLedShift.Checked) ||
                   (chkShiftStack != null && chkShiftStack.Checked) ||
                   (chkLedShiftUp != null && chkLedShiftUp.Checked) ||
                   (chkShiftStackUp != null && chkShiftStackUp.Checked) ||
                   (chkMove != null && chkMove.Checked) ||
                   (chkTouch != null && chkTouch.Checked);
        }

        private void UpdateTxRxFeedbackMode()
        {
            if (_ft4222 != null)
                _ft4222.TxRxFeedbackEnabled = IsRxFeedbackEnabled() && !IsAnyShiftModeChecked();
        }

        private CheckBox[] GetSlaveFlagBoxes()
        {
            return new CheckBox[] {
                chkSF1,  chkSF2,  chkSF3,  chkSF4,  chkSF5,
                chkSF6,  chkSF7,  chkSF8,  chkSF9,  chkSF10,
                chkSF11, chkSF12, chkSF13, chkSF14, chkSF15,
                chkSF16, chkSF17, chkSF18, chkSF19, chkSF20,
                chkSF21, chkSF22, chkSF23, chkSF24, chkSF25
            };
        }

        private void ApplySlaveFlagLimit(bool checkEnabledFlags)
        {
            CheckBox[] boxes = GetSlaveFlagBoxes();
            int enabledCount = Math.Min(FIO_LED_COUNT, boxes.Length);

            for (int i = 0; i < boxes.Length; i++)
            {
                bool enabled = i < enabledCount;
                boxes[i].Enabled = enabled;
                boxes[i].Checked = enabled && (checkEnabledFlags || boxes[i].Checked);
                boxes[i].ForeColor = enabled
                    ? Color.FromArgb(200, 220, 240)
                    : Color.FromArgb(95, 100, 115);
            }
        }


        private bool IsIlasTarget(byte fid)
        {
            return fid >= 0x01 && fid <= 0x08 &&
                   _ft4222 != null && _ft4222.IsConnected &&
                   _ft4222.IsIlasActiveFid(fid);
        }

        private bool IsFioTarget(byte fid)
        {
            return fid >= 0x01 && fid <= 0x08 &&
                   _ft4222 != null && _ft4222.IsConnected &&
                   _ft4222.IsFioActiveFid(fid);
        }

        private bool IsSplitFioIlasTarget(byte fid)
        {
            return fid >= 0x01 && fid <= 0x08 && IsFioTarget(fid) && IsIlasTarget(fid);
        }

        private int GetIlasLedCountForTarget(byte fid)
        {
            if (!IsIlasTarget(fid)) return 0;
            return Math.Min(MAX_TRACKED_LED_COUNT, ISE_LED_COUNT);
        }

        private System.Collections.Generic.List<int> GetIlasNodesForShiftTarget(byte fid)
        {
            int count = (fid == 0x00) ? GetBroadcastIlasLedCount() : GetIlasLedCountForTarget(fid);
            return count > 0
                ? BuildNodeRange(1, count)
                : new System.Collections.Generic.List<int>();
        }

        private System.Collections.Generic.List<int> GetFioShiftUpNodes()
        {
            return BuildNodeRange(1, FIO_LED_COUNT);
        }

        private int GetTargetLedCount(byte fid)
        {
            bool hasFio = IsOspTxEnabled() && _ft4222 != null && _ft4222.IsConnected && _ft4222.IsFioActiveFid(fid);
            bool hasIlas = IsIlasTxEnabled() && IsIlasTarget(fid);
            int ilasCount = 0;

            if (hasIlas)
            {
                ilasCount = Math.Min(MAX_TRACKED_LED_COUNT, ISE_LED_COUNT);
            }

            if (hasFio && hasIlas)
                return Math.Max(FIO_LED_COUNT, ilasCount > 0 ? ilasCount : FIO_LED_COUNT);
            if (hasIlas && ilasCount > 0)
                return ilasCount;
            return FIO_LED_COUNT;
        }

        private System.Collections.Generic.List<int> BuildNodeRange(int firstNode, int lastNode)
        {
            var nodes = new System.Collections.Generic.List<int>();
            for (int node = firstNode; node <= lastNode; node++)
                nodes.Add(node);
            return nodes;
        }

        private System.Collections.Generic.List<int> GetFioActiveNodesFromSlaveFlags()
        {
            CheckBox[] allSF = GetSlaveFlagBoxes();
            int sfCount = Math.Min(FIO_LED_COUNT, allSF.Length);
            var activeNodes = new System.Collections.Generic.List<int>();
            for (int i = 0; i < sfCount; i++)
                if (allSF[i].Checked)
                    activeNodes.Add(i + 1);
            return activeNodes;
        }

        private System.Collections.Generic.List<int> GetActiveNodesForTarget(byte fid)
        {
            if (fid == 0x00 && IsIlasTxEnabled() && HasAnyIlasActiveTarget())
                return BuildNodeRange(1, GetBroadcastIlasLedCount());
            if (IsIlasTxEnabled() && IsIlasTarget(fid))
                return BuildNodeRange(1, GetTargetLedCount(fid));

            return GetFioActiveNodesFromSlaveFlags();
        }

        private System.Collections.Generic.List<int> GetShiftUpNodesForTarget(byte fid)
        {
            if (fid == 0x00 && IsIlasTxEnabled() && HasAnyIlasActiveTarget())
                return BuildNodeRange(1, GetBroadcastIlasLedCount());
            if (IsIlasTxEnabled() && IsIlasTarget(fid))
                return BuildNodeRange(1, GetTargetLedCount(fid));

            return BuildNodeRange(1, FIO_LED_COUNT);
        }

        private bool HasAnyIlasActiveTarget()
        {
            if (_ft4222 == null || !_ft4222.IsConnected) return false;
            var fids = _ft4222.GetActiveFids();
            foreach (byte fid in fids)
                if (_ft4222.IsIlasActiveFid(fid))
                    return true;
            return false;
        }

        private int GetBroadcastIlasLedCount()
        {
            return Math.Min(MAX_TRACKED_LED_COUNT, ISE_LED_COUNT);
        }

        private System.Collections.Generic.List<byte> GetTargetFidsForOspSend(byte fid)
        {
            var result = new System.Collections.Generic.List<byte>();
            if (fid != 0x00)
            {
                result.Add(fid);
                return result;
            }

            if (_ft4222 != null && _ft4222.IsConnected)
            {
                var activeFids = _ft4222.GetActiveFids();
                foreach (byte activeFid in activeFids)
                    if (activeFid >= 0x01 && activeFid <= 0x08)
                        result.Add(activeFid);
            }

            return result;
        }
        // =========================================================================
        // CIE 색도도 렌더링
        // =========================================================================

        /// <summary>
        /// CIE 렌더링 영역(_cr)을 패널 기준 좌표로 계산
        /// 여백: 좌 44px, 상 10px, 우 10px, 하 26px
        /// </summary>
        private void CalcCr()
        {
            _cr = new Rectangle(44, 10,
                pnlCIE.ClientSize.Width - 54,
                pnlCIE.ClientSize.Height - 36);
        }

        /// <summary>CIE 좌표(cx,cy) → 패널 픽셀 좌표 변환</summary>
        private PointF CiePx(float cx, float cy)
            => new PointF(_cr.Left + cx * _cr.Width,
                          _cr.Bottom - cy * _cr.Height);

        /// <summary>패널 픽셀 좌표(px,py) → CIE 좌표(0~1) 변환</summary>
        private PointF PxCie(int px, int py)
            => new PointF(
                Math.Max(0f, Math.Min(1f, (px - _cr.Left) / (float)_cr.Width)),
                Math.Max(0f, Math.Min(1f, (_cr.Bottom - py) / (float)_cr.Height)));

        /// <summary>
        /// CIE1931 xy → CIE1976 u'v' 변환
        ///   u' = 4x / (-2x + 12y + 3)
        ///   v' = 9y / (-2x + 12y + 3)
        /// </summary>
        private static PointF XyToUv(float x, float y)
        {
            float d = -2f * x + 12f * y + 3f;
            if (Math.Abs(d) < 1e-6f) return new PointF(0.19f, 0.46f); // 분모가 0이면 D65 기본값
            return new PointF(4f * x / d, 9f * y / d);
        }

        private static PointF UvToXy(float u, float v)
        {
            float d = (6f * u) - (16f * v) + 12f;
            if (Math.Abs(d) < 1e-6f) return new PointF(0.3127f, 0.3290f);
            return new PointF(9f * u / d, 4f * v / d);
        }

        /// <summary>
        /// CIE xy 좌표를 sRGB Color 로 변환 (화면 미리보기용)
        ///   xy → XYZ (Y=1 정규화) → sRGB 선형 → 감마 보정(sRGB)
        /// </summary>
        private Color CieRgb(float x, float y)
        {
            if (y < 1e-4f) return Color.FromArgb(5, 5, 10); // y=0이면 거의 검정

            // CIE XYZ 계산 (Y=1로 정규화)
            float z = 1f - x - y;
            float X = x / y, Y = 1f, Z = z / y;

            // XYZ → sRGB 선형 (D65 기준 변환 행렬)
            float r = 3.2406f * X - 1.5372f * Y - 0.4986f * Z;
            float g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
            float b = 0.0557f * X - 0.2040f * Y + 1.0570f * Z;

            // sRGB 감마 보정 후 0~255 범위로 클리핑
            return Color.FromArgb(
                Clamp((int)(GammaCorrect(r) * 255)),
                Clamp((int)(GammaCorrect(g) * 255)),
                Clamp((int)(GammaCorrect(b) * 255)));
        }

        /// <summary>sRGB 감마 보정 (선형 → sRGB)</summary>
        private static float GammaCorrect(float c)
            => c <= 0f ? 0f
             : c <= 0.0031308f ? 12.92f * c
             : (float)(1.055 * Math.Pow(c, 1.0 / 2.4) - 0.055);

        /// <summary>int 값을 0~255 범위로 클리핑</summary>
        private static int Clamp(int v) => v < 0 ? 0 : v > 255 ? 255 : v;

        /// <summary>
        /// 스펙트럼 궤적(_spec) 좌표를 현재 모드(xy/u'v')에 따라
        /// 패널 픽셀 좌표 배열로 변환 (마지막 요소 = 첫 요소로 닫힌 폴리곤)
        /// </summary>
        private PointF[] SpecPx()
        {
            int n = _spec.GetLength(0);
            var pts = new PointF[n + 1];

            for (int i = 0; i < n; i++)
            {
                float sx = _spec[i, 0], sy = _spec[i, 1];
                if (_is1976) { var uv = XyToUv(sx, sy); sx = uv.X; sy = uv.Y; }
                pts[i] = CiePx(sx, sy);
            }
            pts[n] = pts[0]; // 폴리곤 닫기
            return pts;
        }

        /// <summary>
        /// CIE 색도도 비트맵을 새로 생성하여 반환 (캐시용)
        /// 스펙트럼 궤적 내부 영역만 CIE 색상으로 채우고
        /// 외부는 배경색(어두운 남색)으로 채움
        /// </summary>
        private Bitmap BuildCieBmp()
        {
            int W = _cr.Width, H = _cr.Height;
            if (W < 2 || H < 2) return new Bitmap(2, 2);

            var bmp = new Bitmap(W, H);

            // 스펙트럼 궤적 좌표를 비트맵 좌표계(0,0 = 좌상단)로 변환
            var sPts = new PointF[_spec.GetLength(0)];
            for (int i = 0; i < _spec.GetLength(0); i++)
            {
                float sx = _spec[i, 0], sy = _spec[i, 1];
                if (_is1976) { var uv = XyToUv(sx, sy); sx = uv.X; sy = uv.Y; }
                sPts[i] = new PointF(sx * W, (1f - sy) * H);
            }

            // 스펙트럼 궤적을 경계로 하는 GraphicsPath 생성
            var gp = new GraphicsPath();
            gp.AddPolygon(sPts);

            using (var g = Graphics.FromImage(bmp))
            {
                g.Clear(Color.FromArgb(10, 12, 20)); // 배경: 어두운 남색

                // 2px 간격으로 픽셀을 순회하며 스펙트럼 궤적 내부에만 색상 칠하기
                for (int py = 0; py < H; py += 2)
                {
                    for (int px = 0; px < W; px += 2)
                    {
                        // 스펙트럼 궤적 외부 픽셀은 건너뜀
                        if (!gp.IsVisible(px + 1, py + 1)) continue;

                        // 비트맵 좌표 → CIE 좌표 (정규화)
                        float cx = (float)px / W;
                        float cy = 1f - (float)py / H;
                        float rx = cx, ry = cy;

                        // CIE1976 모드이면 u'v' → xy 역변환
                        if (_is1976)
                        {
                            PointF xy = UvToXy(cx, cy);
                            rx = xy.X;
                            ry = xy.Y;
                        }

                        // CIE xy → sRGB 변환 후 픽셀 채우기 (반투명 적용)
                        var c = CieRgb(rx, ry);
                        using (var br = new SolidBrush(Color.FromArgb(210, c.R, c.G, c.B)))
                            g.FillRectangle(br, px, py, 2, 2);
                    }
                }
            }

            gp.Dispose();
            return bmp;
        }

        /// <summary>CIE 색도도 패널 Paint 이벤트 – 색도도, 궤적, 격자, 선택 마커를 순서대로 그림</summary>
        private void pnlCIE_Paint(object sender, PaintEventArgs e)
        {
            CalcCr();
            var g = e.Graphics;
            g.SmoothingMode = SmoothingMode.AntiAlias;
            g.Clear(Color.FromArgb(10, 12, 20));

            if (_cr.Width < 10 || _cr.Height < 10) return;

            // ── 1) CIE 색도도 비트맵 그리기 (크기 변경 시 재생성) ─────
            if (_cieBmp == null || _cieCache != new Size(_cr.Width, _cr.Height))
            {
                _cieBmp?.Dispose();
                _cieBmp = BuildCieBmp();
                _cieCache = new Size(_cr.Width, _cr.Height);
            }
            g.DrawImage(_cieBmp, _cr.Left, _cr.Top);

            // ── 2) 스펙트럼 궤적 외곽선 그리기 ──────────────────────────
            var sp = SpecPx();
            using (var p = new Pen(Color.FromArgb(220, 255, 255, 255), 1.2f))
                g.DrawLines(p, sp);                           // 파장 궤적 (흰색)
            using (var p = new Pen(Color.FromArgb(160, 200, 100, 255), 1f))
                g.DrawLine(p, sp[0], sp[sp.Length - 2]);      // 자홍색 닫힘선

            // ── 3) 격자 그리기 ────────────────────────────────────────
            using (var p = new Pen(Color.FromArgb(35, 180, 180, 200), 0.5f))
            {
                for (float v = 0.1f; v <= 1.0f; v += 0.1f)
                {
                    g.DrawLine(p,
                        _cr.Left + v * _cr.Width, _cr.Top,
                        _cr.Left + v * _cr.Width, _cr.Bottom); // 세로 격자선
                    g.DrawLine(p,
                        _cr.Left, _cr.Bottom - v * _cr.Height,
                        _cr.Right, _cr.Bottom - v * _cr.Height); // 가로 격자선
                }
            }

            // ── 4) 축 레이블 (눈금 값 + 축 이름) ─────────────────────
            using (var fn = new Font("Consolas", 7f))
            using (var br = new SolidBrush(Color.FromArgb(130, 150, 180)))
            using (var brB = new SolidBrush(Color.FromArgb(170, 190, 215)))
            {
                for (float v = 0.1f; v <= 1.01f; v += 0.1f)
                {
                    g.DrawString(v.ToString("F1"), fn, br,
                        _cr.Left + v * _cr.Width - 9, _cr.Bottom + 3);    // X축 눈금
                    g.DrawString(v.ToString("F1"), fn, br,
                        _cr.Left - 34, _cr.Bottom - v * _cr.Height - 6);  // Y축 눈금
                }

                // 축 이름: CIE1931 = x, y / CIE1976 = u', v'
                string xl = _is1976 ? "u'" : "x";
                string yl = _is1976 ? "v'" : "y";
                using (var fnB = new Font("Consolas", 9f, FontStyle.Bold))
                {
                    g.DrawString(xl, fnB, brB, _cr.Right - 4, _cr.Bottom + 5);
                    g.DrawString(yl, fnB, brB, _cr.Left - 38, _cr.Top - 2);
                }
            }

            // ── 5) 선택된 색상 마커 (십자선 + 좌표 레이블) ────────────
            if (_hasSel)
            {
                // 현재 모드에 따라 마커 좌표 계산
                float dpx = _selXY.X, dpy = _selXY.Y;
                if (_is1976) { var uv = XyToUv(dpx, dpy); dpx = uv.X; dpy = uv.Y; }
                var pp = CiePx(dpx, dpy);

                // 흰색 원 + 십자선 그리기
                using (var p = new Pen(Color.White, 1.8f))
                {
                    g.DrawEllipse(p, pp.X - 8, pp.Y - 8, 16, 16);
                    g.DrawLine(p, pp.X - 13, pp.Y, pp.X + 13, pp.Y);
                    g.DrawLine(p, pp.X, pp.Y - 13, pp.X, pp.Y + 13);
                }

                // 좌표값 레이블 (배경 반투명 사각형 위에 흰 글자)
                string lbl = string.Format("{0:F3}, {1:F3}", dpx, dpy);
                using (var fn = new Font("Consolas", 8.5f, FontStyle.Bold))
                using (var bg = new SolidBrush(Color.FromArgb(170, 0, 0, 0)))
                using (var fg = new SolidBrush(Color.White))
                {
                    var sz = g.MeasureString(lbl, fn);
                    float lx = Math.Min(pp.X + 14, _cr.Right - sz.Width - 2);
                    float ly = Math.Max(pp.Y - 22, _cr.Top + 2);
                    g.FillRectangle(bg, lx - 2, ly - 1, sz.Width + 4, sz.Height + 1);
                    g.DrawString(lbl, fn, fg, lx, ly);
                }
            }
        }

        /// <summary>패널 크기 변경 시 캐시 비트맵 파기 후 재그리기</summary>
        private void pnlCIE_Resize(object sender, EventArgs e)
        {
            _cieBmp?.Dispose();
            _cieBmp = null;
            pnlCIE.Invalidate();
        }

        /// <summary>
        /// CIE 색도도 마우스 클릭 이벤트
        /// 클릭 위치를 CIE xy 좌표로 변환하여 선택하고 TX 필드에 반영
        /// </summary>
        private void pnlCIE_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left) return;
            CalcCr();
            if (!_cr.Contains(e.Location)) return;

            var raw = PxCie(e.X, e.Y);

            if (_is1976)
            {
                // CIE1976 u'v' → CIE1931 xy 역변환
                PointF xy = UvToXy(raw.X, raw.Y);
                _selXY = new PointF(
                    Math.Max(0f, Math.Min(1f, xy.X)),
                    Math.Max(0f, Math.Min(1f, xy.Y)));
            }
            else
            {
                // CIE1931 xy 직접 사용
                _selXY = raw;
            }

            _hasSel = true;
            ApplyColorToTx();    // 선택한 좌표를 TX 필드(nudColorX/Y)에 반영
            pnlCIE.Invalidate(); // 마커 갱신
        }

        /// <summary>CIE 색도도 마우스 이동 이벤트 – 커서 좌표를 하단 레이블에 표시</summary>
        private void pnlCIE_MouseMove(object sender, MouseEventArgs e)
        {
            CalcCr();
            if (!_cr.Contains(e.Location)) { lblCursor.Text = "Cursor: -"; return; }

            var raw = PxCie(e.X, e.Y);
            lblCursor.Text = _is1976
                ? string.Format("Cursor  u'={0:F3}  v'={1:F3}", raw.X, raw.Y)
                : string.Format("Cursor   x={0:F3}   y={1:F3}", raw.X, raw.Y);
        }

        /// <summary>CIE 모드(1931/1976) 전환 – 캐시 비트맵 파기 후 재그리기</summary>
        private void cmbMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            _is1976 = (cmbMode.SelectedIndex == 1);
            _cieBmp?.Dispose();
            _cieBmp = null;
            UpdateColorInfo();
            pnlCIE.Invalidate();
        }


        // =========================================================================
        // 이미지 피커
        // =========================================================================

        /// <summary>이미지 파일 열기 버튼 클릭 – 파일 선택 후 pnlImage 에 표시</summary>
        private void btnLoadImg_Click(object sender, EventArgs e)
        {
            using (var dlg = new OpenFileDialog())
            {
                dlg.Title = "이미지 파일 선택";
                dlg.Filter = "이미지|*.png;*.jpg;*.jpeg;*.bmp;*.gif|모든 파일|*.*";
                if (dlg.ShowDialog() != DialogResult.OK) return;

                try
                {
                    _imgBmp?.Dispose();
                    _imgBmp = new Bitmap(dlg.FileName);
                    _imgSelPt = PointF.Empty;
                    pnlImage.Invalidate();
                    lblImgInfo.Text = string.Format("{0}  ({1}×{2})",
                        Path.GetFileName(dlg.FileName), _imgBmp.Width, _imgBmp.Height);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("이미지 로드 실패: " + ex.Message);
                }
            }
        }

        /// <summary>
        /// 이미지를 패널에 맞춰 비율 유지 확대/축소할 때의 표시 영역 계산
        /// 이미지 중앙 정렬
        /// </summary>
        private Rectangle CalcImgRect()
        {
            if (_imgBmp == null) return Rectangle.Empty;

            float sw = pnlImage.ClientSize.Width / (float)_imgBmp.Width;
            float sh = pnlImage.ClientSize.Height / (float)_imgBmp.Height;
            float s = Math.Min(sw, sh); // 가로/세로 중 더 작은 비율로 맞춤

            int w = (int)(_imgBmp.Width * s);
            int h = (int)(_imgBmp.Height * s);

            return new Rectangle(
                (pnlImage.ClientSize.Width - w) / 2,
                (pnlImage.ClientSize.Height - h) / 2,
                w, h);
        }

        /// <summary>이미지 피커 패널 Paint 이벤트 – 이미지와 선택 마커 그리기</summary>
        private void pnlImage_Paint(object sender, PaintEventArgs e)
        {
            var g = e.Graphics;
            g.Clear(Color.FromArgb(15, 15, 25));

            if (_imgBmp == null)
            {
                // 이미지 미로드 상태: 안내 문구 표시
                using (var f = new Font("Consolas", 9f))
                using (var b = new SolidBrush(Color.FromArgb(70, 100, 130)))
                    g.DrawString("이미지를 로드하면\n픽셀 클릭으로 색상 선택", f, b,
                        pnlImage.ClientSize.Width / 2f - 75f,
                        pnlImage.ClientSize.Height / 2f - 18f);
                return;
            }

            // 이미지 그리기 (고품질 보간)
            var rc = CalcImgRect();
            g.InterpolationMode = InterpolationMode.HighQualityBicubic;
            g.DrawImage(_imgBmp, rc);

            // 선택된 픽셀 위치에 십자선 마커 그리기
            if (_imgSelPt != PointF.Empty)
            {
                using (var p = new Pen(Color.Black, 3f))
                    g.DrawEllipse(p, _imgSelPt.X - 8, _imgSelPt.Y - 8, 16, 16); // 외곽 검정 테두리

                using (var p = new Pen(Color.White, 1.8f))
                {
                    g.DrawEllipse(p, _imgSelPt.X - 8, _imgSelPt.Y - 8, 16, 16);
                    g.DrawLine(p, _imgSelPt.X - 12, _imgSelPt.Y, _imgSelPt.X + 12, _imgSelPt.Y);
                    g.DrawLine(p, _imgSelPt.X, _imgSelPt.Y - 12, _imgSelPt.X, _imgSelPt.Y + 12);
                }
            }
        }

        /// <summary>
        /// 이미지 피커 마우스 클릭 이벤트
        /// 클릭한 픽셀의 sRGB 색상을 CIE xy 로 변환하여 TX 필드에 반영
        /// </summary>
        private void pnlImage_MouseClick(object sender, MouseEventArgs e)
        {
            if (_imgBmp == null) { MessageBox.Show("이미지를 먼저 로드하세요.", "알림"); return; }

            var rc = CalcImgRect();
            if (!rc.Contains(e.Location)) return;

            // 화면 좌표 → 이미지 픽셀 좌표 변환 (범위 클리핑)
            int px = ClampMax((int)((e.X - rc.Left) / (float)rc.Width * _imgBmp.Width), _imgBmp.Width - 1);
            int py = ClampMax((int)((e.Y - rc.Top) / (float)rc.Height * _imgBmp.Height), _imgBmp.Height - 1);

            var col = _imgBmp.GetPixel(px, py);

            _imgSelPt = new PointF(e.X, e.Y);
            pnlImage.Invalidate();

            // sRGB → CIE xy 변환 후 선택 좌표에 적용
            RgbToXY(col.R, col.G, col.B, out float cx, out float cy);
            _selXY = new PointF(cx, cy);
            _hasSel = true;

            ApplyColorToTx();    // TX 필드(nudColorX/Y) 갱신
            pnlCIE.Invalidate(); // CIE 마커 갱신

            lblImgColor.Text = string.Format(
                "클릭 R={0} G={1} B={2}  →  x={3:F3} y={4:F3}",
                col.R, col.G, col.B, cx, cy);
        }

        /// <summary>값을 [0, max] 범위로 클리핑</summary>
        private static int ClampMax(int v, int max) => v < 0 ? 0 : v > max ? max : v;

        /// <summary>이미지 피커 마우스 이동 이벤트 – 커서 위치의 픽셀 색상을 레이블에 표시</summary>
        private void pnlImage_MouseMove(object sender, MouseEventArgs e)
        {
            if (_imgBmp == null) { lblImgColor.Text = ""; return; }

            var rc = CalcImgRect();
            if (!rc.Contains(e.Location)) { lblImgColor.Text = ""; return; }

            int px = ClampMax((int)((e.X - rc.Left) / (float)rc.Width * _imgBmp.Width), _imgBmp.Width - 1);
            int py = ClampMax((int)((e.Y - rc.Top) / (float)rc.Height * _imgBmp.Height), _imgBmp.Height - 1);

            var col = _imgBmp.GetPixel(px, py);
            lblImgColor.Text = string.Format(
                "Cursor  R={0} G={1} B={2}  (px {3},{4})",
                col.R, col.G, col.B, px, py);
        }


        // =========================================================================
        // 색 공간 변환 유틸리티
        // =========================================================================

        /// <summary>
        /// sRGB (R, G, B: 0~255) → CIE xy 변환 (D65 기준)
        ///   sRGB → 선형 RGB (역 감마) → XYZ → xy 정규화
        /// </summary>
        private static void RgbToXY(int r, int g, int b, out float ox, out float oy)
        {
            float fr = InvGamma(r / 255f);
            float fg = InvGamma(g / 255f);
            float fb = InvGamma(b / 255f);

            // 선형 sRGB → CIE XYZ (D65 기준 변환 행렬)
            float X = 0.4124f * fr + 0.3576f * fg + 0.1805f * fb;
            float Y = 0.2126f * fr + 0.7152f * fg + 0.0722f * fb;
            float Z = 0.0193f * fr + 0.1192f * fg + 0.9505f * fb;

            float s = X + Y + Z;

            // 무채색이면 D65 백색점 반환
            if (s < 1e-6f) { ox = 0.3127f; oy = 0.3290f; return; }

            ox = Math.Max(0f, Math.Min(1f, X / s));
            oy = Math.Max(0f, Math.Min(1f, Y / s));
        }

        /// <summary>sRGB 역 감마 보정 (sRGB → 선형)</summary>
        private static float InvGamma(float c)
            => c <= 0.04045f ? c / 12.92f
             : (float)Math.Pow((c + 0.055) / 1.055, 2.4);


        // =========================================================================
        // 색상 → TX 필드 반영
        // =========================================================================

        /// <summary>
        /// 선택된 CIE xy 좌표(_selXY)를 TX 필드에 반영
        ///   nudColorX / nudColorY 값 업데이트
        ///   pnlColorPatch 배경색 업데이트 (미리보기)
        ///   패킷 프리뷰 갱신
        /// </summary>
        private void ApplyColorToTx()
        {
            PointF coordinate = _selXY;
            byte xb = XYtoByte(coordinate.X);
            byte yb = XYtoByte(coordinate.Y);

            nudColorX.Value = xb;
            nudColorY.Value = yb;

            // 미리보기 패치에 CIE xy → sRGB 변환 색상 표시
            pnlColorPatch.BackColor = CieRgb(_selXY.X, _selXY.Y);

            UpdateColorInfo();
            RefreshPacketPreview();
        }

        /// <summary>
        /// CIE xy 좌표 → LIN 데이터 바이트 변환
        /// 사양서 Table 17/18: 0x00~0xFA = 0.0~1.0 (선형 매핑)
        /// </summary>
        private static byte XYtoByte(float v)
            => (byte)Math.Max(0, Math.Min(0xFA, (int)Math.Round(v * 250.0)));

        /// <summary>LIN 데이터 바이트 → CIE xy 좌표 역변환</summary>
        private static float ByteToXY(byte b) => b / 250.0f;

        /// <summary>선택된 색상 좌표를 lblCIEMode / lblSelXY 레이블에 갱신</summary>
        private void UpdateColorInfo()
        {
            float dpx = _selXY.X, dpy = _selXY.Y;
            if (_is1976) { var uv = XyToUv(dpx, dpy); dpx = uv.X; dpy = uv.Y; }

            lblCIEMode.Text = _is1976 ? "CIE1976 (u'v')" : "CIE1931 (xy)";
            lblSelXY.Text = string.Format(
                "{0}  x={1:F4}  y={2:F4}  →  DATA2=0x{3:X2} DATA3=0x{4:X2}",
                _is1976 ? "CIE1976" : "CIE1931",
                _selXY.X, _selXY.Y,
                XYtoByte(_selXY.X), XYtoByte(_selXY.Y));
        }


        // =========================================================================
        // LIN 패킷 빌드 & 미리보기
        // =========================================================================

        /// <summary>
        /// 현재 UI 값으로 8바이트 LIN 데이터를 구성하고
        /// txtPreview + lblD1~D8 에 상세 내용을 출력
        /// </summary>
        private void RefreshPacketPreview()
        {
            var d = BuildData();
            txtPreview.Text = string.Format(
                "D1={0:X2} D2={1:X2} D3={2:X2} D4={3:X2} D5={4:X2} D6={5:X2} D7={6:X2} D8={7:X2}",
                d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);

            if (_ad3306CommandMode)
            {
                lblD1.Text = string.Format("D1 Target/FID = 0x{0:X2}", d[0]);
                lblD2.Text = chkTouch.Checked ? string.Format("D2 Touch Position = {0} ({1}%)", d[1], touchTrackBar.Value) : string.Format("D2 LED Number = {0} (0=all)", d[1]);
                lblD3.Text = string.Format("D3 CIE Cx = 0x{0:X2}", d[2]);
                lblD4.Text = string.Format("D4 CIE Cy = 0x{0:X2}", d[3]);
                lblD5.Text = string.Format("D5 Brightness = 0x{0:X2}", d[4]);
                lblD6.Text = string.Format("D6 Function = 0x{0:X2}", d[5]);
                lblD7.Text = string.Format("D7 Period = {0}ms", d[6]);
                lblD8.Text = string.Format("D8 Fade = 0x{0:X2}", d[7]);
                return;
            }

            if (IsFT4222Active)
            {
                // FT4222(OSP) 모드: SF9~SF12 포함 구조
                lblD1.Text = string.Format("D1 SlaveFlag_L = 0x{0:X2}  (SF1~SF8)",   d[0]);
                lblD2.Text = string.Format("D2 SlaveFlag_M = 0x{0:X2}  (SF9~SF16)",  d[1]);
                lblD3.Text = string.Format("D3 SlaveFlag_H = 0x{0:X2}  (SF17~SF24)", d[2]);
                lblD4.Text = string.Format("D4 SlaveFlag_X = 0x{0:X2}  (SF25)",      d[3]);
                lblD3.Text = string.Format("D3 LIN_ColorX = 0x{0:X2}  (x={1:F3})", d[2], ByteToXY(d[2]));
                lblD4.Text = string.Format("D4 LIN_ColorY = 0x{0:X2}  (y={1:F3})", d[3], ByteToXY(d[3]));
                lblD5.Text = string.Format("D5 Brightness = 0x{0:X2}  ({1:F0}%)", d[4], d[4] / 250.0 * 100);
                lblD6.Text = string.Format("D6 FadeInTime = 0x{0:X2}  ({1}ms)", d[5], d[5] * FADE_TIME_UNIT_MS);
                lblD7.Text = string.Format("D7 FadeOutTime= 0x{0:X2}  ({1}ms)", d[6], d[6] * FADE_TIME_UNIT_MS);
                lblD8.Text = string.Format("D8 NoFadeFlag = 0x{0:X2}  (bit7={1})", d[7], chkNoFade.Checked ? "1" : "0");
            }
            else
            {
                // IndieMicro/PEAK/RS485: LINMaster와 동일한 구조
                lblD1.Text = string.Format("D1 SlaveFlag  = 0x{0:X2}  (SF1~SF8,  bit0=SF1..bit7=SF8)", d[0]);
                lblD2.Text = string.Format("D2 LIN_ColorX = 0x{0:X2}  (x={1:F3})", d[1], ByteToXY(d[1]));
                lblD3.Text = string.Format("D3 LIN_ColorY = 0x{0:X2}  (y={1:F3})", d[2], ByteToXY(d[2]));
                lblD4.Text = string.Format("D4 Brightness = 0x{0:X2}  ({1:F0}%)", d[3], d[3] / 250.0 * 100);
                lblD5.Text = string.Format("D5 FadeInTime = 0x{0:X2}  ({1}ms)", d[4], d[4] * FADE_TIME_UNIT_MS);
                lblD6.Text = string.Format("D6 FadeOutTime= 0x{0:X2}  ({1}ms)", d[5], d[5] * FADE_TIME_UNIT_MS);
                lblD7.Text = string.Format("D7 NoFadeFlag = 0x{0:X2}  (bit7={1})", d[6], chkNoFade.Checked ? "1" : "0");
                lblD8.Text = string.Format("D8 SOH/LINErr = 0x{0:X2}  (SOH={1} LINErr={2})", d[7],
                    chkSOH.Checked ? "1" : "0", chkLINErr.Checked ? "1" : "0");
            }
        }

        /// <summary>
        /// 현재 UI 값으로 8바이트 LIN 데이터 배열 구성
        ///
        /// [DATA1 / DATA2 SlaveFlag 비트 배치]
        ///   DATA1 : SF1(bit0)  ~ SF8(bit7)   → LED 1~8  (체크된 LED만 변경)
        ///   DATA2 : SF9(bit0)  ~ SF12(bit3)  → LED 9~12 (체크된 LED만 변경)
        ///   ※ FIO_LED_COUNT 변경 시 해당 비트까지만 유효
        ///   ※ 체크되지 않은 SF 비트 = 0 → 해당 LED 변경 없음
        /// </summary>
        int Divide_TX = 0;
        private byte[] BuildData()
        {
            if (_ad3306CommandMode)
                return BuildAD3306Command().ToArray();

            bool isFT4222 = IsFT4222Active;

            if (Divide_TX == 0)
            {
                Divide_TX = 1;

                // ── SlaveFlag 비트 패킹 (SF1~SF25, 최대 25비트 = 4바이트) ─
                // D1=SF1~SF8, D2=SF9~SF16, D3=SF17~SF24, D4 bit0=SF25
                CheckBox[] allSFBoxes = GetSlaveFlagBoxes();
                byte sf1 = 0, sf2 = 0, sf3 = 0, sf4 = 0;
                for (int i = 0; i < Math.Min(8, allSFBoxes.Length); i++)
                    if (allSFBoxes[i].Checked) sf1 |= (byte)(1 << i);
                for (int i = 0; i < Math.Min(8, allSFBoxes.Length - 8); i++)
                    if (allSFBoxes[8 + i].Checked) sf2 |= (byte)(1 << i);
                for (int i = 0; i < Math.Min(8, allSFBoxes.Length - 16); i++)
                    if (allSFBoxes[16 + i].Checked) sf3 |= (byte)(1 << i);
                if (allSFBoxes.Length > 24 && allSFBoxes[24].Checked) sf4 |= 0x01;

                byte cx = (byte)nudColorX.Value;
                byte cy = (byte)nudColorY.Value;
                byte br = (byte)nudBrightness.Value;
                byte fin = (byte)nudFadeIn.Value;
                byte fout = (byte)nudFadeOut.Value;
                byte d7 = chkNoFade.Checked ? (byte)0x80 : (byte)0x00;
                byte d8 = (byte)((chkLINErr.Checked ? 0x80 : 0x00)
                                 | (chkSOH.Checked ? 0x01 : 0x00));

                if (isFT4222)
                {
                    // FT4222(OSP): SF1~SF25 포함, 8바이트
                    // [D1]=SF1~8  [D2]=SF9~16  [D3]=SF17~24 [D4]=SF25
                    // [D5]=ColorX [D6]=ColorY  [D7]=Bright  [D8]=NoFade
                    return new byte[] { sf1, sf2, sf3, sf4, cx, cy, br, d7 };
                }
                else
                {
                    // IndieMicro/PEAK/RS485: LINMaster와 동일한 구조
                    // [D1]=SF1~8  [D2]=ColorX  [D3]=ColorY  [D4]=Bright
                    // [D5]=FadeIn [D6]=FadeOut [D7]=NoFade  [D8]=SOH/LINErr
                    return new byte[] { sf1, cx, cy, br, fin, fout, d7, d8 };
                }
            }
            else
            {
                Divide_TX = 0;
                byte[] TX_Data = new byte[8];
                int TX_Data_Convert = 0;
                TX_Data[0] = 0x01;
                TX_Data[1] = 0x01; // Control Byte
                TX_Data_Convert = (int)nudColorX.Value * 256;
                TX_Data[2] = (byte)(TX_Data_Convert & 0xFF);
                TX_Data[3] = (byte)((TX_Data_Convert >> 8) & 0xFF);
                TX_Data_Convert = (int)nudColorY.Value * 256;
                TX_Data[4] = (byte)(TX_Data_Convert & 0xFF);
                TX_Data[5] = (byte)((TX_Data_Convert >> 8) & 0xFF);
                TX_Data[6] = (byte)nudBrightness.Value;
                TX_Data[7] = 0xFF;

                return new byte[] { TX_Data[0], TX_Data[1], TX_Data[2], TX_Data[3],
                                    TX_Data[4], TX_Data[5], TX_Data[6], TX_Data[7] };
            }
        }

        private AD3306LedCommand BuildAD3306Command()
        {
            AD3306LedTarget target = AD3306LedTarget.None;
            if (IsOspTxEnabled()) target |= AD3306LedTarget.Fio;
            if (IsIlasTxEnabled()) target |= AD3306LedTarget.Ilas;
            if (target == AD3306LedTarget.None) target = AD3306LedTarget.Both;

            int periodMs = Math.Max(1, Math.Min(255, (int)nudInterval.Value));
            int fadeInMs = (int)nudFadeIn.Value * FADE_TIME_UNIT_MS;
            int fadeOutMs = (int)nudFadeOut.Value * FADE_TIME_UNIT_MS;
            AD3306FadeMode fadeMode = AD3306FadeMode.None;
            if (!chkNoFade.Checked)
            {
                fadeMode = fadeInMs > 0 && fadeOutMs > 0 ? AD3306FadeMode.FadeInOut
                         : fadeOutMs > 0 ? AD3306FadeMode.FadeOut
                         : AD3306FadeMode.FadeIn;
            }

            return new AD3306LedCommand
            {
                TargetAndFid = AD3306LedCommand.PackTargetAndFid(target, GetFID(),
                    _backendSelection),
                LedNumber = chkTouch.Checked ? GetTouchPositionByte() : (byte)0,
                Cx = (byte)nudColorX.Value,
                Cy = (byte)nudColorY.Value,
                Brightness = (byte)nudBrightness.Value,
                Function = (byte)GetAdiFunction(),
                Period = (byte)periodMs,
                Fade = AD3306LedCommand.PackFade(fadeMode, Math.Max(fadeInMs, fadeOutMs))
            };
        }

        private void AD3306LedBox_Click(object sender, EventArgs e)
        {
            if (!_ad3306CommandMode) return;

            CheckBox selected = sender as CheckBox;
            if (selected == null) return;

            foreach (CheckBox ledBox in GetSlaveFlagBoxes())
                ledBox.Checked = ReferenceEquals(ledBox, selected);

            RefreshPacketPreview();
        }

        private byte GetAdiLedNumber()
        {
            CheckBox[] boxes = GetSlaveFlagBoxes();
            int selected = 0;
            int selectedNode = 0;
            for (int i = 0; i < boxes.Length; i++)
            {
                if (!boxes[i].Checked) continue;
                selected++;
                selectedNode = i + 1;
                if (selected > 1) return (byte)0U;
            }
            return selected == 1 ? (byte)selectedNode : (byte)0U;
        }

        private AD3306LedFunction GetAdiFunction()
        {
            if (chkRGBWShift.Checked) return AD3306LedFunction.RgbwShift;
            if (chkLedShift.Checked) return AD3306LedFunction.LedShift;
            if (chkLedShiftUp.Checked) return AD3306LedFunction.LedShiftUp;
            if (chkShiftStack.Checked) return AD3306LedFunction.ShiftStack;
            if (chkShiftStackUp.Checked) return AD3306LedFunction.ShiftStackUp;
            if (chkMove.Checked) return AD3306LedFunction.Move;
            if (chkTouch.Checked) return AD3306LedFunction.Touch;
            return nudBrightness.Value == 0 ? AD3306LedFunction.Off : AD3306LedFunction.Solid;
        }

        private byte GetTouchPositionByte()
        {
            return (byte)((touchTrackBar.Value * 255 + 50) / 100);
        }

        private void touchTrackBar_Scroll(object sender, EventArgs e)
        {
            lblTouchValue.Text = touchTrackBar.Value.ToString() + "%";
            RefreshPacketPreview();
            if (!chkTouch.Checked || !_ad3306CommandMode || !CheckGW(silent: true)) return;
            DoTxImmediate(advanceRgbwShift: false);
        }


        // =========================================================================
        // Fade In / Out
        // =========================================================================

        private static LedRgb CieToLedRgb(float cx, float cy, float brightness01)
        {
            if (brightness01 < 0f) brightness01 = 0f;
            if (brightness01 > 1f) brightness01 = 1f;
            if (cy < 1e-4f) cy = 1e-4f;

            float Y = 1f;
            float X = cx / cy * Y;
            float Z = (1f - cx - cy) / cy * Y;

            float r = 3.2406f * X - 1.5372f * Y - 0.4986f * Z;
            float g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
            float b = 0.0557f * X - 0.2040f * Y + 1.0570f * Z;

            if (r < 0f) r = 0f;
            if (g < 0f) g = 0f;
            if (b < 0f) b = 0f;

            float max = Math.Max(r, Math.Max(g, b));
            if (max > 1e-6f)
            {
                r /= max;
                g /= max;
                b /= max;
            }

            return new LedRgb(
                FloatToLedByte(r * brightness01),
                FloatToLedByte(g * brightness01),
                FloatToLedByte(b * brightness01));
        }

        private static byte FloatToLedByte(float v)
        {
            int i = (int)(v * 255f + 0.5f);
            return (byte)(i < 0 ? 0 : i > 255 ? 255 : i);
        }

        private void SendLedShiftWave(
            byte fid,
            System.Collections.Generic.List<int> activeNodes,
            System.Collections.Generic.List<int> focusNodes,
            System.Collections.Generic.List<int> lockedNodes,
            float cx,
            float cy,
            int peakBrightness,
            System.Collections.Generic.Dictionary<int, int> lastLevels,
            ref string lastWaveKey,
            System.Collections.Generic.List<int> waveOrder = null,
            bool circularWave = false,
            OspLedPath path = OspLedPath.Combined)
        {
            int peak = Math.Max(LED_SHIFT_BASE_BRIGHTNESS, Math.Min(250, peakBrightness));
            var order = waveOrder ?? activeNodes;
            string waveKey = string.Format("{0}|{1:F5}|{2:F5}|{3}|{4}|{5}",
                fid, cx, cy, peak, circularWave ? 1 : 0, string.Join(",", order));
            if (lastWaveKey != waveKey)
            {
                lastLevels.Clear();
                lastWaveKey = waveKey;
            }

            var txNodes = new System.Collections.Generic.List<ushort>();
            var txReds = new System.Collections.Generic.List<byte>();
            var txGreens = new System.Collections.Generic.List<byte>();
            var txBlues = new System.Collections.Generic.List<byte>();
            var txLevels = new System.Collections.Generic.List<int>();

            for (int i = 0; i < activeNodes.Count; i++)
            {
                int node = activeNodes[i];
                int level = LED_SHIFT_BASE_BRIGHTNESS;

                if (lockedNodes.Contains(node))
                {
                    level = peak;
                }
                else
                {
                    int minDist = int.MaxValue;
                    int nodeIdx = order.IndexOf(node);
                    foreach (int focusNode in focusNodes)
                    {
                        int focusIdx = order.IndexOf(focusNode);
                        if (nodeIdx >= 0 && focusIdx >= 0)
                        {
                            int dist = Math.Abs(nodeIdx - focusIdx);
                            if (circularWave && order.Count > 0)
                                dist = Math.Min(dist, order.Count - dist);
                            minDist = Math.Min(minDist, dist);
                        }
                    }

                    if (minDist <= LED_SHIFT_WAVE_RADIUS)
                    {
                        float t = 1f - (float)minDist / (LED_SHIFT_WAVE_RADIUS + 1);
                        float smooth = t * t * (3f - 2f * t);
                        level = LED_SHIFT_BASE_BRIGHTNESS +
                            (int)((peak - LED_SHIFT_BASE_BRIGHTNESS) * smooth + 0.5f);
                    }
                }

                int prevLevel;
                if (lastLevels.TryGetValue(node, out prevLevel) && prevLevel == level)
                    continue;

                LedRgb rgb = CieToLedRgb(cx, cy, level / 250.0f);
                txNodes.Add((ushort)node);
                txReds.Add(rgb.R);
                txGreens.Add(rgb.G);
                txBlues.Add(rgb.B);
                txLevels.Add(level);
            }

            if (txNodes.Count == 0) return;

            bool sent = SendColorRgbByNodesForPath(fid, txNodes.ToArray(), txReds.ToArray(), txGreens.ToArray(), txBlues.ToArray(), txNodes.Count, path);

            if (sent)
            {
                for (int i = 0; i < txNodes.Count; i++)
                    lastLevels[txNodes[i]] = txLevels[i];
            }
        }

        private bool SendColorRgbByNodesForPath(byte fid, ushort[] nodes, byte[] reds, byte[] greens, byte[] blues, int count, OspLedPath path)
        {
            UpdateTxRxFeedbackMode();

            if (path == OspLedPath.FioOnly && !IsOspTxEnabled())
                return true;
            if (path == OspLedPath.IlasOnly && !IsIlasTxEnabled())
                return true;

            if (path == OspLedPath.FioOnly)
            {
                if (OSP_VERBOSE_NODE_TX_LOG)
                    OspMessage?.Invoke(string.Format(
                        "[CIE OSP] FIO nodes TX FID=0x{0:X2} count={1} nodes=[{2}]",
                        fid, count, string.Join(",", nodes.Take(count))));
            }
            else if (path == OspLedPath.IlasOnly)
            {
                if (OSP_VERBOSE_NODE_TX_LOG)
                    OspMessage?.Invoke(string.Format(
                        "[CIE OSP] ILAS nodes TX FID=0x{0:X2} count={1} first={2} last={3}",
                        fid, count, count > 0 ? nodes[0].ToString() : "-",
                        count > 0 ? nodes[count - 1].ToString() : "-"));
            }
            if (IsCANoeActive)
                return SendCANoeColorRgbByNodes(fid, nodes, reds, greens, blues, count, path);

            if (path == OspLedPath.FioOnly)
                return _ft4222.SetFioColorRgbByNodes(fid, nodes, reds, greens, blues, count);
            if (path == OspLedPath.IlasOnly)
                return _ft4222.SetIlasColorRgbByNodes(fid, nodes, reds, greens, blues, count);

            if (IsOspTxEnabled() && !IsIlasTxEnabled())
                return _ft4222.SetFioColorRgbByNodes(fid, nodes, reds, greens, blues, count);

            bool sent = false;
            foreach (byte targetFid in GetTargetFidsForOspSend(fid))
            {
                if (!IsOspTxEnabled() && !IsIlasTxEnabled())
                    continue;
                if (IsOspTxEnabled() && IsIlasTxEnabled())
                {
                    if (_ft4222.SetColorRgbByNodes(targetFid, nodes, reds, greens, blues, count))
                        sent = true;
                    continue;
                }
                if (IsOspTxEnabled() && _ft4222.SetFioColorRgbByNodes(targetFid, nodes, reds, greens, blues, count))
                    sent = true;
                if (IsIlasTxEnabled() && _ft4222.SetIlasColorRgbByNodes(targetFid, nodes, reds, greens, blues, count))
                    sent = true;
            }
            return sent;
        }
        private string GetOspTransportLastError()
        {
            if (IsCANoeActive)
                return string.IsNullOrEmpty(_canoeBridge.LastError) ? "CANoe TX failed" : _canoeBridge.LastError;
            if (_ft4222 == null || string.IsNullOrEmpty(_ft4222.LastError))
                return "OSP TX failed";
            return _ft4222.LastError.Replace("\r\n", " / ");
        }
        private bool SendCANoeColorRgbByNodes(byte fid, ushort[] nodes, byte[] reds, byte[] greens, byte[] blues, int count, OspLedPath path)
        {
            if (!TryEnsureCANoe(silent: true))
                return false;
            if (nodes == null || reds == null || greens == null || blues == null || count <= 0)
                return false;

            int targetPath = GetCANoeTargetPath(path);
            if (targetPath < 0)
                return false;
            int maxNode = targetPath == 1 ? FIO_LED_COUNT : ISE_LED_COUNT;
            int validCount = Math.Min(count, Math.Min(nodes.Length, Math.Min(reds.Length, Math.Min(greens.Length, blues.Length))));
            int periodMs = Math.Max(1, (int)nudInterval.Value);
            int width = Math.Max(1, (int)nudShiftWidth.Value);
            string message = null;

            for (int i = 0; i < validCount; i++)
            {
                ushort node = nodes[i];
                if (node > maxNode)
                    continue;

                int firstNode = node == 0 ? 1 : node;
                int lastNode = node == 0 ? maxNode : node;
                if (!TrySendCANoeDirectColor(fid, targetPath, node, firstNode, lastNode, width, periodMs,
                        reds[i], greens[i], blues[i], 100, out message))
                {
                    OspMessage?.Invoke(string.Format(
                        "[CANoe] node={0} TX failed: {1}", node, message));
                    return false;
                }

                if (node > 0 && node <= FIO_LED_COUNT)
                    RememberNodeRgb(node, new LedRgb(reds[i], greens[i], blues[i]));
                System.Threading.Thread.Sleep(GetCANoeNodeTxDelayMs());
            }

            return true;
        }
        private bool SendSolidColorNodesForPath(byte fid, System.Collections.Generic.List<int> activeNodes, LedRgb rgb, OspLedPath path, string label)
        {
            if (activeNodes == null || activeNodes.Count == 0) return false;

            ushort[] nodes = new ushort[activeNodes.Count];
            byte[] reds = new byte[activeNodes.Count];
            byte[] greens = new byte[activeNodes.Count];
            byte[] blues = new byte[activeNodes.Count];

            for (int i = 0; i < activeNodes.Count; i++)
            {
                nodes[i] = (ushort)activeNodes[i];
                reds[i] = rgb.R;
                greens[i] = rgb.G;
                blues[i] = rgb.B;
            }

            bool ok = SendColorRgbByNodesForPath(fid, nodes, reds, greens, blues, activeNodes.Count, path);
            if (ok)
            {
                foreach (int nodeAddr in activeNodes)
                    RememberNodeRgb(nodeAddr, rgb);
                OspMessage?.Invoke(string.Format(
                    "[CIE OSP] FID=0x{0:X2} {1} batch TX complete ({2} nodes)",
                    fid, label, activeNodes.Count));
            }
            else
            {
                OspMessage?.Invoke(string.Format(
                    "[CIE OSP] FID=0x{0:X2} {1} batch TX failed: {2}",
                    fid, label, GetOspTransportLastError()));
            }
            return ok;
        }

        private bool SendBroadcastIlasSolidColor(LedRgb rgb, double cx, double cy, double brightness)
        {
            if (!IsIlasTxEnabled())
                return true;
            if (!HasAnyIlasActiveTarget())
                return true;

            int ilasCount = GetBroadcastIlasLedCount();
            var ilasNodes = ilasCount > 0 ? BuildNodeRange(1, ilasCount) : new System.Collections.Generic.List<int>();
            if (ilasNodes.Count == 0)
                return true;

            OspMessage?.Invoke(string.Format(
                "[CIE OSP] FID=0x00 Broadcast -> ILAS batch {0}개 노드  x={1:F4} y={2:F4} bri={3:F3}",
                ilasNodes.Count, cx, cy, brightness));

            return SendSolidColorNodesForPath(0x00, ilasNodes, rgb, OspLedPath.IlasOnly, "ILAS");
        }

        private bool AreAllFioNodesSelected(System.Collections.Generic.List<int> nodes)
        {
            if (nodes == null || nodes.Count != FIO_LED_COUNT)
                return false;

            for (int node = 1; node <= FIO_LED_COUNT; node++)
                if (!nodes.Contains(node))
                    return false;

            return true;
        }

        private bool SendFioSolidColorForSelection(byte fid, LedRgb rgb)
        {
            var fioNodes = GetFioActiveNodesFromSlaveFlags();
            if (fioNodes.Count == 0)
                return false;

            if (IsCANoeActive)
            {
                if (AreAllFioNodesSelected(fioNodes))
                {
                    ushort[] nodes = { 0 };
                    byte[] reds = { rgb.R };
                    byte[] greens = { rgb.G };
                    byte[] blues = { rgb.B };
                    bool ok = SendColorRgbByNodesForPath(fid, nodes, reds, greens, blues, 1, OspLedPath.FioOnly);
                    if (ok)
                    {
                        foreach (int nodeAddr in fioNodes)
                            RememberNodeRgb(nodeAddr, rgb);
                        OspMessage?.Invoke(string.Format(
                            "[CIE OSP] FID=0x{0:X2} CANoe FIO broadcast TX complete ({1} nodes)",
                            fid, fioNodes.Count));
                    }
                    else
                    {
                        OspMessage?.Invoke(string.Format(
                            "[CIE OSP] FID=0x{0:X2} CANoe FIO broadcast TX failed: {1}",
                            fid, GetOspTransportLastError()));
                    }
                    return ok;
                }

                return SendSolidColorNodesForPath(fid, fioNodes, rgb, OspLedPath.FioOnly, "FIO");
            }

            if (AreAllFioNodesSelected(fioNodes))
            {
                ushort[] nodes = { 0 };
                byte[] reds = { rgb.R };
                byte[] greens = { rgb.G };
                byte[] blues = { rgb.B };
                bool ok = _ft4222.SetFioColorRgbByNodes(fid, nodes, reds, greens, blues, 1);
                if (ok)
                {
                    foreach (int nodeAddr in fioNodes)
                        RememberNodeRgb(nodeAddr, rgb);
                    OspMessage?.Invoke(string.Format(
                        "[CIE OSP] FID=0x{0:X2} FIO broadcast TX complete ({1} nodes)",
                        fid, fioNodes.Count));
                }
                else
                {
                    OspMessage?.Invoke(string.Format(
                        "[CIE OSP] FID=0x{0:X2} FIO broadcast TX failed: {1}",
                        fid, GetOspTransportLastError()));
                }
                return ok;
            }

            return SendSolidColorNodesForPath(fid, fioNodes, rgb, OspLedPath.FioOnly, "FIO");
        }

        private bool SendColorRgbByFidForSelection(byte fid, LedRgb rgb)
        {
            bool sentAny = false;
            bool failedAny = false;

            if (fid == 0x00)
            {
                if (IsOspTxEnabled())
                {
                    bool okFio = SendFioSolidColorForSelection(0x00, rgb);
                    sentAny |= okFio;
                    failedAny |= !okFio;
                }

                if (IsIlasTxEnabled() && HasAnyIlasActiveTarget())
                {
                    int ilasCount = GetBroadcastIlasLedCount();
                    var ilasNodes = ilasCount > 0 ? BuildNodeRange(1, ilasCount) : new System.Collections.Generic.List<int>();
                    if (ilasNodes.Count > 0)
                    {
                        bool okIlas = SendSolidColorNodesForPath(0x00, ilasNodes, rgb, OspLedPath.IlasOnly, "ILAS");
                        sentAny |= okIlas;
                        failedAny |= !okIlas;
                    }
                }

                return sentAny || !failedAny;
            }

            foreach (byte targetFid in GetTargetFidsForOspSend(fid))
            {
                if (IsOspTxEnabled() && IsFioTarget(targetFid))
                {
                    bool okFio = SendFioSolidColorForSelection(targetFid, rgb);
                    sentAny |= okFio;
                    failedAny |= !okFio;
                }

                if (IsIlasTxEnabled() && IsIlasTarget(targetFid))
                {
                    int ilasCount = GetIlasLedCountForTarget(targetFid);
                    var ilasNodes = ilasCount > 0 ? BuildNodeRange(1, ilasCount) : new System.Collections.Generic.List<int>();
                    bool okIlas = SendSolidColorNodesForPath(targetFid, ilasNodes, rgb, OspLedPath.IlasOnly, "ILAS");
                    sentAny |= okIlas;
                    failedAny |= !okIlas;
                }
            }

            return sentAny || !failedAny;
        }

        private bool SendRgbwNodesForPath(byte fid, System.Collections.Generic.List<int> activeNodes, byte level, int shiftOffset, OspLedPath path, string label)
        {
            if (activeNodes == null || activeNodes.Count == 0) return false;

            ushort[] nodes = new ushort[activeNodes.Count];
            byte[] reds = new byte[activeNodes.Count];
            byte[] greens = new byte[activeNodes.Count];
            byte[] blues = new byte[activeNodes.Count];

            for (int i = 0; i < activeNodes.Count; i++)
            {
                int nodeAddr = activeNodes[i];
                byte r = 0, g = 0, b = 0;
                switch ((nodeAddr - 1 + shiftOffset) & 0x03)
                {
                    case 0: r = level; break;
                    case 1: g = level; break;
                    case 2: b = level; break;
                    default: r = g = b = level; break;
                }

                nodes[i] = (ushort)nodeAddr;
                reds[i] = r;
                greens[i] = g;
                blues[i] = b;
            }

            bool ok = SendColorRgbByNodesForPath(fid, nodes, reds, greens, blues, activeNodes.Count, path);
            if (ok)
            {
                for (int i = 0; i < activeNodes.Count; i++)
                    RememberNodeRgb(activeNodes[i], new LedRgb(reds[i], greens[i], blues[i]));
                OspMessage?.Invoke(string.Format(
                    "[CIE OSP] FID=0x{0:X2} {1} RGBW batch TX complete ({2} nodes)",
                    fid, label, activeNodes.Count));
            }
            else
            {
                OspMessage?.Invoke(string.Format(
                    "[CIE OSP] FID=0x{0:X2} {1} RGBW batch TX failed: {2}",
                    fid, label, GetOspTransportLastError()));
            }
            return ok;
        }

        private static LedRgb GetEachControlRgb(byte fid, int level)
        {
            byte value = (byte)Math.Max(0, Math.Min(255, level));
            switch ((fid - 1) & 0x03)
            {
                case 0: return new LedRgb(value, 0, 0);
                case 1: return new LedRgb(0, value, 0);
                case 2: return new LedRgb(0, 0, value);
                default: return new LedRgb(value, value, value);
            }
        }

        private void SendLedShiftWaveSelected(
            byte fid,
            System.Collections.Generic.List<int> activeNodes,
            System.Collections.Generic.List<int> focusNodes,
            System.Collections.Generic.List<int> lockedNodes,
            float cx,
            float cy,
            int peakBrightness,
            System.Collections.Generic.Dictionary<int, int> lastLevels,
            ref string lastWaveKey,
            System.Collections.Generic.List<int> waveOrder = null,
            bool circularWave = false,
            OspLedPath path = OspLedPath.Combined)
        {
            if (path != OspLedPath.Combined || fid != 0x00 || !chkEachControl.Checked)
            {
                SendLedShiftWave(fid, activeNodes, focusNodes, lockedNodes, cx, cy,
                    peakBrightness, lastLevels, ref lastWaveKey, waveOrder, circularWave, path);
                return;
            }

            if (IsCANoeActive)
            {
                SendLedShiftWave(fid, activeNodes, focusNodes, lockedNodes, cx, cy,
                    peakBrightness, lastLevels, ref lastWaveKey, waveOrder, circularWave, path);
                return;
            }

            var activeFids = _ft4222.GetActiveFids();
            if (activeFids.Count == 0)
            {
                OspMessage?.Invoke("[CIE Each Control] FIO ACTIVE FID 없음");
                return;
            }

            int peak = Math.Max(LED_SHIFT_BASE_BRIGHTNESS, Math.Min(250, peakBrightness));
            var order = waveOrder ?? activeNodes;
            var levels = new int[activeNodes.Count];

            for (int i = 0; i < activeNodes.Count; i++)
            {
                int node = activeNodes[i];
                int level = lockedNodes.Contains(node) ? peak : LED_SHIFT_BASE_BRIGHTNESS;
                if (!lockedNodes.Contains(node))
                {
                    int minDist = int.MaxValue;
                    int nodeIdx = order.IndexOf(node);
                    foreach (int focusNode in focusNodes)
                    {
                        int focusIdx = order.IndexOf(focusNode);
                        if (nodeIdx < 0 || focusIdx < 0) continue;
                        int dist = Math.Abs(nodeIdx - focusIdx);
                        if (circularWave && order.Count > 0)
                            dist = Math.Min(dist, order.Count - dist);
                        minDist = Math.Min(minDist, dist);
                    }

                    if (minDist <= LED_SHIFT_WAVE_RADIUS)
                    {
                        float t = 1f - (float)minDist / (LED_SHIFT_WAVE_RADIUS + 1);
                        float smooth = t * t * (3f - 2f * t);
                        level = LED_SHIFT_BASE_BRIGHTNESS +
                            (int)((peak - LED_SHIFT_BASE_BRIGHTNESS) * smooth + 0.5f);
                    }
                }
                levels[i] = level;
            }

            foreach (byte targetFid in activeFids)
            {
                var nodes = new ushort[activeNodes.Count];
                var reds = new byte[activeNodes.Count];
                var greens = new byte[activeNodes.Count];
                var blues = new byte[activeNodes.Count];
                for (int i = 0; i < activeNodes.Count; i++)
                {
                    LedRgb rgb = GetEachControlRgb(targetFid, levels[i]);
                    nodes[i] = (ushort)activeNodes[i];
                    reds[i] = rgb.R;
                    greens[i] = rgb.G;
                    blues[i] = rgb.B;
                }
                SendColorRgbByNodesForPath(targetFid, nodes, reds, greens, blues, activeNodes.Count, OspLedPath.Combined);
            }
        }

        private void SendLedShiftBase(
            byte fid,
            System.Collections.Generic.List<int> activeNodes,
            LedRgb baseRgb,
            System.Collections.Generic.Dictionary<int, int> lastLevels,
            OspLedPath path = OspLedPath.Combined)
        {
            var nodes = new ushort[activeNodes.Count];
            var reds = new byte[activeNodes.Count];
            var greens = new byte[activeNodes.Count];
            var blues = new byte[activeNodes.Count];

            for (int i = 0; i < activeNodes.Count; i++)
            {
                nodes[i] = (ushort)activeNodes[i];
                reds[i] = baseRgb.R;
                greens[i] = baseRgb.G;
                blues[i] = baseRgb.B;
            }

            bool sent = SendColorRgbByNodesForPath(fid, nodes, reds, greens, blues, activeNodes.Count, path);

            if (sent)
            {
                lastLevels.Clear();
                foreach (int n in activeNodes)
                    lastLevels[n] = LED_SHIFT_BASE_BRIGHTNESS;
            }
        }

        private static LedRgb LerpRgb(LedRgb start, LedRgb target, int step, int total)
        {
            if (total <= 0) total = 1;
            if (step > total) step = total;

            int r = start.R + ((target.R - start.R) * step) / total;
            int g = start.G + ((target.G - start.G) * step) / total;
            int b = start.B + ((target.B - start.B) * step) / total;
            return new LedRgb((byte)r, (byte)g, (byte)b);
        }


        private static LedRgb LerpRgbKeepPeak(LedRgb start, LedRgb target, int step, int total)
        {
            LedRgb rgb = LerpRgb(start, target, step, total);
            int startPeak = Math.Max(start.R, Math.Max(start.G, start.B));
            int targetPeak = Math.Max(target.R, Math.Max(target.G, target.B));
            int currentPeak = Math.Max(rgb.R, Math.Max(rgb.G, rgb.B));

            if (startPeak == 0 || targetPeak == 0 || currentPeak == 0)
                return rgb;

            int safeTotal = Math.Max(1, total);
            int safeStep = Math.Min(step, safeTotal);
            int desiredPeak = startPeak + ((targetPeak - startPeak) * safeStep) / safeTotal;
            int r = Math.Min(255, (rgb.R * desiredPeak + currentPeak / 2) / currentPeak);
            int g = Math.Min(255, (rgb.G * desiredPeak + currentPeak / 2) / currentPeak);
            int b = Math.Min(255, (rgb.B * desiredPeak + currentPeak / 2) / currentPeak);
            return new LedRgb((byte)r, (byte)g, (byte)b);
        }        private LedRgb GetTargetRgbForNode(int nodeAddr, LedRgb cieTarget, bool rgbwMode, int rgbwShift)
        {
            if (!rgbwMode) return cieTarget;

            byte level = (byte)Math.Max(0, Math.Min(255, (int)nudBrightness.Value));
            byte r = 0, g = 0, b = 0;
            switch ((nodeAddr - 1 + rgbwShift) % 4)
            {
                case 0: r = level; break;
                case 1: g = level; break;
                case 2: b = level; break;
                default: r = g = b = level; break;
            }
            return new LedRgb(r, g, b);
        }

        private LedRgb GetRgbwPhaseColor(int rgbwShift)
        {
            byte level = (byte)Math.Max(0, Math.Min(255, (int)nudBrightness.Value));
            switch (rgbwShift & 0x03)
            {
                case 0: return new LedRgb(level, 0, 0);
                case 1: return new LedRgb(0, level, 0);
                case 2: return new LedRgb(0, 0, level);
                default: return new LedRgb(level, level, level);
            }
        }

        private void RememberNodeRgb(int nodeAddr, LedRgb rgb)
        {
            if (nodeAddr < 1 || nodeAddr >= _currentRgb.Length) return;
            _currentRgb[nodeAddr] = rgb;
            _currentRgbKnown[nodeAddr] = true;
        }

        private LedRgb GetCurrentNodeRgb(int nodeAddr)
        {
            if (nodeAddr >= 1 && nodeAddr < _currentRgb.Length && _currentRgbKnown[nodeAddr])
                return _currentRgb[nodeAddr];
            return new LedRgb(0, 0, 0);
        }

        /// <summary>
        /// Fade 시퀀스를 시작한다.
        ///   - chkNoFade 체크 시 → 즉시 DoTxImmediate() 호출
        ///   - chkNoFade 해제 시 → 현재 RGB에서 목표 RGB까지 PC가 단계 전송
        /// DoTx() 는 이 함수를 통해서만 OSP 전송을 시작한다.
        /// </summary>
        private void StartFadeOrImmediate(bool advanceRgbwShift)
        {
            if (_ad3306CommandMode)
            {
                DoTxImmediate(advanceRgbwShift);
                return;
            }

            if (chkNoFade.Checked || (!IsFT4222Active && !IsCANoeActive))
            {
                DoTxImmediate(advanceRgbwShift);
                return;
            }

            byte id = GetFID();
            if (id > 0x08)
            {
                DoTxImmediate(advanceRgbwShift);
                return;
            }

            // 개별 FID의 RGBW는 노드마다 목표색이 다르므로 batch 경로를 사용한다.
            // FID 0x00 RGBW Broadcast는 모든 LED가 같은 phase라 기존 Fade를 적용할 수 있다.
            if (chkRGBWShift.Checked && id != 0x00)
            {
                DoTxImmediate(advanceRgbwShift);
                return;
            }
            if (IsCANoeActive && !chkRGBWShift.Checked && !chkMove.Checked && !IsAnyShiftModeChecked())
            {
                TryStartCANoePanelEffect(15);
                return;
            }

            // ── 파라미터 캡처 ──
            _fadeFid       = id;
            float cx       = ByteToXY((byte)nudColorX.Value);
            float cy       = ByteToXY((byte)nudColorY.Value);
            float bri      = (float)nudBrightness.Value / 250.0f;
            bool rgbwMode  = chkRGBWShift.Checked;
            int rgbwShift  = rgbwMode && chkRGBWShift.Checked ? _rgbwShiftOffset : 0;
            LedRgb cieTarget = CieToLedRgb(cx, cy, bri);

            _fadeActiveNodes = GetActiveNodesForTarget(id);

            if (_fadeActiveNodes.Count == 0)
            {
                OspMessage?.Invoke("[CIE Fade] SlaveFlag 활성 없음 → 전송 건너뜀");
                return;
            }

            // ── 진행 중인 Fade 중단 후 재시작 ──
            fadeTimer.Stop();

            int fadeInMs  = (int)nudFadeIn.Value  * FADE_TIME_UNIT_MS;
            int fadeOutMs = (int)nudFadeOut.Value * FADE_TIME_UNIT_MS;  // FadeIn이 0이면 대체 시간으로 사용
            int fadeMs = fadeInMs > 0 ? fadeInMs : fadeOutMs;

            if (fadeMs <= 0)
            {
                // FadeIn/Out 시간이 모두 0 이면 즉시 전송
                DoTxImmediate(advanceRgbwShift);
                return;
            }

            _fadeStartRgb = new LedRgb[MAX_TRACKED_LED_COUNT + 1];
            _fadeTargetRgb = new LedRgb[MAX_TRACKED_LED_COUNT + 1];
            LedRgb broadcastRgbwTarget = GetRgbwPhaseColor(rgbwShift);
            foreach (int nodeAddr in _fadeActiveNodes)
            {
                _fadeStartRgb[nodeAddr] = GetCurrentNodeRgb(nodeAddr);
                _fadeTargetRgb[nodeAddr] = (rgbwMode && id == 0x00)
                    ? broadcastRgbwTarget
                    : GetTargetRgbForNode(nodeAddr, cieTarget, rgbwMode, rgbwShift);
            }

            _fadePhase = FadePhase.Running;
            _fadeIsMove = false;
            _fadeTotalMs = Math.Max(1, fadeMs);
            _fadeStopwatch.Restart();

            if (advanceRgbwShift && chkRGBWShift.Checked)
                _rgbwShiftOffset = (_rgbwShiftOffset + (int)nudShiftWidth.Value) & 0x03;

            OspMessage?.Invoke(string.Format(
                "[CIE Fade] RGB Fade 시작 ({0}ms, tick={1}ms, {2} nodes)",
                _fadeTotalMs, FADE_STEP_MS, _fadeActiveNodes.Count));
            fadeTimer.Start();
        }

        /// <summary>
        /// fadeTimer Tick — 실제 경과시간 기준으로 보간값을 전송한다.
        /// </summary>
        private void FadeTimer_Tick(object sender, EventArgs e)
        {
            if (_fadePhase == FadePhase.None || _fadeActiveNodes == null ||
                _fadeStartRgb == null || _fadeTargetRgb == null)
            {
                fadeTimer.Stop();
                _fadeIsMove = false;
                return;
            }

            int elapsedMs = (int)Math.Min(_fadeStopwatch.ElapsedMilliseconds, _fadeTotalMs);

            SendFadeStep(elapsedMs, _fadeTotalMs);

            if (elapsedMs >= _fadeTotalMs)
            {
                _fadeStopwatch.Stop();
                fadeTimer.Stop();
                _fadePhase = FadePhase.None;
                _fadeIsMove = false;
                OspMessage?.Invoke("[CIE Fade] RGB Fade 완료");
            }
        }

        /// <summary>
        /// Fade Tick 에서 현재 step의 RGB 보간값으로 OSP SET_PWM 전송
        /// </summary>
        private void SendFadeStep(int step, int total)
        {
            if (!IsFT4222Active && !IsCANoeActive) return;
            if (_fadeActiveNodes == null || _fadeActiveNodes.Count == 0) return;

            int baseNode = _fadeActiveNodes[0];
            LedRgb rgb = LerpRgb(_fadeStartRgb[baseNode], _fadeTargetRgb[baseNode], step, total);

            if (_fadeIsMove)
            {
                if (_fadeMoveUseFidTx)
                {
                    bool sent = SendColorRgbByFidForSelection(_fadeFid, rgb);
                    if (sent)
                    {
                        foreach (int nodeAddr in _fadeActiveNodes)
                            RememberNodeRgb(nodeAddr, rgb);
                    }
                    return;
                }

                var nodes = new ushort[_fadeActiveNodes.Count];
                var reds = new byte[_fadeActiveNodes.Count];
                var greens = new byte[_fadeActiveNodes.Count];
                var blues = new byte[_fadeActiveNodes.Count];
                for (int i = 0; i < _fadeActiveNodes.Count; i++)
                {
                    nodes[i] = (ushort)_fadeActiveNodes[i];
                    reds[i] = rgb.R;
                    greens[i] = rgb.G;
                    blues[i] = rgb.B;
                }

                bool batchSent = false;
                foreach (byte targetFid in GetTargetFidsForOspSend(_fadeFid))
                {
                    if (SendColorRgbByNodesForPath(
                        targetFid, nodes, reds, greens, blues, _fadeActiveNodes.Count, OspLedPath.Combined))
                        batchSent = true;
                }
                if (batchSent)
                {
                    foreach (int nodeAddr in _fadeActiveNodes)
                        RememberNodeRgb(nodeAddr, rgb);
                }
                return;
            }

            // 일반/RGBW Fade는 FID 채널 전체에 한 번 전송한다.
            // FID 0x00: multicast 1회 / 그 외: 해당 FID 1회
            bool fadeSent = SendColorRgbByFidForSelection(_fadeFid, rgb);
            if (fadeSent)
            {
                foreach (int nodeAddr in _fadeActiveNodes)
                    RememberNodeRgb(nodeAddr, rgb);
            }
        }

        // =========================================================================
        // LIN TX
        // =========================================================================

        /// <summary>즉시 TX 버튼 클릭 – LIN 프레임 1회 전송</summary>
        private void btnTxOnce_Click(object sender, EventArgs e)
        {
            if (!CheckGW()) return;
            if (_ad3306CommandMode)
            {
                if (_adiImmediateEffectRunning)
                {
                    CancelSunshineDeferredImmediateTx();
                    SendAdiLinStop();
                    SetAdiImmediateEffectState(false);
                    return;
                }

                DoTxImmediate(advanceRgbwShift: true);
                SetAdiImmediateEffectState(IsAdiAutonomousEffectSelected());
                return;
            }
            if (chkLedShift.Checked)   { DoTxLedShift();   return; }
            if (chkShiftStack.Checked) { DoTxShiftStack(); return; }
            if (chkLedShiftUp.Checked) { DoTxLedShiftUp(); return; }
            if (chkShiftStackUp.Checked) { DoTxShiftStackUp(); return; }
            if (chkMove.Checked) { DoTxMove(); return; }
            DoTx(advanceRgbwShift: true);
        }

        /// <summary>
        /// 주기 TX 버튼 클릭 – 타이머 토글
        /// - 실행 중이면 중지 (버튼 → 파란색)
        /// - 중지 상태이면 시작 (버튼 → 빨간색)
        /// </summary>
        private void btnTxCycle_Click(object sender, EventArgs e)
        {
            if (_ad3306CommandMode)
            {
                if (txTimer.Enabled)
                {
                    txTimer.Stop();
                    SetAdiLinRunningState(false);
                }
                else
                {
                    if (!CheckGW()) return;
                    // A periodic transmission owns the selected command from this
                    // point.  Do not leave the one-shot Stop toggle armed because
                    // the next timer tick would immediately start the effect again.
                    SetAdiImmediateEffectState(false);
                    txTimer.Interval = Math.Max(1, (int)nudInterval.Value);
                    txTimer.Start();
                    SetAdiLinRunningState(true);
                }
                return;
            }

            if (_canoeCycleActive)
            {
                StopCANoeCycleIfRunning();
                return;
            }

            if (txTimer.Enabled)
            {
                txTimer.Stop();
                fadeTimer.Stop();
                _fadeStopwatch.Stop();
                _fadePhase = FadePhase.None;
                _fadeIsMove = false;
                btnTxCycle.Text = "▶ 주기 TX 시작";
                btnTxCycle.BackColor = Color.FromArgb(50, 110, 190);
                return;
            }

            if (!CheckGW()) return;
if (chkRGBWShift.Checked)
                _rgbwShiftOffset = 0;

            if (chkLedShift.Checked)
            {
                _ledShiftStep   = 1;
                _ledShiftLocked = 0;
                _ledShiftPrevOn.Clear();
                _ledShiftLastLevels.Clear();
                _ledShiftLastWaveKey = string.Empty;
                _ledShiftFioLastLevels.Clear();
                _ledShiftFioLastWaveKey = string.Empty;
                _ledShiftIlasLastLevels.Clear();
                _ledShiftIlasLastWaveKey = string.Empty;
            }

            if (chkLedShiftUp.Checked)
            {
                _ledShiftUpStepLo   = 0;
                _ledShiftUpStepHi   = 0;
                _ledShiftUpLockedLo = 0;
                _ledShiftUpLockedHi = 0;
                _ledShiftUpFioStepLo = 0;
                _ledShiftUpFioStepHi = 0;
                _ledShiftUpIlasStepLo = 0;
                _ledShiftUpIlasStepHi = 0;
                _ledShiftUpPrevOn.Clear();
                _ledShiftUpLastLevels.Clear();
                _ledShiftUpLastWaveKey = string.Empty;
                _ledShiftUpFioLastLevels.Clear();
                _ledShiftUpIlasLastLevels.Clear();
                _ledShiftUpFioLastWaveKey = string.Empty;
                _ledShiftUpIlasLastWaveKey = string.Empty;
            }

            if (chkShiftStack.Checked)
            {
                _shiftStackStep = 1;
                _shiftStackLocked = 0;
                _shiftStackFioStep = 1;
                _shiftStackFioLocked = 0;
                _shiftStackIlasStep = 1;
                _shiftStackIlasLocked = 0;
                _shiftStackLastLevels.Clear();
                _shiftStackLastWaveKey = string.Empty;
                _shiftStackFioLastLevels.Clear();
                _shiftStackIlasLastLevels.Clear();
                _shiftStackFioLastWaveKey = string.Empty;
                _shiftStackIlasLastWaveKey = string.Empty;
            }

            if (chkShiftStackUp.Checked)
            {
                _shiftStackUpStepLo = 0;
                _shiftStackUpStepHi = 0;
                _shiftStackUpLockedLo = 0;
                _shiftStackUpLockedHi = 0;
                _shiftStackUpFioStepLo = 0;
                _shiftStackUpFioStepHi = 0;
                _shiftStackUpFioLockedLo = 0;
                _shiftStackUpFioLockedHi = 0;
                _shiftStackUpIlasStepLo = 0;
                _shiftStackUpIlasStepHi = 0;
                _shiftStackUpIlasLockedLo = 0;
                _shiftStackUpIlasLockedHi = 0;
                _shiftStackUpLastLevels.Clear();
                _shiftStackUpLastWaveKey = string.Empty;
                _shiftStackUpFioLastLevels.Clear();
                _shiftStackUpIlasLastLevels.Clear();
                _shiftStackUpFioLastWaveKey = string.Empty;
                _shiftStackUpIlasLastWaveKey = string.Empty;
            }

            if (chkMove.Checked)
            {
                _moveCoordinateIndex = 0;
                _fadeIsMove = false;
            }

            int canoeEffectMode = GetSelectedCANoeEffectMode();
            if (IsCANoeActive)
            {
                if (canoeEffectMode > 0)
                {
                    if (TryStartCANoePanelEffect(canoeEffectMode))
                    {
                        _canoeCycleActive = true;
                        btnTxCycle.Text = "■ CANoe 주기 TX 중지";
                        btnTxCycle.BackColor = Color.FromArgb(160, 50, 50);
                    }
                }
                else
                {
                    TrySendCANoeCurrentColor();
                    _canoeCycleActive = false;
                    btnTxCycle.Text = "▶ 주기 TX 시작";
                    btnTxCycle.BackColor = Color.FromArgb(50, 110, 190);
                }
                return;
            }

            txTimer.Interval = Math.Max(1, (int)nudInterval.Value);
            txTimer.Start();
            btnTxCycle.Text = "■ 주기 TX 중지";
            btnTxCycle.BackColor = Color.FromArgb(160, 50, 50);
        }

        /// <summary>주기 TX 타이머 Tick – 연결 상태 확인 후 DoTx() 호출</summary>
        private void TxTimer_Tick(object sender, EventArgs e)
        {
            if (_ospTxBusy) return;
            if (!CheckGW(silent: true))
            {
                txTimer.Stop();
                if (_ad3306CommandMode) SetAdiLinRunningState(false);
                return;
            }
            if (_fadePhase != FadePhase.None) return;

            _ospTxBusy = true;
            try
            {
                // AD3306/Microchip command mode transmits exactly one common
                // 8-byte LED command per timer period.  The Sunshine firmware
                // owns the actual OSP/ISE effect progression.
                if (_ad3306CommandMode)
                {
                    DoTxImmediate(advanceRgbwShift: true);
                    return;
                }

                if (chkLedShift.Checked)
                {
                    DoTxLedShift();
                    return;
                }

                if (chkShiftStack.Checked)
                {
                    DoTxShiftStack();
                    return;
                }

                if (chkLedShiftUp.Checked)
                {
                    DoTxLedShiftUp();
                    return;
                }

                if (chkShiftStackUp.Checked)
                {
                    DoTxShiftStackUp();
                    return;
                }

                if (chkMove.Checked)
                {
                    DoTxMove();
                    return;
                }

                DoTx(advanceRgbwShift: true);
            }
            finally
            {
                _ospTxBusy = false;
            }
        }

        /// <summary>Move 좌표를 한 개 전송하고 다음 좌표로 이동한다.</summary>
        private void DoTxMove()
        {
            if (!IsFT4222Active && !IsCANoeActive) return;

            int count = MoveCieCoordinates.GetLength(0);
            if (count == 0) return;

            int index = _moveCoordinateIndex % count;
            byte cx = MoveCieCoordinates[index, 0];
            byte cy = MoveCieCoordinates[index, 1];

            nudColorX.Value = cx;
            nudColorY.Value = cy;

            byte fid = GetFID();
            if (fid > 0x08) return;

            float cieX = ByteToXY(cx);
            float cieY = ByteToXY(cy);
            LedRgb targetRgb = CieToLedRgb(cieX, cieY, (float)nudBrightness.Value / 250.0f);

            int fadeInMs = (int)nudFadeIn.Value * FADE_TIME_UNIT_MS;
            int fadeOutMs = (int)nudFadeOut.Value * FADE_TIME_UNIT_MS;
            int moveFadeMs = fadeInMs > 0 ? fadeInMs : fadeOutMs;
            bool useMoveFade = !chkNoFade.Checked && moveFadeMs > 0;

            var fioNodes = IsOspTxEnabled()
                ? GetFioActiveNodesFromSlaveFlags()
                : new System.Collections.Generic.List<int>();
            int ilasCount = (fid == 0x00)
                ? (IsIlasTxEnabled() && HasAnyIlasActiveTarget() ? GetBroadcastIlasLedCount() : 0)
                : (IsIlasTxEnabled() ? GetIlasLedCountForTarget(fid) : 0);
            var ilasNodes = ilasCount > 0
                ? BuildNodeRange(1, ilasCount)
                : new System.Collections.Generic.List<int>();

            // Fade bookkeeping uses the union only. Physical transmission remains
            // separated by SendColorRgbByFidForSelection() for FIO and ILAS.
            var activeNodes = fioNodes
                .Concat(ilasNodes)
                .Distinct()
                .OrderBy(node => node)
                .ToList();

            if (activeNodes.Count == 0)
            {
                OspMessage?.Invoke("[CIE Move] SlaveFlag 활성 없음 -> 전송 건너뜀");
                return;
            }

            if (useMoveFade)
            {
                fadeTimer.Stop();
                _fadeFid = fid;
                _fadeActiveNodes = activeNodes;
                _fadeStartRgb = new LedRgb[MAX_TRACKED_LED_COUNT + 1];
                _fadeTargetRgb = new LedRgb[MAX_TRACKED_LED_COUNT + 1];
                foreach (int node in activeNodes)
                {
                    _fadeStartRgb[node] = GetCurrentNodeRgb(node);
                    _fadeTargetRgb[node] = targetRgb;
                }

                _fadeIsMove = true;
                _fadeMoveUseFidTx = true;
                _fadePhase = FadePhase.Running;
                _fadeTotalMs = Math.Max(1, moveFadeMs);
                _fadeStopwatch.Restart();
                fadeTimer.Start();

                _selXY = new PointF(cieX, cieY);
                _hasSel = true;
                OspMessage?.Invoke(string.Format(
                    "[CIE Move Fade] {0}/{1} CX=0x{2:X2} CY=0x{3:X2} ({4}ms, {5} nodes)",
                    index + 1, count, cx, cy, moveFadeMs, activeNodes.Count));
                _moveCoordinateIndex = (index + 1) % count;
                return;
            }

            // Move applies one color to each selected path. This helper uses
            // FIO_LED_COUNT for FIO and ISE_LED_COUNT for ILAS independently.
            bool ok = SendColorRgbByFidForSelection(fid, targetRgb);

            if (!ok)
            {
                OspMessage?.Invoke("[CIE Move] OSP TX failed: " +
                    GetOspTransportLastError());
                return;
            }

            foreach (int node in activeNodes)
                RememberNodeRgb(node, targetRgb);

            _selXY = new PointF(cieX, cieY);
            _hasSel = true;

            OspMessage?.Invoke(string.Format(
                "[CIE Move] {0}/{1} CX=0x{2:X2} CY=0x{3:X2} ({4}, {5} nodes, Fade={6}ms)",
                index + 1, count, cx, cy,
                "split FIO/ILAS TX", activeNodes.Count,
                useMoveFade ? moveFadeMs : 0));

            _moveCoordinateIndex = (index + 1) % count;
        }

        /// <summary>
        /// LED_Shift 1 Tick 처리
        ///
        /// 동작 흐름:
        ///   Phase A (순차): _ledShiftStep 번 LED만 ON, 나머지(locked 제외) OFF
        ///     → Tick마다 _ledShiftStep++
        ///     → _ledShiftStep > FIO_LED_COUNT - _ledShiftLocked 이면 마지막 LED가 locked에 추가되고
        ///       _ledShiftStep = 1 로 리셋 (Phase B 진입)
        ///   Phase B (누적): 위 반복. locked가 LED_COUNT개 채워지면 전체 OFF 후 처음부터.
        ///
        /// FID=0x00(Broadcast) 이면 multicast 1회 전송은 불가 — 노드별 unicast 사용.
        /// FID=0x01~0x08 이면 해당 FID의 SF 체크된 노드(activeNodes) 범위 내에서 제어.
        /// </summary>
        private void DoTxLedShift()
        {
            if (!IsFT4222Active && !IsCANoeActive) return;

            byte id = GetFID();
            if (id > 0x08) return;

            float cx = ByteToXY((byte)nudColorX.Value);
            float cy = ByteToXY((byte)nudColorY.Value);
            LedRgb baseRgb = CieToLedRgb(cx, cy, LED_SHIFT_BASE_BRIGHTNESS / 250.0f);
            int width = Math.Max(1, (int)nudShiftWidth.Value);

            if ((id == 0x00 && HasAnyIlasActiveTarget()) || IsSplitFioIlasTarget(id))
            {
                var fioNodes = GetFioActiveNodesFromSlaveFlags();
                var ilasNodes = GetIlasNodesForShiftTarget(id);
                int fioTotal = fioNodes.Count;
                int ilasTotal = ilasNodes.Count;
                if (fioTotal == 0 && ilasTotal == 0) return;

                if (_ledShiftLocked == 0 && _ledShiftStep == 1 && _ledShiftPrevOn.Count == 0)
                {
                    if (fioTotal > 0)
                        SendLedShiftBase(id, fioNodes, baseRgb, _ledShiftFioLastLevels, OspLedPath.FioOnly);
                    if (ilasTotal > 0)
                        SendLedShiftBase(id, ilasNodes, baseRgb, _ledShiftIlasLastLevels, OspLedPath.IlasOnly);
                }

                var fioOn = new System.Collections.Generic.List<int>();
                var ilasOn = new System.Collections.Generic.List<int>();
                if (fioTotal > 0)
                {
                    int fioStartIdx = (_ledShiftStep - 1) % fioTotal;
                    for (int i = 0; i < width; i++)
                        fioOn.Add(fioNodes[(fioStartIdx + i) % fioTotal]);
                }
                if (ilasTotal > 0)
                {
                    int ilasStartIdx = (_ledShiftStep - 1) % ilasTotal;
                    for (int i = 0; i < width; i++)
                        ilasOn.Add(ilasNodes[(ilasStartIdx + i) % ilasTotal]);
                }

                var lockedNodes = new System.Collections.Generic.List<int>();
                if (fioTotal > 0)
                    SendLedShiftWave(id, fioNodes, fioOn, lockedNodes, cx, cy, (int)nudBrightness.Value,
                        _ledShiftFioLastLevels, ref _ledShiftFioLastWaveKey, null, false, OspLedPath.FioOnly);
                if (ilasTotal > 0)
                    SendLedShiftWave(id, ilasNodes, ilasOn, lockedNodes, cx, cy, (int)nudBrightness.Value,
                        _ledShiftIlasLastLevels, ref _ledShiftIlasLastWaveKey, null, false, OspLedPath.IlasOnly);

                string fioStepText = fioTotal > 0 ? string.Format("{0}/{1}", ((_ledShiftStep - 1) % fioTotal) + 1, fioTotal) : "OFF";
                string ilasStepText = ilasTotal > 0 ? string.Format("{0}/{1}", ((_ledShiftStep - 1) % ilasTotal) + 1, ilasTotal) : "OFF";
                if (CIE_SHIFT_VERBOSE_LOG)
                    OspMessage?.Invoke(string.Format(
                        "[CIE LED_Shift] FID=0x{0:X2} split FIO step={1} on={2} / ILAS step={3} on={4}",
                        id, fioStepText, string.Join(",", fioOn),
                        ilasStepText, string.Join(",", ilasOn)));

                _ledShiftPrevOn.Clear();
                _ledShiftPrevOn.AddRange(fioOn);
                _ledShiftStep += width;
                return;
            }

            var activeNodes = GetActiveNodesForTarget(id);
            if (activeNodes.Count == 0) return;

            int totalNodes = activeNodes.Count;
            if (_ledShiftLocked == 0 && _ledShiftStep == 1 && _ledShiftPrevOn.Count == 0)
            {
                SendLedShiftBase(id, activeNodes, baseRgb, _ledShiftLastLevels);
            }

            int startIdx = _ledShiftStep - 1;
            var nowOn = new System.Collections.Generic.List<int>();
            for (int i = 0; i < width; i++)
                nowOn.Add(activeNodes[(startIdx + i) % totalNodes]);

            var lockedNodes2 = new System.Collections.Generic.List<int>();
            SendLedShiftWaveSelected(id, activeNodes, nowOn, lockedNodes2, cx, cy, (int)nudBrightness.Value,
                _ledShiftLastLevels, ref _ledShiftLastWaveKey);

            if (CIE_SHIFT_VERBOSE_LOG)
                OspMessage?.Invoke(string.Format(
                    "[CIE LED_Shift] step={0}/{1} on={2}",
                    _ledShiftStep, totalNodes,
                    string.Join(",", nowOn)));

            _ledShiftPrevOn.Clear();
            _ledShiftPrevOn.AddRange(nowOn);
            _ledShiftStep = ((_ledShiftStep - 1 + width) % totalNodes) + 1;
        }
        private void DoShiftStackPath(
            byte fid,
            System.Collections.Generic.List<int> activeNodes,
            ref int step,
            ref int locked,
            System.Collections.Generic.Dictionary<int, int> lastLevels,
            ref string lastWaveKey,
            OspLedPath path,
            string label)
        {
            if (activeNodes == null || activeNodes.Count == 0) return;

            int totalNodes = activeNodes.Count;
            int width = Math.Max(1, (int)nudShiftWidth.Value);

            if (locked >= totalNodes)
            {
                step = 1;
                locked = 0;
                lastLevels.Clear();
                lastWaveKey = string.Empty;
            }

            float cx = ByteToXY((byte)nudColorX.Value);
            float cy = ByteToXY((byte)nudColorY.Value);
            LedRgb baseRgb = CieToLedRgb(cx, cy, LED_SHIFT_BASE_BRIGHTNESS / 250.0f);

            if (locked == 0 && step == 1)
                SendLedShiftBase(fid, activeNodes, baseRgb, lastLevels, path);

            int movingCount = totalNodes - locked;
            int startIdx = Math.Min(step - 1, movingCount - 1);
            int endIdx = Math.Min(movingCount - 1, startIdx + width - 1);

            var nowOn = new System.Collections.Generic.List<int>();
            for (int i = startIdx; i <= endIdx; i++)
                nowOn.Add(activeNodes[i]);

            var lockedNodes = new System.Collections.Generic.List<int>();
            for (int i = movingCount; i < totalNodes; i++)
                lockedNodes.Add(activeNodes[i]);

            SendLedShiftWaveSelected(fid, activeNodes, nowOn, lockedNodes, cx, cy, (int)nudBrightness.Value,
                lastLevels, ref lastWaveKey, activeNodes, circularWave: false, path: path);

            if (CIE_SHIFT_VERBOSE_LOG)
                OspMessage?.Invoke(string.Format(
                    "[CIE Shift Stack] {0} step={1}/{2} locked={3} on={4}",
                    label, step, movingCount, locked, string.Join(",", nowOn)));

            if (endIdx >= movingCount - 1)
            {
                locked++;
                step = 1;
            }
            else
            {
                step += width;
            }
        }

        private void DoShiftUpPath(
            byte fid,
            System.Collections.Generic.List<int> shiftNodes,
            ref int stepLo,
            ref int stepHi,
            System.Collections.Generic.Dictionary<int, int> lastLevels,
            ref string lastWaveKey,
            OspLedPath path,
            string label)
        {
            if (shiftNodes == null || shiftNodes.Count == 0) return;

            int totalNodes = shiftNodes.Count;
            const int width = 1;
            float cx = ByteToXY((byte)nudColorX.Value);
            float cy = ByteToXY((byte)nudColorY.Value);
            LedRgb baseRgb = CieToLedRgb(cx, cy, LED_SHIFT_BASE_BRIGHTNESS / 250.0f);

            if (stepLo == 0 && stepHi == 0 && lastLevels.Count == 0)
                SendLedShiftBase(fid, shiftNodes, baseRgb, lastLevels, path);

            int centerLow = (totalNodes - 1) / 2;
            int centerHigh = Math.Min(totalNodes - 1, centerLow + 1);
            int seqLo = 0;
            int seqHi = totalNodes - 1;

            int loHiIdx = centerLow - stepLo * width;
            int loLoIdx = loHiIdx - (width - 1);
            int hiLoIdx = centerHigh + stepHi * width;
            int hiHiIdx = hiLoIdx + (width - 1);

            loLoIdx = Math.Max(seqLo, loLoIdx);
            loHiIdx = Math.Max(seqLo, loHiIdx);
            hiLoIdx = Math.Min(seqHi, hiLoIdx);
            hiHiIdx = Math.Min(seqHi, hiHiIdx);

            var nowOn = new System.Collections.Generic.List<int>();
            for (int i = loLoIdx; i <= loHiIdx; i++)
                if (!nowOn.Contains(shiftNodes[i])) nowOn.Add(shiftNodes[i]);
            for (int i = hiLoIdx; i <= hiHiIdx; i++)
                if (!nowOn.Contains(shiftNodes[i])) nowOn.Add(shiftNodes[i]);

            var lockedNodes = new System.Collections.Generic.List<int>();
            SendLedShiftWaveSelected(fid, shiftNodes, nowOn, lockedNodes, cx, cy, (int)nudBrightness.Value,
                lastLevels, ref lastWaveKey, shiftNodes, circularWave: false, path: path);

            if (CIE_SHIFT_VERBOSE_LOG)
                OspMessage?.Invoke(string.Format(
                    "[CIE LED_Shift_Up] {0} stepLo={1} stepHi={2} on={3}",
                    label, stepLo, stepHi, string.Join(",", nowOn)));

            stepLo++;
            stepHi++;
            if (loLoIdx <= seqLo) stepLo = 0;
            if (hiHiIdx >= seqHi) stepHi = 0;
        }

        private void DoShiftStackUpPath(
            byte fid,
            System.Collections.Generic.List<int> shiftNodes,
            ref int stepLo,
            ref int stepHi,
            ref int lockedLo,
            ref int lockedHi,
            System.Collections.Generic.Dictionary<int, int> lastLevels,
            ref string lastWaveKey,
            OspLedPath path,
            string label)
        {
            if (shiftNodes == null || shiftNodes.Count == 0) return;

            int totalNodes = shiftNodes.Count;
            const int width = 1;

            if (lockedLo + lockedHi >= totalNodes)
            {
                stepLo = 0;
                stepHi = 0;
                lockedLo = 0;
                lockedHi = 0;
                lastLevels.Clear();
                lastWaveKey = string.Empty;
            }

            float cx = ByteToXY((byte)nudColorX.Value);
            float cy = ByteToXY((byte)nudColorY.Value);
            LedRgb baseRgb = CieToLedRgb(cx, cy, LED_SHIFT_BASE_BRIGHTNESS / 250.0f);

            if (lockedLo == 0 && lockedHi == 0 && stepLo == 0 && stepHi == 0)
                SendLedShiftBase(fid, shiftNodes, baseRgb, lastLevels, path);

            int seqLo = lockedLo;
            int seqHi = totalNodes - 1 - lockedHi;
            int centerLow = Math.Max(seqLo, Math.Min(seqHi, (totalNodes - 1) / 2));
            int centerHigh = Math.Max(seqLo, Math.Min(seqHi, Math.Min(totalNodes - 1, centerLow + 1)));

            int loHiIdx = centerLow - stepLo * width;
            int loLoIdx = loHiIdx - (width - 1);
            int hiLoIdx = centerHigh + stepHi * width;
            int hiHiIdx = hiLoIdx + (width - 1);

            loLoIdx = Math.Max(seqLo, loLoIdx);
            loHiIdx = Math.Max(seqLo, Math.Min(seqHi, loHiIdx));
            hiLoIdx = Math.Max(seqLo, Math.Min(seqHi, hiLoIdx));
            hiHiIdx = Math.Min(seqHi, hiHiIdx);

            var nowOn = new System.Collections.Generic.List<int>();
            for (int i = loLoIdx; i <= loHiIdx; i++)
                if (!nowOn.Contains(shiftNodes[i])) nowOn.Add(shiftNodes[i]);
            for (int i = hiLoIdx; i <= hiHiIdx; i++)
                if (!nowOn.Contains(shiftNodes[i])) nowOn.Add(shiftNodes[i]);

            var lockedNodes = new System.Collections.Generic.List<int>();
            for (int i = 0; i < seqLo; i++) lockedNodes.Add(shiftNodes[i]);
            for (int i = seqHi + 1; i < totalNodes; i++) lockedNodes.Add(shiftNodes[i]);

            SendLedShiftWaveSelected(fid, shiftNodes, nowOn, lockedNodes, cx, cy, (int)nudBrightness.Value,
                lastLevels, ref lastWaveKey, shiftNodes, circularWave: false, path: path);

            if (CIE_SHIFT_VERBOSE_LOG)
                OspMessage?.Invoke(string.Format(
                    "[CIE Shift Stack Up] {0} stepLo={1} stepHi={2} lockedLo={3} lockedHi={4} on={5}",
                    label, stepLo, stepHi, lockedLo, lockedHi, string.Join(",", nowOn)));

            bool lowArrived = loLoIdx <= seqLo;
            bool highArrived = hiHiIdx >= seqHi;

            if (lowArrived) { lockedLo++; stepLo = 0; } else { stepLo++; }
            if (lockedLo + lockedHi < totalNodes)
            {
                if (highArrived) { lockedHi++; stepHi = 0; } else { stepHi++; }
            }
        }

        /// <summary>Shift와 동일하게 이동하고, 도착한 끝 LED를 LED25부터 역순으로 누적한다.</summary>
        private void DoTxShiftStack()
        {
            if (!IsFT4222Active && !IsCANoeActive) return;

            byte id = GetFID();
            if (id > 0x08) return;

            if ((id == 0x00 && HasAnyIlasActiveTarget()) || IsSplitFioIlasTarget(id))
            {
                DoShiftStackPath(id, GetFioActiveNodesFromSlaveFlags(),
                    ref _shiftStackFioStep, ref _shiftStackFioLocked,
                    _shiftStackFioLastLevels, ref _shiftStackFioLastWaveKey,
                    OspLedPath.FioOnly, "FIO");
                DoShiftStackPath(id, GetIlasNodesForShiftTarget(id),
                    ref _shiftStackIlasStep, ref _shiftStackIlasLocked,
                    _shiftStackIlasLastLevels, ref _shiftStackIlasLastWaveKey,
                    OspLedPath.IlasOnly, "ILAS");
                return;
            }

            var activeNodes = GetActiveNodesForTarget(id);

            if (activeNodes.Count == 0) return;

            int totalNodes = activeNodes.Count;
            int width = Math.Max(1, (int)nudShiftWidth.Value);

            if (_shiftStackLocked >= totalNodes)
            {
                _shiftStackStep = 1;
                _shiftStackLocked = 0;
                _shiftStackLastLevels.Clear();
                _shiftStackLastWaveKey = string.Empty;
            }

            float cx = ByteToXY((byte)nudColorX.Value);
            float cy = ByteToXY((byte)nudColorY.Value);
            LedRgb baseRgb = CieToLedRgb(cx, cy, LED_SHIFT_BASE_BRIGHTNESS / 250.0f);

            if (_shiftStackLocked == 0 && _shiftStackStep == 1)
                SendLedShiftBase(id, activeNodes, baseRgb, _shiftStackLastLevels);

            int movingCount = totalNodes - _shiftStackLocked;
            int startIdx = Math.Min(_shiftStackStep - 1, movingCount - 1);
            int endIdx = Math.Min(movingCount - 1, startIdx + width - 1);

            var nowOn = new System.Collections.Generic.List<int>();
            for (int i = startIdx; i <= endIdx; i++)
                nowOn.Add(activeNodes[i]);

            var lockedNodes = new System.Collections.Generic.List<int>();
            for (int i = movingCount; i < totalNodes; i++)
                lockedNodes.Add(activeNodes[i]);

            SendLedShiftWaveSelected(id, activeNodes, nowOn, lockedNodes, cx, cy, (int)nudBrightness.Value,
                _shiftStackLastLevels, ref _shiftStackLastWaveKey);

            if (CIE_SHIFT_VERBOSE_LOG)
                OspMessage?.Invoke(string.Format(
                    "[CIE Shift Stack] step={0}/{1} locked={2} on={3}",
                    _shiftStackStep, movingCount, _shiftStackLocked, string.Join(",", nowOn)));

            if (endIdx >= movingCount - 1)
            {
                _shiftStackLocked++;
                _shiftStackStep = 1;
            }
            else
            {
                _shiftStackStep += width;
            }
        }

        /// <summary>
        /// LED_Shift_Up 1 Tick 처리 (누적 방식)
        ///
        /// lo(아래)/hi(위) 방향을 각각 독립적으로 추적.
        /// 각 방향이 끝에 도달하면 locked 누적 후 해당 방향만 중앙 리셋.
        /// 양방향 locked 합 = totalNodes 되면 전체 OFF 후 반복.
        ///
        /// width=1, LED2~LED25:
        ///   Tick1 : node13, node14
        ///   Tick2 : node12, node15
        ///   ...
        ///   Tick12: node2, node25
        ///   다음 Tick부터 node13, node14에서 다시 시작
        /// </summary>
        private void DoTxLedShiftUp()
        {
            if (!IsFT4222Active && !IsCANoeActive) return;

            byte id = GetFID();
            if (id > 0x08) return;

            if ((id == 0x00 && HasAnyIlasActiveTarget()) || IsSplitFioIlasTarget(id))
            {
                DoShiftUpPath(id, GetFioShiftUpNodes(),
                    ref _ledShiftUpFioStepLo, ref _ledShiftUpFioStepHi,
                    _ledShiftUpFioLastLevels, ref _ledShiftUpFioLastWaveKey,
                    OspLedPath.FioOnly, "FIO");
                DoShiftUpPath(id, GetIlasNodesForShiftTarget(id),
                    ref _ledShiftUpIlasStepLo, ref _ledShiftUpIlasStepHi,
                    _ledShiftUpIlasLastLevels, ref _ledShiftUpIlasLastWaveKey,
                    OspLedPath.IlasOnly, "ILAS");
                return;
            }

            var shiftNodes = GetShiftUpNodesForTarget(id);

            int totalNodes = shiftNodes.Count;
            const int width = 1;

            // ON 색상
            float cx  = ByteToXY((byte)nudColorX.Value);
            float cy  = ByteToXY((byte)nudColorY.Value);
            float bri = (float)nudBrightness.Value / 250.0f;
            LedRgb onRgb = CieToLedRgb(cx, cy, bri);
            LedRgb baseRgb = CieToLedRgb(cx, cy, LED_SHIFT_BASE_BRIGHTNESS / 250.0f);

            if (_ledShiftUpLockedLo == 0 && _ledShiftUpLockedHi == 0 &&
                _ledShiftUpStepLo == 0 && _ledShiftUpStepHi == 0 &&
                _ledShiftUpPrevOn.Count == 0)
            {
                SendLedShiftBase(id, shiftNodes, baseRgb, _ledShiftUpLastLevels);
                if (IsFT4222Active && !IsIlasTarget(id)) _ft4222.SetColorRgbByNode(id, 1, 0, 0, 0);
            }

            // LED2~LED25의 중앙: node13(idx11), node14(idx12)
            int centerLow  = (totalNodes - 1) / 2;
            int centerHigh = centerLow + 1;

            // 현재 순차 범위: LED2~LED25 전체
            int seqLo = 0;
            int seqHi = totalNodes - 1;

            // lo묶음: centerLow 기준 step*width 만큼 바깥, width개
            // hi묶음: centerHigh 기준 step*width 만큼 바깥, width개
            int loHiIdx = centerLow  - _ledShiftUpStepLo * width;           // 묶음 중앙쪽 끝
            int loLoIdx = loHiIdx - (width - 1);                             // 묶음 외곽쪽 끝
            int hiLoIdx = centerHigh + _ledShiftUpStepHi * width;           // 묶음 중앙쪽 끝
            int hiHiIdx = hiLoIdx + (width - 1);                             // 묶음 외곽쪽 끝

            // 경계 클램프
            loLoIdx = Math.Max(seqLo, loLoIdx);
            loHiIdx = Math.Max(seqLo, loHiIdx);
            hiLoIdx = Math.Min(seqHi, hiLoIdx);
            hiHiIdx = Math.Min(seqHi, hiHiIdx);

            var nowOn = new System.Collections.Generic.List<int>();
            for (int i = loLoIdx; i <= loHiIdx; i++)
                if (!nowOn.Contains(shiftNodes[i])) nowOn.Add(shiftNodes[i]);
            for (int i = hiLoIdx; i <= hiHiIdx; i++)
                if (!nowOn.Contains(shiftNodes[i])) nowOn.Add(shiftNodes[i]);

            var lockedNodes = new System.Collections.Generic.List<int>();

            SendLedShiftWaveSelected(id, shiftNodes, nowOn, lockedNodes, cx, cy, (int)nudBrightness.Value,
                _ledShiftUpLastLevels, ref _ledShiftUpLastWaveKey, shiftNodes, circularWave: false);

            if (CIE_SHIFT_VERBOSE_LOG)
                OspMessage?.Invoke(string.Format(
                    "[CIE LED_Shift_Up] stepLo={0} stepHi={1} on={2}",
                    _ledShiftUpStepLo, _ledShiftUpStepHi,
                    string.Join(",", nowOn)));

            _ledShiftUpPrevOn.Clear();
            _ledShiftUpPrevOn.AddRange(nowOn);

            // step 갱신: 끝에 도달하면 누적 유지 없이 중앙부터 다시 반복
            _ledShiftUpStepLo++;
            _ledShiftUpStepHi++;

            if (loLoIdx <= seqLo)
            {
                _ledShiftUpStepLo = 0;
            }
            if (hiHiIdx >= seqHi)
            {
                _ledShiftUpStepHi = 0;
            }
        }

        /// <summary>Shift Up과 동일하게 양방향 이동하고, 양쪽 끝 LED를 안쪽으로 누적한다.</summary>
        private void DoTxShiftStackUp()
        {
            if (!IsFT4222Active && !IsCANoeActive) return;

            byte id = GetFID();
            if (id > 0x08) return;

            if ((id == 0x00 && HasAnyIlasActiveTarget()) || IsSplitFioIlasTarget(id))
            {
                DoShiftStackUpPath(id, GetFioShiftUpNodes(),
                    ref _shiftStackUpFioStepLo, ref _shiftStackUpFioStepHi,
                    ref _shiftStackUpFioLockedLo, ref _shiftStackUpFioLockedHi,
                    _shiftStackUpFioLastLevels, ref _shiftStackUpFioLastWaveKey,
                    OspLedPath.FioOnly, "FIO");
                DoShiftStackUpPath(id, GetIlasNodesForShiftTarget(id),
                    ref _shiftStackUpIlasStepLo, ref _shiftStackUpIlasStepHi,
                    ref _shiftStackUpIlasLockedLo, ref _shiftStackUpIlasLockedHi,
                    _shiftStackUpIlasLastLevels, ref _shiftStackUpIlasLastWaveKey,
                    OspLedPath.IlasOnly, "ILAS");
                return;
            }

            var shiftNodes = GetShiftUpNodesForTarget(id);

            int totalNodes = shiftNodes.Count;
            const int width = 1;

            if (_shiftStackUpLockedLo + _shiftStackUpLockedHi >= totalNodes)
            {
                _shiftStackUpStepLo = 0;
                _shiftStackUpStepHi = 0;
                _shiftStackUpLockedLo = 0;
                _shiftStackUpLockedHi = 0;
                _shiftStackUpLastLevels.Clear();
                _shiftStackUpLastWaveKey = string.Empty;
            }

            float cx = ByteToXY((byte)nudColorX.Value);
            float cy = ByteToXY((byte)nudColorY.Value);
            LedRgb baseRgb = CieToLedRgb(cx, cy, LED_SHIFT_BASE_BRIGHTNESS / 250.0f);

            if (_shiftStackUpLockedLo == 0 && _shiftStackUpLockedHi == 0 &&
                _shiftStackUpStepLo == 0 && _shiftStackUpStepHi == 0)
            {
                SendLedShiftBase(id, shiftNodes, baseRgb, _shiftStackUpLastLevels);
                if (IsFT4222Active && !IsIlasTarget(id)) _ft4222.SetColorRgbByNode(id, 1, 0, 0, 0);
            }

            int seqLo = _shiftStackUpLockedLo;
            int seqHi = totalNodes - 1 - _shiftStackUpLockedHi;
            int centerLow = (totalNodes - 1) / 2;
            int centerHigh = Math.Min(totalNodes - 1, centerLow + 1);

            centerLow = Math.Max(seqLo, Math.Min(seqHi, centerLow));
            centerHigh = Math.Max(seqLo, Math.Min(seqHi, centerHigh));

            int loHiIdx = centerLow - _shiftStackUpStepLo * width;
            int loLoIdx = loHiIdx - (width - 1);
            int hiLoIdx = centerHigh + _shiftStackUpStepHi * width;
            int hiHiIdx = hiLoIdx + (width - 1);

            loLoIdx = Math.Max(seqLo, loLoIdx);
            loHiIdx = Math.Max(seqLo, Math.Min(seqHi, loHiIdx));
            hiLoIdx = Math.Max(seqLo, Math.Min(seqHi, hiLoIdx));
            hiHiIdx = Math.Min(seqHi, hiHiIdx);

            var nowOn = new System.Collections.Generic.List<int>();
            for (int i = loLoIdx; i <= loHiIdx; i++)
                if (!nowOn.Contains(shiftNodes[i])) nowOn.Add(shiftNodes[i]);
            for (int i = hiLoIdx; i <= hiHiIdx; i++)
                if (!nowOn.Contains(shiftNodes[i])) nowOn.Add(shiftNodes[i]);

            var lockedNodes = new System.Collections.Generic.List<int>();
            for (int i = 0; i < seqLo; i++) lockedNodes.Add(shiftNodes[i]);
            for (int i = seqHi + 1; i < totalNodes; i++) lockedNodes.Add(shiftNodes[i]);

            SendLedShiftWaveSelected(id, shiftNodes, nowOn, lockedNodes, cx, cy, (int)nudBrightness.Value,
                _shiftStackUpLastLevels, ref _shiftStackUpLastWaveKey, shiftNodes, circularWave: false);

            if (CIE_SHIFT_VERBOSE_LOG)
                OspMessage?.Invoke(string.Format(
                    "[CIE Shift Stack Up] stepLo={0} stepHi={1} lockedLo={2} lockedHi={3} on={4}",
                    _shiftStackUpStepLo, _shiftStackUpStepHi,
                    _shiftStackUpLockedLo, _shiftStackUpLockedHi, string.Join(",", nowOn)));

            bool lowArrived = loLoIdx <= seqLo;
            bool highArrived = hiHiIdx >= seqHi;

            if (lowArrived)
            {
                _shiftStackUpLockedLo++;
                _shiftStackUpStepLo = 0;
            }
            else
            {
                _shiftStackUpStepLo++;
            }

            if (_shiftStackUpLockedLo + _shiftStackUpLockedHi < totalNodes)
            {
                if (highArrived)
                {
                    _shiftStackUpLockedHi++;
                    _shiftStackUpStepHi = 0;
                }
                else
                {
                    _shiftStackUpStepHi++;
                }
            }
        }


        /// <summary>DoTx 진입점 – Fade 활성 시 StartFadeOrImmediate로 라우팅</summary>
        private void DoTx(bool advanceRgbwShift = false)
        {
            StartFadeOrImmediate(advanceRgbwShift);
        }

        /// <summary>
        /// LIN 프레임 구성 및 전송 (Fade 없이 즉시 전송)
        /// BuildData() 로 8바이트 데이터를 만들어 LINGateway 로 전송
        /// FT4222 가 연결된 경우 선택한 FID의 LED PWM도 동시에 업데이트
        /// </summary>
        private void DoTxImmediate(bool advanceRgbwShift = false)
        {
            if (TryBeginSunshineSelectionBeforeTx(advanceRgbwShift))
                return;
            
            if (!IsFT4222Active && _gw == null)
            {
                TrySendCANoeCurrentColor();
                return;
            }

            UpdateTxRxFeedbackMode();

            byte id = GetFID();

            var data = BuildData();

            var f = new LINFrame();
            f.FrameID = _ad3306CommandMode ? AD3306LedCommand.FrameId : id;
            f.DataLength = 8;
            f.ChecksumType = (cmbCS.SelectedIndex == 0)
                ? LINChecksumType.Classic
                : LINChecksumType.Enhanced;

            Array.Copy(data, f.Data, 8);
            bool sent = _gw.SendPublisher(f);

            if (_ad3306CommandMode)
            {
                AD3306LedCommand command = BuildAD3306Command();
                if (command.Function != (byte)AD3306LedFunction.Touch)
                {
                    OspMessage?.Invoke(string.Format(
                        "[AD3306 CMD] {0} ID=0x{1:X2} Target/FID=0x{2:X2} LED={3} Func={4} Period={5} Fade=0x{6:X2}",
                        sent ? "TX" : "TX FAIL", AD3306LedCommand.FrameId, command.TargetAndFid,
                        command.LedNumber, command.Function, command.Period, command.Fade));
                    if (lblFT4222Status != null)
                    {
                        lblFT4222Status.Text = sent ? "● AD3306 command TX complete" : "AD3306 command TX failed";
                        lblFT4222Status.ForeColor = sent ? Color.Yellow : Color.OrangeRed;
                    }
                }
                else
                {
                    lblTouchValue.ForeColor = sent ? Color.White : Color.OrangeRed;
                }
                return;
            }

            // ── FT4222 / AD3306 / AD3301 OSP LED 동기화 ──────────────────────
            // SlaveFlag(SF1~SF12) 가 활성화된 LED 노드만 unicast SET_PWM 전송.
            // OSP 체인 주소는 INIT_BIDIR 결과 1-based (SF1→node1, SF2→node2, ...).
            // FID 0x00~0x08 → AD3304 FID 직접 전달.
            if (_ft4222 != null && _ft4222.IsConnected)
            {
                if (id > 0x08)
                {
                    OspMessage?.Invoke(string.Format(
                        "[CIE OSP] FID=0x{0:X2} → OSP LED 제어 대상 아님 (지원: FID 0x00~0x08)", id));
                    return;
                }

                float cx = ByteToXY((byte)nudColorX.Value);
                float cy = ByteToXY((byte)nudColorY.Value);
                float brightness = (float)nudBrightness.Value / 250.0f;

                _selXY = new PointF(cx, cy);
                _hasSel = true;

                // ── FID 0x00 Broadcast ──────────────────────────────────────
                if (id == 0x00)
                {
                    bool rgbwModeBc = chkRGBWShift.Checked;

                    if (rgbwModeBc)
                    {
                        // RGBW_Shift: TX 1회마다 전체 LED가 R → G → B → W → R ... 순환
                        byte level = (byte)Math.Max(0, Math.Min(255, (int)nudBrightness.Value));
                        int phase = _rgbwShiftOffset; // 0=R, 1=G, 2=B, 3=W

                        byte r = 0, g = 0, b = 0;
                        string colorName;
                        switch (phase)
                        {
                            case 0: r = level; colorName = "R"; break;
                            case 1: g = level; colorName = "G"; break;
                            case 2: b = level; colorName = "B"; break;
                            default: r = g = b = level; colorName = "W"; break;
                        }

                        OspMessage?.Invoke(string.Format(
                            "[CIE OSP] FID=0x00 Broadcast RGBW_Shift → 전체 LED {0} (phase={1}, level={2})",
                            colorName, phase, level));

                        // 전체 LED 동일 색상 → multicast 핸들로 1회 전송
                        bool anyFailBc = false;
                        bool okBc = SendColorRgbByFidForSelection(0x00, new LedRgb(r, g, b));
                        if (!okBc)
                        {
                            anyFailBc = true;
                            OspMessage?.Invoke("[CIE OSP] Broadcast RGBW TX failed: "
                                + GetOspTransportLastError());
                        }

                        if (advanceRgbwShift && chkRGBWShift.Checked)
                            _rgbwShiftOffset = (_rgbwShiftOffset + (int)nudShiftWidth.Value) & 0x03;

                        if (lblFT4222Status != null)
                        {
                            lblFT4222Status.Text = anyFailBc
                                ? "FT4222 OSP TX 실패 (FID=0x00 Broadcast RGBW)"
                                : string.Format("● FID 0x00 Broadcast RGBW_Shift: {0} TX complete", colorName);
                            lblFT4222Status.ForeColor = anyFailBc ? Color.OrangeRed : Color.Yellow;
                        }
                    }
                    else
                    {
                        var fioNodes = GetFioActiveNodesFromSlaveFlags();
                        if (fioNodes.Count == 0)
                        {
                            OspMessage?.Invoke("[CIE OSP] SlaveFlag 활성 없음 → 전송 건너뜀");
                            return;
                        }

                        if (fioNodes.Count == FIO_LED_COUNT)
                        {
                            // SF1~SF25가 모두 선택된 경우에만 multicast 1회 전송
                            OspMessage?.Invoke(string.Format(
                                "[CIE OSP] FID=0x00 Broadcast (multicast 1회, SF 전체)  x={0:F4} y={1:F4} bri={2:F3}",
                                cx, cy, brightness));
                            LedRgb broadcastRgb = CieToLedRgb(cx, cy, brightness);
                            bool ok = IsOspTxEnabled()
                                ? SendFioSolidColorForSelection(0x00, broadcastRgb)
                                : true;
                            bool okIlas = SendBroadcastIlasSolidColor(broadcastRgb, cx, cy, brightness);
                            if (lblFT4222Status != null)
                            {
                                lblFT4222Status.Text = ok && okIlas
                                    ? "● FID 0x00 Broadcast TX complete"
                                    : "FT4222 OSP TX 실패 (FID=0x00 Broadcast)";
                                lblFT4222Status.ForeColor = ok && okIlas ? Color.Yellow : Color.OrangeRed;
                            }
                            if (!ok)
                                OspMessage?.Invoke("[CIE OSP] Broadcast TX failed: " + GetOspTransportLastError());
                        }
                        else
                        {
                            // SF 일부만 선택된 경우 FID=0x00이라도 선택 노드만 unicast 전송
                            OspMessage?.Invoke(string.Format(
                                "[CIE OSP] FID=0x00 Broadcast target, SF unicast {0}개 노드  x={1:F4} y={2:F4} bri={3:F3}",
                                fioNodes.Count, cx, cy, brightness));

                            LedRgb fioTargetRgb = CieToLedRgb(cx, cy, brightness);
                            ushort[] nodes = new ushort[fioNodes.Count];
                            byte[] reds = new byte[fioNodes.Count];
                            byte[] greens = new byte[fioNodes.Count];
                            byte[] blues = new byte[fioNodes.Count];
                            for (int i = 0; i < fioNodes.Count; i++)
                            {
                                nodes[i] = (ushort)fioNodes[i];
                                reds[i] = fioTargetRgb.R;
                                greens[i] = fioTargetRgb.G;
                                blues[i] = fioTargetRgb.B;
                            }

                            OspMessage?.Invoke(string.Format(
                                "[CIE OSP] FIO nodes TX FID=0x00 count={0} nodes=[{1}]",
                                fioNodes.Count, string.Join(",", nodes)));

                            bool okBatch = IsOspTxEnabled() ? SendFioSolidColorForSelection(0x00, fioTargetRgb) : true;
                            bool okIlas = SendBroadcastIlasSolidColor(fioTargetRgb, cx, cy, brightness);
                            if (okBatch)
                            {
                                foreach (int nodeAddr in fioNodes)
                                    RememberNodeRgb(nodeAddr, fioTargetRgb);
                            }
                            else
                            {
                                OspMessage?.Invoke("[CIE OSP] FID=0x00 SF FIO batch TX failed: "
                                    + GetOspTransportLastError());
                            }

                            if (lblFT4222Status != null)
                            {
                                lblFT4222Status.Text = okBatch && okIlas
                                    ? string.Format("● FID 0x00 SF FIO/ILAS batch TX complete ({0}개)", fioNodes.Count)
                                    : "FT4222 OSP TX 실패 (FID=0x00 SF FIO/ILAS batch)";
                                lblFT4222Status.ForeColor = okBatch && okIlas ? Color.Yellow : Color.OrangeRed;
                            }
                        }
                    }
                    return;
                }

                string ifLabel = string.Format("FID 0x{0:X2}", id);
                bool rgbwMode = chkRGBWShift.Checked;
                bool hasFioPath = IsOspTxEnabled() && IsFioTarget(id);
                bool hasIlasPath = IsIlasTxEnabled() && IsIlasTarget(id);

                if (!hasFioPath && !hasIlasPath)
                {
                    OspMessage?.Invoke(string.Format("[CIE OSP] FID=0x{0:X2} 활성 FIO/ILAS 없음 → 전송 건너뜀", id));
                    return;
                }

                if (rgbwMode)
                {
                    byte level = (byte)Math.Max(0, Math.Min(255, (int)nudBrightness.Value));
                    int shiftOffset = chkRGBWShift.Checked ? _rgbwShiftOffset : 0;
                    bool anySent = false;
                    bool anyFail = false;

                    if (hasFioPath)
                    {
                        var fioNodes = GetFioActiveNodesFromSlaveFlags();
                        if (fioNodes.Count > 0)
                        {
                            bool okFio = SendRgbwNodesForPath(id, fioNodes, level, shiftOffset, OspLedPath.FioOnly, "FIO SF");
                            anySent |= okFio;
                            anyFail |= !okFio;
                        }
                    }

                    if (hasIlasPath)
                    {
                        int ilasCount = GetIlasLedCountForTarget(id);
                        var ilasNodes = ilasCount > 0 ? BuildNodeRange(1, ilasCount) : new System.Collections.Generic.List<int>();
                        if (ilasNodes.Count > 0)
                        {
                            bool okIlas = SendRgbwNodesForPath(id, ilasNodes, level, shiftOffset, OspLedPath.IlasOnly, "ILAS");
                            anySent |= okIlas;
                            anyFail |= !okIlas;
                        }
                    }

                    if (!anySent && !anyFail)
                    {
                        OspMessage?.Invoke("[CIE OSP] SlaveFlag/ILAS 활성 노드 없음 → 전송 건너뜀");
                        return;
                    }

                    if (lblFT4222Status != null)
                    {
                        lblFT4222Status.Text = anyFail
                            ? string.Format("FT4222 OSP TX 실패 (FID=0x{0:X2} RGBW split)", id)
                            : string.Format("● FID 0x{0:X2} RGBW split TX complete", id);
                        lblFT4222Status.ForeColor = anyFail ? Color.OrangeRed : Color.Yellow;
                    }

                    if (chkRGBWShift.Checked && advanceRgbwShift)
                        _rgbwShiftOffset = (_rgbwShiftOffset + (int)nudShiftWidth.Value) & 0x03;

                    return;
                }

                LedRgb splitTargetRgb = CieToLedRgb(cx, cy, brightness);
                bool sentAnyPath = false;
                bool failedAnyPath = false;

                if (hasFioPath)
                {
                    var fioNodes = GetFioActiveNodesFromSlaveFlags();
                    if (fioNodes.Count > 0)
                    {
                        OspMessage?.Invoke(string.Format(
                            "[CIE OSP] FID=0x{0:X2} ({1}) FIO SF batch {2}개 노드  x={3:F4} y={4:F4} bri={5:F3}",
                            id, ifLabel, fioNodes.Count, cx, cy, brightness));

                        bool okFio = SendFioSolidColorForSelection(id, splitTargetRgb);
                        sentAnyPath |= okFio;
                        failedAnyPath |= !okFio;
                    }
                    else
                    {
                        OspMessage?.Invoke("[CIE OSP] FIO SlaveFlag 활성 없음 → FIO 전송 건너뜀");
                    }
                }

                if (hasIlasPath)
                {
                    int ilasCount = GetIlasLedCountForTarget(id);
                    var ilasNodes = ilasCount > 0 ? BuildNodeRange(1, ilasCount) : new System.Collections.Generic.List<int>();
                    if (ilasNodes.Count > 0)
                    {
                        OspMessage?.Invoke(string.Format(
                            "[CIE OSP] FID=0x{0:X2} ({1}) ILAS batch {2}개 노드  x={3:F4} y={4:F4} bri={5:F3}",
                            id, ifLabel, ilasNodes.Count, cx, cy, brightness));

                        bool okIlas = SendSolidColorNodesForPath(id, ilasNodes, splitTargetRgb, OspLedPath.IlasOnly, "ILAS");
                        sentAnyPath |= okIlas;
                        failedAnyPath |= !okIlas;
                    }
                }

                if (!sentAnyPath && !failedAnyPath)
                {
                    OspMessage?.Invoke("[CIE OSP] 활성 노드 없음 → 전송 건너뜀");
                    return;
                }

                if (lblFT4222Status != null)
                {
                    lblFT4222Status.Text = failedAnyPath
                        ? string.Format("FT4222 OSP TX 실패 (FID=0x{0:X2} split)", id)
                        : string.Format("● FID 0x{0:X2} split TX complete", id);
                    lblFT4222Status.ForeColor = failedAnyPath ? Color.OrangeRed : Color.Yellow;
                }
            }
        }

        private void SendAdiLinStop()
        {
            AD3306LedCommand command = BuildAD3306Command();
            command.Function = (byte)AD3306LedFunction.Stop;
            command.Period = 1;
            command.Fade = 0;

            LINFrame frame = new LINFrame();
            frame.FrameID = AD3306LedCommand.FrameId;
            frame.DataLength = 8;
            frame.ChecksumType = (cmbCS.SelectedIndex == 0)
                ? LINChecksumType.Classic
                : LINChecksumType.Enhanced;
            Array.Copy(command.ToArray(), frame.Data, 8);

            bool sent = _gw.SendPublisher(frame);
            OspMessage?.Invoke(string.Format(
                "[AD3306 CMD] {0} ID=0x{1:X2} Target/FID=0x{2:X2} LED={3} Func=STOP",
                sent ? "STOP TX" : "STOP TX FAIL", AD3306LedCommand.FrameId,
                command.TargetAndFid, command.LedNumber));

            if (lblFT4222Status != null)
            {
                lblFT4222Status.Text = sent ? "● AD3306 command stopped" : "AD3306 Stop TX failed";
                lblFT4222Status.ForeColor = sent ? Color.Yellow : Color.OrangeRed;
            }
        }

        private void SetAdiLinRunningState(bool running)
        {
            btnTxCycle.Text = running ? "■ 주기 TX 중지" : "▶ 주기 TX 시작";
            btnTxCycle.BackColor = running
                ? Color.FromArgb(160, 50, 50)
                : Color.FromArgb(50, 110, 190);
        }

        private bool IsAdiAutonomousEffectSelected()
        {
            AD3306LedFunction function = GetAdiFunction();
            return function == AD3306LedFunction.RgbwShift ||
                   function == AD3306LedFunction.LedShift ||
                   function == AD3306LedFunction.LedShiftUp ||
                   function == AD3306LedFunction.ShiftStack ||
                   function == AD3306LedFunction.ShiftStackUp ||
                   function == AD3306LedFunction.Move;
        }

        private void SetAdiImmediateEffectState(bool running)
        {
            _adiImmediateEffectRunning = running;
            btnTxOnce.Text = running ? "■ Stop" : "즉시 TX";
            btnTxOnce.BackColor = running
                ? Color.FromArgb(160, 50, 50)
                : Color.FromArgb(50, 110, 190);
        }

        /// <summary>FormMain 에서 게이트웨이 재연결 시 호출 – CIE 창이 열려있는 상태에서 재연결해도 반영됨</summary>
        public void UpdateGateway(ILINGateway gw)
        {
            UpdateSunshineGateway(gw);
            _gw = gw;
        }

        public void UpdateAD3306CommandMode(bool enabled)
        {
            _ad3306CommandMode = enabled;
            SetSunshineExtensionVisible(enabled);
            txTimer.Stop();
            fadeTimer.Stop();
            SetAdiImmediateEffectState(false);
            if (enabled)
            {
                _sunSelectionDirty[0] = true;
                _sunSelectionDirty[1] = true;
                _sunTuningDirty = true;
                _sunTemperatureDirty = true;
                SetAdiLinRunningState(false);
            }
            else
            {
                btnTxCycle.Text = "▶ 주기 TX 시작";
                btnTxCycle.BackColor = Color.FromArgb(50, 110, 190);
            }
            touchTrackBar.Enabled = enabled && chkTouch.Checked;
            RefreshPacketPreview();
        }

        public void UpdateBackendSelection(AD3306LedBackend backend)
        {
            _backendSelection = backend;
            RefreshPacketPreview();
        }

        /// <summary>FormMain 에서 FT4222 연결 상태 변경 시 호출</summary>
        public void UpdateFT4222Status(bool connected)
        {
            if (lblFT4222Status != null)
            {
                lblFT4222Status.Text = connected ? "● FT4222 연결됨" : "● FT4222 연결 안됨";
                lblFT4222Status.ForeColor = connected
                    ? System.Drawing.Color.LimeGreen
                    : System.Drawing.Color.FromArgb(160, 160, 180);
            }
            // [Bug Fix] FT4222 연결 상태 변경 시 SF9~SF12 가시성 동적 갱신
            RefreshSlaveFlagVisibility();
        }

        /// <summary>FT4222 연결 상태에 맞춰 SF9~SF12 체크박스 가시성을 갱신합니다.</summary>
        private void RefreshSlaveFlagVisibility()
        {
            CheckBox[] groups = GetSlaveFlagBoxes();
            for (int i = 0; i < groups.Length; i++)
                groups[i].Visible = true;
        }

        /// <summary>
        /// cmbFID 선택에 따라 Frame ID 반환
        /// 인덱스 0: 0x00 (Broadcast) / 인덱스 1~8: 0x01~0x08 고정값 / 인덱스 9: txtManualFID 직접 입력
        ///
        /// [FID → AD3304 채널 매핑]
        ///   0x00 (Broadcast) → FID 0x01~0x08 전체 전송
        ///   0x01 (MLMM0) → AD3304 FID 0x01
        ///   0x02 (MLMM1) → AD3304 FID 0x02
        ///   0x03 (MLMM2) → AD3304 FID 0x03
        ///   0x04 (MLMM3) → AD3304 FID 0x04
        ///   0x05 (MLMM4) → AD3304 FID 0x05
        ///   0x06 (MLMM5) → AD3304 FID 0x06
        ///   0x07 (MLMM6) → AD3304 FID 0x07
        ///   0x08 (MLMM7) → AD3304 FID 0x08
        /// </summary>
        private byte GetFID()
        {
            switch (cmbFID.SelectedIndex)
            {
                case 0: return 0x00;
                case 1: return 0x01;
                case 2: return 0x02;
                case 3: return 0x03;
                case 4: return 0x04;
                case 5: return 0x05;
                case 6: return 0x06;
                case 7: return 0x07;
                case 8: return 0x08;
                default:
                    // "Manual" 선택 시 txtManualFID 파싱, 실패 시 0x00 반환
                    return TryHexByte(txtManualFID.Text, out byte r) ? r : (byte)0x00;
            }
        }

        private bool TryEnsureCANoe(bool silent = false)
        {
            string message;
            if (_canoeBridge.TryAttach(allowStart: !silent, out message))
            {
                if (!silent) OspMessage?.Invoke("[CANoe] " + message);
                return true;
            }

            if (!silent) OspMessage?.Invoke("[CANoe] " + message);
            return false;
        }

        private int GetSelectedCANoeEffectMode()
        {
            if (chkRGBWShift.Checked) return 9;
            if (chkLedShift.Checked) return 10;
            if (chkLedShiftUp.Checked) return 11;
            if (chkShiftStack.Checked) return 12;
            if (chkShiftStackUp.Checked) return 13;
            if (chkMove.Checked) return 16;
            if (!chkNoFade.Checked) return 15;
            return 0;
        }

        private int GetCANoeNodeTxDelayMs()
        {
            return Math.Max(20, Math.Min(100, (int)nudInterval.Value / 2));
        }

        private bool TrySendCANoeDirectColor(byte fid, int targetPath, int node, int firstNode, int lastNode, int width, int periodMs,
            int r, int g, int b, int brightness, out string message)
        {
            if (!_canoeBridge.TryConfigureAd3304Panel(fid, node, targetPath, firstNode, lastNode, width, periodMs,
                    r, g, b, brightness, out message))
                return false;

            if (!_canoeBridge.TrySetPanelVariable("Command", 0, out message))
                return false;

            if (!_canoeBridge.TryStartEffect(1, out message))
                return false;

            return true;
        }

        private byte GetCANoeTargetFid()
        {
            byte fid = GetFID();
            return fid <= 0x08 ? fid : (byte)0x00;
        }

        // CAPL TargetPath: 0=BOTH, 1=FIO(OSP), 2=ILAS.
        private int GetCANoeTargetPath()
        {
            bool osp = IsOspTxEnabled();
            bool ilas = IsIlasTxEnabled();
            if (osp && ilas) return 0;
            if (osp) return 1;
            if (ilas) return 2;
            return -1;
        }

        private int GetCANoeTargetPath(OspLedPath path)
        {
            if (path == OspLedPath.FioOnly) return 1;
            if (path == OspLedPath.IlasOnly) return 2;
            return GetCANoeTargetPath();
        }

        private string GetCANoeTargetPathName(int targetPath)
        {
            if (targetPath == 1) return "OSP(FIO)";
            if (targetPath == 2) return "ILAS";
            return "OSP(FIO)+ILAS";
        }

        private bool TryStartCANoePanelEffect(int effectMode)
        {
            if (effectMode <= 0) return false;
            if (!TryEnsureCANoe()) return false;

            byte fid = GetCANoeTargetFid();
            int targetPath = GetCANoeTargetPath();
            if (targetPath < 0)
            {
                OspMessage?.Invoke("[CANoe] OSP/ILAS가 모두 해제되어 있어 Effect를 시작하지 않습니다.");
                return false;
            }
            int brightness = (int)Math.Round((double)nudBrightness.Value * 100.0 / 250.0);
            float cieX = ByteToXY((byte)nudColorX.Value);
            float cieY = ByteToXY((byte)nudColorY.Value);
            Color color = CieRgb(cieX, cieY);
            if (effectMode == 15)
            {
                LedRgb fadeTarget = CieToLedRgb(cieX, cieY, (float)nudBrightness.Value / 250.0f);
                color = Color.FromArgb(fadeTarget.R, fadeTarget.G, fadeTarget.B);
                brightness = 100;
            }
            int periodMs = Math.Max(1, (int)nudInterval.Value);
            int fadeTotalMs = 0;
            if (effectMode == 15 || effectMode == 16)
            {
                int fadeInMs = (int)nudFadeIn.Value * FADE_TIME_UNIT_MS;
                int fadeOutMs = (int)nudFadeOut.Value * FADE_TIME_UNIT_MS;
                int requestedFadeMs = fadeInMs > 0 ? fadeInMs : fadeOutMs;
                fadeTotalMs = (!chkNoFade.Checked && requestedFadeMs > 0)
                    ? Math.Max(1, requestedFadeMs)
                    : 0;
                if (effectMode == 15 && fadeTotalMs <= 0)
                    fadeTotalMs = Math.Max(1, periodMs);
            }
            int width = Math.Max(1, (int)nudShiftWidth.Value);

            var activeNodes = GetFioActiveNodesFromSlaveFlags();
            int firstNode = 1;
            int lastNode = targetPath == 1 ? FIO_LED_COUNT : ISE_LED_COUNT;
            if (targetPath == 1 && activeNodes.Count > 0 && !AreAllFioNodesSelected(activeNodes))
            {
                firstNode = activeNodes.Min();
                lastNode = activeNodes.Max();
            }

            string message;
            if (!_canoeBridge.TryConfigureAd3304Panel(fid, 0, targetPath, firstNode, lastNode, width, periodMs,
                    color.R, color.G, color.B, brightness, out message))
            {
                OspMessage?.Invoke("[CANoe] Effect configure failed: " + message);
                return false;
            }

            if (!_canoeBridge.TrySetPanelVariable("EffectStep", fadeTotalMs, out message))
            {
                OspMessage?.Invoke("[CANoe] Fade total set failed: " + message);
                return false;
            }

            if (!_canoeBridge.TrySetPanelVariable("Command", 0, out message))
            {
                OspMessage?.Invoke("[CANoe] Effect Command=0 failed: " + message);
                return false;
            }

            if (!_canoeBridge.TryStartEffect(effectMode, out message))
            {
                OspMessage?.Invoke("[CANoe] Effect Command failed: " + message);
                return false;
            }

            OspMessage?.Invoke(string.Format(
                "[CANoe] Effect start mode={0} target={1} FID=0x{2:X2} nodes={3}..{4} width={5} tick={6}ms RGB=({7},{8},{9}) brightness={10}% fadeTotal={11}ms",
                effectMode, GetCANoeTargetPathName(targetPath), fid, firstNode, lastNode, width, periodMs,
                color.R, color.G, color.B, brightness, fadeTotalMs));
            return true;
        }
        private bool TrySendCANoeCurrentColor()
        {
            if (!TryEnsureCANoe()) return false;

            bool ospEnabled = IsOspTxEnabled();
            bool ilasEnabled = IsIlasTxEnabled();
            if (!ospEnabled && !ilasEnabled)
            {
                OspMessage?.Invoke("[CANoe] OSP/ILAS가 모두 해제되어 있어 Color를 전송하지 않습니다.");
                return false;
            }

            byte fid = GetFID();
            if (fid > 0x08) fid = 0x00;

            float cieX = ByteToXY((byte)nudColorX.Value);
            float cieY = ByteToXY((byte)nudColorY.Value);
            LedRgb ledColor = CieToLedRgb(cieX, cieY, (float)nudBrightness.Value / 250.0f);
            Color color = Color.FromArgb(ledColor.R, ledColor.G, ledColor.B);
            int brightness = 100;
            int periodMs = Math.Max(1, (int)nudInterval.Value);
            int width = Math.Max(1, (int)nudShiftWidth.Value);

            var activeNodes = GetFioActiveNodesFromSlaveFlags();
            string message;
            bool sentAny = false;
            bool failedAny = false;

            if (ospEnabled)
            {
                if (activeNodes.Count == 0)
                {
                    OspMessage?.Invoke("[CANoe] FIO SlaveFlag 활성 없음 -> OSP 전송 건너뜀");
                }
                else if (AreAllFioNodesSelected(activeNodes))
                {
                    bool ok = TrySendCANoeDirectColor(fid, 1, 0, 1, FIO_LED_COUNT, width, periodMs,
                        color.R, color.G, color.B, brightness, out message);
                    sentAny |= ok;
                    failedAny |= !ok;
                    if (!ok)
                        OspMessage?.Invoke("[CANoe] OSP(FIO) Color TX failed: " + message);
                }
                else
                {
                    bool sentAll = true;
                    foreach (int node in activeNodes)
                    {
                        if (!TrySendCANoeDirectColor(fid, 1, node, node, node, width, periodMs,
                                color.R, color.G, color.B, brightness, out message))
                        {
                            OspMessage?.Invoke(string.Format(
                                "[CANoe] OSP selected node={0} Color TX failed: {1}", node, message));
                            sentAll = false;
                            break;
                        }
                        System.Threading.Thread.Sleep(GetCANoeNodeTxDelayMs());
                    }

                    sentAny |= sentAll;
                    failedAny |= !sentAll;
                }
            }

            if (ilasEnabled)
            {
                bool ok = TrySendCANoeDirectColor(fid, 2, 0, 1, ISE_LED_COUNT, width, periodMs,
                    color.R, color.G, color.B, brightness, out message);
                sentAny |= ok;
                failedAny |= !ok;
                if (!ok)
                    OspMessage?.Invoke("[CANoe] ILAS Color TX failed: " + message);
            }

            OspMessage?.Invoke(string.Format(
                "[CANoe] Color TX target={0} FID=0x{1:X2} RGB=({2},{3},{4}) brightness={5}%",
                GetCANoeTargetPathName(GetCANoeTargetPath()), fid, color.R, color.G, color.B, brightness));
            return sentAny && !failedAny;
        }
        private bool TrySendCANoeEffectCommand(int effectMode)
        {
            return TryStartCANoePanelEffect(effectMode);
        }

        private bool TrySendCANoeEffectCommand(int effectMode, bool allowInitRequest)
        {
            return TryStartCANoePanelEffect(effectMode);
        }

        private void CANoeInitDelayTimer_Tick(object sender, EventArgs e)
        {
            _canoeInitDelayTimer.Stop();

            int effectMode = _pendingCANoeEffectMode;
            _pendingCANoeEffectMode = 0;
            if (effectMode <= 0) return;

            TrySendCANoeEffectCommand(effectMode, allowInitRequest: false);
        }
        private void StopCANoeCycleIfRunning()
        {
            if (!_canoeCycleActive) return;

            string message;
            if (_canoeBridge.TryStopEffect(out message))
                OspMessage?.Invoke("[CANoe] " + message);

            _canoeCycleActive = false;
            btnTxCycle.Text = "▶ 주기 TX 시작";
            btnTxCycle.BackColor = Color.FromArgb(50, 110, 190);
        }

        /// <summary>
        /// LIN Gateway 또는 CANoe 연결 상태 확인
        /// silent=true 면 메시지박스 없이 false 반환 (타이머 Tick 용)
        /// </summary>
        private bool CheckGW(bool silent = false)
        {
            if (_gw != null && _gw.IsConnected) return true;
            if (TryEnsureCANoe(silent)) return true;
            if (!silent) MessageBox.Show("LIN Gateway/CANoe 미연결 상태입니다.", "알림");
            return false;
        }

        // =========================================================================
        // 공통 이벤트 핸들러
        // =========================================================================

        /// <summary>SlaveFlag, FadeIn/Out, NoFade, SOH, LINErr 등 변경 시 패킷 미리보기 갱신</summary>
        private bool _changingShiftMode;

        private void ShiftMode_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox selected = sender as CheckBox;
            if (_changingShiftMode || selected == null) return;

            if (selected.Checked)
            {
                _changingShiftMode = true;
                CheckBox[] modes =
                {
                    chkRGBWShift, chkLedShift, chkLedShiftUp,
                    chkShiftStack, chkShiftStackUp, chkMove, chkTouch
                };
                foreach (CheckBox mode in modes)
                {
                    if (!ReferenceEquals(mode, selected)) mode.Checked = false;
                }
                _changingShiftMode = false;
            }

            touchTrackBar.Enabled = _ad3306CommandMode && chkTouch.Checked;

            RefreshPacketPreview();
        }

        private void AnyField_Changed(object sender, EventArgs e) => RefreshPacketPreview();

        /// <summary>ColorX NUD 값 변경 – 색상 패치와 패킷 미리보기 갱신</summary>
        private void nudColorX_ValueChanged(object sender, EventArgs e)
        {
            PointF coordinate = new PointF(
                ByteToXY((byte)nudColorX.Value),
                ByteToXY((byte)nudColorY.Value));
            pnlColorPatch.BackColor = CieRgb(coordinate.X, coordinate.Y);
            RefreshPacketPreview();
        }

        /// <summary>ColorY NUD 값 변경 – 색상 패치와 패킷 미리보기 갱신</summary>
        private void nudColorY_ValueChanged(object sender, EventArgs e)
        {
            PointF coordinate = new PointF(
                ByteToXY((byte)nudColorX.Value),
                ByteToXY((byte)nudColorY.Value));
            pnlColorPatch.BackColor = CieRgb(coordinate.X, coordinate.Y);
            RefreshPacketPreview();
        }

        /// <summary>
        /// Frame ID 콤보박스 변경 이벤트
        /// - "직접입력" 선택 시 txtManualFID 활성화
        /// </summary>
        private void cmbFID_SelectedIndexChanged(object sender, EventArgs e)
        {
            txtManualFID.Enabled = (cmbFID.SelectedIndex == 9);
            grpSF.Enabled = true;
            grpColor.Enabled = true;
            grpFade.Enabled = true;
            grpStatus.Enabled = true;
            btnTxOnce.Text = "즉시 TX";

            if (lblFT4222Status != null)
            {
                byte fid = GetFID();
                switch (fid)
                {
                    case 0x00:
                        lblFT4222Status.Text = "● FID 0x00 → Broadcast (전체 LED)";
                        lblFT4222Status.ForeColor = Color.Yellow;
                        break;
                    case 0x01:
                        lblFT4222Status.Text = "● FID 0x01 선택됨";
                        lblFT4222Status.ForeColor = Color.LimeGreen;
                        break;
                    case 0x02:
                        lblFT4222Status.Text = "● FID 0x02 선택됨";
                        lblFT4222Status.ForeColor = Color.DeepSkyBlue;
                        break;
                    case 0x03:
                        lblFT4222Status.Text = "● FID 0x03 → AD3304 채널 선택됨";
                        lblFT4222Status.ForeColor = Color.LimeGreen;
                        break;
                    case 0x04:
                        lblFT4222Status.Text = "● FID 0x04 → AD3304 채널 선택됨";
                        lblFT4222Status.ForeColor = Color.LimeGreen;
                        break;
                    case 0x05:
                        lblFT4222Status.Text = "● FID 0x05 → AD3304 채널 선택됨";
                        lblFT4222Status.ForeColor = Color.LimeGreen;
                        break;
                    case 0x06:
                        lblFT4222Status.Text = "● FID 0x06 → AD3304 채널 선택됨";
                        lblFT4222Status.ForeColor = Color.LimeGreen;
                        break;
                    case 0x07:
                        lblFT4222Status.Text = "● FID 0x07 → AD3304 채널 선택됨";
                        lblFT4222Status.ForeColor = Color.LimeGreen;
                        break;
                    case 0x08:
                        lblFT4222Status.Text = "● FID 0x08 → AD3304 채널 선택됨";
                        lblFT4222Status.ForeColor = Color.LimeGreen;
                        break;
                    default:
                        lblFT4222Status.Text = string.Format("⚠ FID 0x{0:X2} → Manual 입력값", fid);
                        lblFT4222Status.ForeColor = Color.Orange;
                        break;
                }
            }
        }

        /// <summary>SlaveFlag 전체 선택 버튼 클릭</summary>
        private void btnSFAll_Click(object sender, EventArgs e)
        {
            ApplySlaveFlagLimit(checkEnabledFlags: true);
            RefreshPacketPreview();
        }

        /// <summary>SlaveFlag 전체 해제 버튼 클릭</summary>
        private void btnSFNone_Click(object sender, EventArgs e)
        {
            foreach (CheckBox cb in GetSlaveFlagBoxes())
                cb.Checked = false;
            RefreshPacketPreview();
        }

        /// <summary>폼 닫힐 때 타이머, 비트맵 리소스 해제</summary>
        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            // txTimer / fadeTimer 는 components 에 등록되어 있으므로 Dispose() 자동 호출됨
            fadeTimer.Stop();
            _fadeStopwatch.Stop();
            _fadePhase = FadePhase.None;
            _cieBmp?.Dispose();
            _imgBmp?.Dispose();
            CloseSunshineExtension();
            base.OnFormClosed(e);
        }


        // =========================================================================
        // 유틸리티
        // =========================================================================

        /// <summary>HEX 문자열 → byte 변환 ("0x02", "02", "2" 모두 허용)</summary>
        private static bool TryHexByte(string s, out byte r)
        {
            r = 0;
            if (string.IsNullOrWhiteSpace(s)) return false;
            try
            {
                r = Convert.ToByte(s.Trim().Replace("0x", "").Replace("0X", ""), 16);
                return true;
            }
            catch { return false; }
        }
    }
}




































