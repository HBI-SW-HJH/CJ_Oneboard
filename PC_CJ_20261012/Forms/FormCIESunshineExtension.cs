using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using LINMaster.LIN;

namespace LINMaster.Forms
{
    public partial class FormCIE
    {
        private const byte SUN_EXT_REQUEST = 0xE0;
        private const byte SUN_EXT_RESPONSE = 0xF0;
        private const byte SUN_EXT_LIN_RESPONSE_ID = 0x3B;
        private const byte SUN_TARGET_FIO = 0x01;
        private const byte SUN_TARGET_ISELED = 0x02;
        private const byte SUN_OP_SELECT_MODE = 0x01;
        private const byte SUN_OP_INDIVIDUAL_MASK = 0x02;
        private const byte SUN_OP_GROUP_MASK = 0x03;
        private const byte SUN_OP_SELECTION_READ = 0x04;
        private const byte SUN_OP_GROUP_BOUNDARIES = 0x05;
        private const byte SUN_OP_SCAN_START = 0x10;
        private const byte SUN_OP_SCAN_STATUS = 0x11;
        private const byte SUN_OP_OSP_DATA = 0x12;
        private const byte SUN_OP_ISELED_DATA = 0x13;
        private const byte SUN_OP_TUNING_ENABLE = 0x14;
        private const byte SUN_OP_USB_BULK_READ = 0x15;
        private const byte SUN_OP_OSP_CURRENT_MODE = 0x16;
        private const byte SUN_OP_LIN_FAST_BULK_READ = 0x17;
        private const byte SUN_OP_LIN_FAST_CONTROL = 0x18;
        private const byte SUN_OP_TEMPERATURE_ENABLE = 0x19;
        private const byte SUN_LIN_FAST_START = 0xF0;
        private const byte SUN_LIN_FAST_FID_BEGIN = 0xF1;
        private const byte SUN_LIN_FAST_FID_END = 0xF2;
        private const byte SUN_LIN_FAST_COMPLETE = 0xF3;
        private const byte SUN_LIN_FAST_ERROR = 0xFF;
        private const byte SUN_LIN_FAST_RETRY = 0x01;
        private const byte SUN_LIN_FAST_NEXT = 0x02;
        private const byte SUN_LIN_FAST_ABORT = 0x03;
        private const byte SUN_LIN_FAST_STATUS = 0x04;
        private const byte SUN_LIN_FAST_PROTOCOL_VERSION = 0x07;
        private const byte SUN_LIN_FAST_PAYLOAD_XOR = 0xA5;
        private const byte SUN_LIN_FAST_RETRY_XOR = 0x5A;
        private const int SUN_LIN_FAST_RECORD_SIZE = 20;
        private const int SUN_LIN_FAST_PAYLOAD_SIZE = 7;
        private const int SUN_LIN_FAST_SUBSCRIBER_DELAY_MS = 12;
        private const int SUN_LIN_FAST_SUBSCRIBER_TIMEOUT_MS = 75;
        private const int SUN_LIN_FAST_CONTROL_DELAY_MS = 2;
        private const int SUN_LIN_FAST_CONTROL_TIMEOUT_MS = 500;
        private const ushort SUN_LIN_FAST_SCHEDULE_PERIOD_MS = 7;
        private const byte SUN_OSP_CURRENT_DAY = 0x00;
        private const byte SUN_OSP_CURRENT_NIGHT = 0x01;
        private const byte SUN_USB_BULK_FRAME_ID = 0x3C;
        private const byte SUN_USB_BULK_START = 0x01;
        private const byte SUN_USB_BULK_FID_BEGIN = 0x02;
        private const byte SUN_USB_BULK_FID_END = 0x03;
        private const byte SUN_USB_BULK_COMPLETE = 0x04;
        private const byte SUN_USB_BULK_ERROR = 0x7F;

        private ComboBox _sunTarget;
        private RadioButton _sunAll;
        private RadioButton _sunIndividual;
        private RadioButton _sunGroup;
        private TextBox _sunIndividualText;
        private CheckBox[] _sunLedSelectors;
        private bool _sunLedSelectorDragging;
        private bool _sunLedSelectorDragState;
        private CheckBox[] _sunGroupChecks;
        private Panel _sunGroupBar;
        private Panel[] _sunGroupSegments;
        private Label[] _sunGroupPointers;
        private Label[] _sunGroupBoundaryLabels;
        private int _sunDragBoundary = -1;
        private Button _sunApply;
        private CheckBox _sunTuning;
        private CheckBox _sunTemperature;
        private RadioButton _sunDay;
        private RadioButton _sunNight;
        private Label _sunHint;
        private Button _sunRead;
        private Label _sunStatus;
        private DataGridView _sunGrid;
        private Size _sunBaseClientSize;
        private bool _sunLoadingUi;
        private int _sunUiTargetIndex;
        private int[] _sunFioBoundaries;
        private int[] _sunIseBoundaries;
        private readonly uint[][] _sunIndividualMasks = { new uint[3], new uint[3] };
        private readonly byte[] _sunModes = { 0, 0 };
        private readonly byte[] _sunSelectedGroups = new byte[2];
        private readonly bool[] _sunSelectionDirty = { true, true };
        private readonly bool[] _sunIndividualMaskDirty = { true, true };
        private readonly bool[] _sunGroupMaskDirty = { true, true };
        private readonly int[] _sunLedCounts = { FIO_LED_COUNT, ISE_LED_COUNT };
        private readonly bool[] _sunLedCountKnown = { false, false };
        private bool _sunTuningEnabled;
        private bool _sunTuningDirty = true;
        private bool _sunTemperatureEnabled;
        private bool _sunTemperatureDirty = true;
        private byte _sunOspCurrentMode = SUN_OSP_CURRENT_DAY;
        private bool _sunOspCurrentModeDirty = true;

        private readonly Queue<byte[]> _sunConfigPackets = new Queue<byte[]>();
        private readonly Timer _sunConfigTimer = new Timer();
        private Action _sunConfigCompleted;
        private byte _sunConfigTargetMask;
        private byte _sunConfigIndividualMask;
        private byte _sunConfigGroupMask;
        private bool _sunConfigFailed;
        private bool _sunConfigOspCurrentModeIncluded;

        private sealed class SunshineReadRequest
        {
            public byte[] Packet;
            public Action<byte[]> Handler;
        }

        private sealed class SunshineFidRead
        {
            public byte Fid;
            public int LedCount;
        }

        private sealed class SunshineOspRecord
        {
            public byte Valid;
            public byte Error;
            public byte Open;
            public byte Short;
            public ushort[] Factor = new ushort[3];
            public ushort[] Pwm = new ushort[3];
            public short Temperature;
            public byte TemperatureRaw;
            public byte LedStatus;
        }

        private sealed class SunshineIseRecord
        {
            public byte Valid;
            public byte Error;
            public byte OpenShort;
            public ushort TemperatureRaw;
            public short Temperature;
            public ushort[] Pwm = new ushort[3];
            public ushort[] PwmMax = new ushort[3];
            public ushort DeviceStatus;
            public byte PwmValidMask;
            public byte PwmMaxValidMask;
        }

        private readonly Queue<SunshineReadRequest> _sunReadRequests = new Queue<SunshineReadRequest>();
        private readonly Queue<SunshineFidRead> _sunPendingFidReads = new Queue<SunshineFidRead>();
        private readonly Dictionary<byte, SunshineOspRecord[]> _sunOspRecordsByFid = new Dictionary<byte, SunshineOspRecord[]>();
        private readonly Dictionary<byte, SunshineIseRecord[]> _sunIseRecordsByFid = new Dictionary<byte, SunshineIseRecord[]>();
        private readonly Timer _sunReadTimer = new Timer();
        private System.Threading.Timer _sunUsbReadTimer;
        private int _sunUsbTimerBusy;
        private bool _sunBulkReadActive;
        private byte _sunBulkProtocolVersion;
        private byte _sunBulkActiveMask;
        private byte _sunBulkCompletedMask;
        private byte _sunBulkError;
        private byte _sunBulkRemoteCode;
        private readonly HashSet<int> _sunBulkPages = new HashSet<int>();
        private byte[] _sunBulkRequestPacket;
        private DateTime _sunBulkLastRequestAt;
        private int _sunBulkStartRetry;
        private bool _sunBulkStarted;
        private bool _sunLinFastMode;
        private bool _sunLinHardwareSchedule;
        private byte _sunLinFastFid;
        private int _sunLinFastLedCount;
        private int _sunLinFastChunkCount;
        private byte[] _sunLinFastData;
        private bool[] _sunLinFastReceived;
        private ushort _sunLinFastExpectedCrc;
        private readonly Queue<byte> _sunLinFastMissingChunks = new Queue<byte>();
        private bool _sunLinFastRetryWaiting;
        private byte _sunLinFastRetryChunk;
        private DateTime _sunLinFastRetrySentAt;
        private int _sunLinFastRetryCount;
        private int _sunLinFastCrcRetryCount;
        private DateTime _sunLinFastLastFrameAt;
        private DateTime _sunLinFastStatusSentAt;
        private int _sunLinFastStatusRetryCount;
        private DateTime _sunLinFastNextSubscriberAt;
        private bool _sunLinFastSubscriberPending;
        private DateTime _sunLinFastSubscriberSentAt;
        private bool _sunLinFastPendingFidAck;
        private byte _sunLinFastPendingFid;
        private DateTime _sunLinFastPendingFidSentAt;
        private int _sunLinFastPendingFidRetryCount;
        private byte _sunLinFastControlSequence;
        private readonly HashSet<byte> _sunLinFastFailedFids = new HashSet<byte>();
        private readonly object _sunLinFastPublisherSync = new object();
        private readonly Queue<byte[]> _sunLinFastPublisherQueue = new Queue<byte[]>();
        private DateTime _sunLinFastPublisherDueAt;
        private SunshineReadRequest _sunReadCurrent;
        private DateTime _sunReadSentAt;
        private DateTime _sunSubscriberSentAt;
        private DateTime _sunReadStartedAt;
        private DateTime _sunScanStartedAt;
        private bool _sunSubscriberSent;
        private int _sunReadRetry;
        private int _sunReadTotal;
        private int _sunReadCompleted;
        private bool _sunReadActive;
        private int _sunReadTargetIndex;
        private byte _sunRequestedFid;
        private byte _sunReadFid;
        private int _sunReadMaximumLedCount;
        private int _sunReadTotalLeds;
        private bool _sunReadShowGrid;
        private Action _sunReadFinished;
        private SunshineOspRecord[] _sunOspRecords;
        private SunshineIseRecord[] _sunIseRecords;

        private void InitializeSunshineExtension()
        {
            _sunBaseClientSize = new Size(sunshinePanel.Left - 5, ClientSize.Height);
            _sunTarget = sunTargetComboBox;
            _sunAll = sunAllRadioButton;
            _sunIndividual = sunIndividualRadioButton;
            _sunGroup = sunGroupRadioButton;
            _sunIndividualText = sunIndividualTextBox;
            _sunLedSelectors = new[] { sunLedSelectCheckBox1, sunLedSelectCheckBox2, sunLedSelectCheckBox3, sunLedSelectCheckBox4, sunLedSelectCheckBox5, sunLedSelectCheckBox6, sunLedSelectCheckBox7, sunLedSelectCheckBox8, sunLedSelectCheckBox9, sunLedSelectCheckBox10, sunLedSelectCheckBox11, sunLedSelectCheckBox12, sunLedSelectCheckBox13, sunLedSelectCheckBox14, sunLedSelectCheckBox15, sunLedSelectCheckBox16, sunLedSelectCheckBox17, sunLedSelectCheckBox18, sunLedSelectCheckBox19, sunLedSelectCheckBox20, sunLedSelectCheckBox21, sunLedSelectCheckBox22, sunLedSelectCheckBox23, sunLedSelectCheckBox24, sunLedSelectCheckBox25, sunLedSelectCheckBox26, sunLedSelectCheckBox27, sunLedSelectCheckBox28, sunLedSelectCheckBox29, sunLedSelectCheckBox30, sunLedSelectCheckBox31, sunLedSelectCheckBox32, sunLedSelectCheckBox33, sunLedSelectCheckBox34, sunLedSelectCheckBox35, sunLedSelectCheckBox36, sunLedSelectCheckBox37, sunLedSelectCheckBox38, sunLedSelectCheckBox39, sunLedSelectCheckBox40, sunLedSelectCheckBox41, sunLedSelectCheckBox42, sunLedSelectCheckBox43, sunLedSelectCheckBox44, sunLedSelectCheckBox45, sunLedSelectCheckBox46, sunLedSelectCheckBox47, sunLedSelectCheckBox48, sunLedSelectCheckBox49, sunLedSelectCheckBox50, sunLedSelectCheckBox51, sunLedSelectCheckBox52, sunLedSelectCheckBox53, sunLedSelectCheckBox54, sunLedSelectCheckBox55, sunLedSelectCheckBox56, sunLedSelectCheckBox57, sunLedSelectCheckBox58, sunLedSelectCheckBox59, sunLedSelectCheckBox60, sunLedSelectCheckBox61, sunLedSelectCheckBox62, sunLedSelectCheckBox63, sunLedSelectCheckBox64, sunLedSelectCheckBox65, sunLedSelectCheckBox66, sunLedSelectCheckBox67, sunLedSelectCheckBox68, sunLedSelectCheckBox69, sunLedSelectCheckBox70, sunLedSelectCheckBox71, sunLedSelectCheckBox72, sunLedSelectCheckBox73, sunLedSelectCheckBox74, sunLedSelectCheckBox75, sunLedSelectCheckBox76, sunLedSelectCheckBox77, sunLedSelectCheckBox78, sunLedSelectCheckBox79, sunLedSelectCheckBox80, sunLedSelectCheckBox81, sunLedSelectCheckBox82, sunLedSelectCheckBox83, sunLedSelectCheckBox84 };
            _sunGroupChecks = new[] { sunGroupCheckBox1, sunGroupCheckBox2, sunGroupCheckBox3, sunGroupCheckBox4 };
            byte initialGroups = 0;
            for (int group = 0; group < _sunGroupChecks.Length; group++)
            {
                if (_sunGroupChecks[group].Checked)
                {
                    initialGroups |= (byte)(1U << group);
                }
            }
            if (initialGroups == 0)
            {
                initialGroups = 0x01;
            }
            _sunSelectedGroups[0] = initialGroups;
            _sunSelectedGroups[1] = initialGroups;
            _sunGroupBar = sunGroupBarPanel;
            _sunGroupSegments = new[] { sunGroupSegment1, sunGroupSegment2, sunGroupSegment3, sunGroupSegment4 };
            _sunGroupPointers = new[] { sunGroupPointer1, sunGroupPointer2, sunGroupPointer3 };
            _sunGroupBoundaryLabels = new[] { sunGroupValue1, sunGroupValue2, sunGroupValue3 };
            _sunApply = sunApplyButton;
            _sunTuning = sunTuningCheckBox;
            _sunTemperature = sunTemperatureCheckBox;
            _sunDay = sunDayRadioButton;
            _sunNight = sunNightRadioButton;
            _sunHint = sunHintLabel;
            _sunRead = sunReadButton;
            _sunStatus = sunStatusLabel;
            _sunGrid = sunReadGrid;
            _sunFioBoundaries = CreateSunshineBoundaries(_sunLedCounts[0]);
            _sunIseBoundaries = CreateSunshineBoundaries(_sunLedCounts[1]);

            _sunTarget.SelectedIndexChanged += SunshineTargetChanged;
            _sunAll.CheckedChanged += SunshineSelectionChanged;
            _sunIndividual.CheckedChanged += SunshineSelectionChanged;
            _sunGroup.CheckedChanged += SunshineSelectionChanged;
            _sunIndividualText.TextChanged += SunshineIndividualTextChanged;
            _sunApply.Click += SunshineApply_Click;
            sunLedSelectAllButton.Click += SunshineLedSelectAll_Click;
            sunLedSelectClearButton.Click += SunshineLedSelectClear_Click;
            sunLedSelectInvertButton.Click += SunshineLedSelectInvert_Click;
            sunLedSelectionGroupBox.MouseUp += SunshineLedSelector_MouseUp;
            for (int led = 0; led < _sunLedSelectors.Length; led++)
            {
                _sunLedSelectors[led].MouseDown += SunshineLedSelector_MouseDown;
                _sunLedSelectors[led].MouseEnter += SunshineLedSelector_MouseEnter;
                _sunLedSelectors[led].MouseUp += SunshineLedSelector_MouseUp;
            }
            _sunTuning.CheckedChanged += SunshineTuningChanged;
            _sunTemperature.CheckedChanged += SunshineTemperatureChanged;
            _sunDay.CheckedChanged += SunshineOspCurrentModeChanged;
            _sunNight.CheckedChanged += SunshineOspCurrentModeChanged;
            _sunRead.Click += SunshineRead_Click;
            _sunGroupBar.Resize += SunshineGroupBar_Resize;
            for (int pointer = 0; pointer < _sunGroupPointers.Length; pointer++)
            {
                _sunGroupPointers[pointer].MouseDown += SunshineGroupPointer_MouseDown;
                _sunGroupPointers[pointer].MouseMove += SunshineGroupPointer_MouseMove;
                _sunGroupPointers[pointer].MouseUp += SunshineGroupPointer_MouseUp;
                _sunGroupPointers[pointer].MouseCaptureChanged += SunshineGroupPointer_MouseCaptureChanged;
            }
            for (int group = 0; group < _sunGroupChecks.Length; group++)
                _sunGroupChecks[group].CheckedChanged += SunshineSelectionChanged;

            _sunTarget.SelectedIndex = 0;
            _sunDay.Checked = true;
            LoadSunshineTargetUi(0);
            _sunApply.Text = "선택 설정 적용";
            _sunHint.Text = "적용 후 왼쪽 'ADI 기능 1회 TX'를 누르면 선택한 LED만 제어됩니다.";
            UpdateSunshineCountText();

            _sunConfigTimer.Interval = 15;
            _sunConfigTimer.Tick += SunshineConfigTimer_Tick;
            _sunReadTimer.Interval = 10;
            _sunReadTimer.Tick += SunshineReadTimer_Tick;
            _sunUsbReadTimer = new System.Threading.Timer(SunshineUsbReadTimer_Tick, null, System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
            AttachSunshineGateway(_gw);
            bool designVisible = System.ComponentModel.LicenseManager.UsageMode == System.ComponentModel.LicenseUsageMode.Designtime;
            SetSunshineExtensionVisible(designVisible || _ad3306CommandMode);
        }

        private static int[] CreateSunshineBoundaries(int ledCount)
        {
            return new[]
            {
                (ledCount + 3) / 4,
                ((ledCount * 2) + 3) / 4,
                ((ledCount * 3) + 3) / 4
            };
        }

        private void ApplySunshineLedCount(int index, int ledCount)
        {
            int maximum = index == 0 ? FIO_LED_COUNT : ISE_LED_COUNT;
            if (index < 0 || index > 1 || ledCount < 1 || ledCount > maximum)
                return;
            if (_sunLedCounts[index] != ledCount)
            {
                _sunLedCounts[index] = ledCount;
                if (index == 0)
                    _sunFioBoundaries = CreateSunshineBoundaries(ledCount);
                else
                    _sunIseBoundaries = CreateSunshineBoundaries(ledCount);
                for (int led = ledCount + 1; led <= maximum; led++)
                    _sunIndividualMasks[index][(led - 1) / 32] &= ~(1U << ((led - 1) % 32));
                _sunSelectionDirty[index] = true;
                _sunIndividualMaskDirty[index] = true;
                _sunGroupMaskDirty[index] = true;
            }
            if (_sunUiTargetIndex == index)
                LoadSunshineTargetUi(index);
            UpdateSunshineCountText();
        }

        private void UpdateSunshineCountText()
        {
            if (_sunRead == null || _sunHint == null)
                return;
            _sunRead.Text = _sunUiTargetIndex == 0
                ? "OSP " + _sunLedCounts[0] + "개 Read"
                : "ISELED " + _sunLedCounts[1] + "개 Read";
            _sunHint.Text = "확인 수량 OSP " + _sunLedCounts[0] + " / ISE " + _sunLedCounts[1] + ". 적용 후 왼쪽 'ADI 기능 1회 TX'로 제어합니다.";
        }

        private void SetSunshineExtensionVisible(bool visible)
        {
            if (sunshinePanel == null)
                return;
            sunshinePanel.Visible = visible;
            if (visible)
            {
                ClientSize = new Size(_sunBaseClientSize.Width + 1050, _sunBaseClientSize.Height);
                sunshinePanel.Location = new Point(_sunBaseClientSize.Width + 5, 0);
                sunshinePanel.Size = new Size(1045, _sunBaseClientSize.Height);
                MinimumSize = new Size(_sunBaseClientSize.Width + 1070, 620);
            }
            else
            {
                ClientSize = _sunBaseClientSize;
                MinimumSize = new Size(900, 580);
            }
        }

        private void SunshineTargetChanged(object sender, EventArgs e)
        {
            if (_sunLoadingUi || _sunTarget.SelectedIndex < 0)
                return;
            TrySaveSunshineUi(false);
            _sunUiTargetIndex = _sunTarget.SelectedIndex;
            LoadSunshineTargetUi(_sunUiTargetIndex);
            UpdateSunshineCountText();
        }

        private void LoadSunshineTargetUi(int index)
        {
            _sunLoadingUi = true;
            _sunUiTargetIndex = index;
            _sunAll.Checked = _sunModes[index] == 0;
            _sunIndividual.Checked = _sunModes[index] == 1;
            _sunGroup.Checked = _sunModes[index] == 2;
            _sunIndividualText.Text = MaskToSunshineText(_sunIndividualMasks[index], _sunLedCounts[index]);
            UpdateSunshineLedSelector();
            for (int group = 0; group < 4; group++)
                _sunGroupChecks[group].Checked = (_sunSelectedGroups[index] & (1 << group)) != 0;
            sunOspCurrentModeGroupBox.Visible = index == 0;
            _sunTuning.Visible = index == 0;
            _sunTemperature.Visible = index == 0;
            _sunHint.Visible = index != 0;
            _sunDay.Checked = _sunOspCurrentMode == SUN_OSP_CURRENT_DAY;
            _sunNight.Checked = _sunOspCurrentMode == SUN_OSP_CURRENT_NIGHT;
            _sunTuning.Checked = _sunTuningEnabled;
            _sunTemperature.Checked = _sunTemperatureEnabled;
            UpdateSunshineReadModeHeaders();
            UpdateSunshineGroupBar();
            _sunLoadingUi = false;
        }

        private void UpdateSunshineLedSelector()
        {
            if (_sunLedSelectors == null || sunLedSelectionGroupBox == null)
                return;

            int maximum = _sunLedCounts[_sunUiTargetIndex];
            bool designVisible = System.ComponentModel.LicenseManager.UsageMode == System.ComponentModel.LicenseUsageMode.Designtime;
            sunLedSelectionGroupBox.Text = _sunUiTargetIndex == 0
                ? "[ OSP 개별 LED 선택 - 인식 최대 " + maximum + "개 ]"
                : "[ ISE 개별 LED 선택 - 인식 최대 " + maximum + "개 ]";

            for (int index = 0; index < _sunLedSelectors.Length; index++)
            {
                CheckBox selector = _sunLedSelectors[index];
                int led = index + 1;
                bool available = led <= maximum;
                bool selected = available && ((_sunIndividualMasks[_sunUiTargetIndex][index / 32] & (1U << (index % 32))) != 0);
                selector.Visible = designVisible || available;
                selector.Enabled = available;
                selector.Checked = selected;
                selector.BackColor = selected ? Color.FromArgb(45, 125, 190) : Color.FromArgb(45, 50, 65);
            }
        }

        private void SunshineIndividualTextChanged(object sender, EventArgs e)
        {
            if (_sunLoadingUi)
                return;

            uint[] masks;
            if (TryParseSunshineLedText(_sunIndividualText.Text, _sunLedCounts[_sunUiTargetIndex], out masks))
            {
                Array.Copy(masks, _sunIndividualMasks[_sunUiTargetIndex], 3);
                UpdateSunshineLedSelector();
            }
            _sunSelectionDirty[_sunUiTargetIndex] = true;
            _sunIndividualMaskDirty[_sunUiTargetIndex] = true;
        }

        private void SunshineLedSelector_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left)
                return;

            CheckBox selector = sender as CheckBox;
            if (selector == null || !selector.Enabled)
                return;

            _sunIndividual.Checked = true;
            _sunLedSelectorDragging = true;
            _sunLedSelectorDragState = !selector.Checked;
            ApplySunshineLedSelectorState(selector, _sunLedSelectorDragState);
            selector.Capture = false;
        }

        private void SunshineLedSelector_MouseEnter(object sender, EventArgs e)
        {
            if (!_sunLedSelectorDragging)
                return;
            if ((Control.MouseButtons & MouseButtons.Left) == 0)
            {
                _sunLedSelectorDragging = false;
                return;
            }

            CheckBox selector = sender as CheckBox;
            if (selector != null && selector.Enabled)
                ApplySunshineLedSelectorState(selector, _sunLedSelectorDragState);
        }

        private void SunshineLedSelector_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
                _sunLedSelectorDragging = false;
        }

        private void ApplySunshineLedSelectorState(CheckBox selector, bool selected)
        {
            int led = Convert.ToInt32(selector.Tag);
            int maximum = _sunLedCounts[_sunUiTargetIndex];
            if (led < 1 || led > maximum)
                return;

            int segment = (led - 1) / 32;
            int bit = (led - 1) % 32;
            selector.Checked = selected;
            selector.BackColor = selected ? Color.FromArgb(45, 125, 190) : Color.FromArgb(45, 50, 65);
            if (selected)
                _sunIndividualMasks[_sunUiTargetIndex][segment] |= 1U << bit;
            else
                _sunIndividualMasks[_sunUiTargetIndex][segment] &= ~(1U << bit);
            SyncSunshineLedSelectorText();
            _sunSelectionDirty[_sunUiTargetIndex] = true;
            _sunIndividualMaskDirty[_sunUiTargetIndex] = true;
        }

        private void SyncSunshineLedSelectorText()
        {
            bool loading = _sunLoadingUi;
            _sunLoadingUi = true;
            _sunIndividualText.Text = MaskToSunshineText(_sunIndividualMasks[_sunUiTargetIndex], _sunLedCounts[_sunUiTargetIndex]);
            _sunLoadingUi = loading;
        }

        private void SunshineLedSelectAll_Click(object sender, EventArgs e)
        {
            SetSunshineLedSelectorRange(1);
        }

        private void SunshineLedSelectClear_Click(object sender, EventArgs e)
        {
            SetSunshineLedSelectorRange(0);
        }

        private void SunshineLedSelectInvert_Click(object sender, EventArgs e)
        {
            SetSunshineLedSelectorRange(2);
        }

        private void SetSunshineLedSelectorRange(int mode)
        {
            int maximum = _sunLedCounts[_sunUiTargetIndex];
            _sunIndividual.Checked = true;
            for (int led = 1; led <= maximum; led++)
            {
                int segment = (led - 1) / 32;
                uint bit = 1U << ((led - 1) % 32);
                if (mode == 0)
                    _sunIndividualMasks[_sunUiTargetIndex][segment] &= ~bit;
                else if (mode == 1)
                    _sunIndividualMasks[_sunUiTargetIndex][segment] |= bit;
                else
                    _sunIndividualMasks[_sunUiTargetIndex][segment] ^= bit;
            }
            SyncSunshineLedSelectorText();
            UpdateSunshineLedSelector();
            _sunSelectionDirty[_sunUiTargetIndex] = true;
            _sunIndividualMaskDirty[_sunUiTargetIndex] = true;
        }

        private void SunshineGroupBar_Resize(object sender, EventArgs e)
        {
            UpdateSunshineGroupBar();
        }

        private void SunshineGroupPointer_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left)
                return;
            Label pointer = sender as Label;
            if (pointer == null)
                return;
            _sunDragBoundary = Convert.ToInt32(pointer.Tag);
            pointer.Capture = true;
        }

        private void SunshineGroupPointer_MouseMove(object sender, MouseEventArgs e)
        {
            if (_sunDragBoundary < 0 || e.Button != MouseButtons.Left)
                return;

            Label pointer = sender as Label;
            if (pointer == null)
                return;
            Point barPoint = _sunGroupBar.PointToClient(pointer.PointToScreen(e.Location));
            int maximum = Math.Max(4, _sunLedCounts[_sunUiTargetIndex]);
            int value = (int)Math.Round((double)barPoint.X * maximum / Math.Max(1, _sunGroupBar.ClientSize.Width));
            int[] boundaries = _sunUiTargetIndex == 0 ? _sunFioBoundaries : _sunIseBoundaries;
            int minimum = _sunDragBoundary == 0 ? 1 : boundaries[_sunDragBoundary - 1] + 1;
            int limit = _sunDragBoundary == 2 ? maximum - 1 : boundaries[_sunDragBoundary + 1] - 1;
            value = Math.Max(minimum, Math.Min(limit, value));
            if (boundaries[_sunDragBoundary] == value)
                return;

            boundaries[_sunDragBoundary] = value;
            _sunSelectionDirty[_sunUiTargetIndex] = true;
            _sunGroupMaskDirty[_sunUiTargetIndex] = true;
            UpdateSunshineGroupBar();
        }

        private void SunshineGroupPointer_MouseUp(object sender, MouseEventArgs e)
        {
            Label pointer = sender as Label;
            _sunDragBoundary = -1;
            if (pointer != null)
                pointer.Capture = false;
        }

        private void SunshineGroupPointer_MouseCaptureChanged(object sender, EventArgs e)
        {
            Label pointer = sender as Label;
            if (pointer == null || !pointer.Capture)
                _sunDragBoundary = -1;
        }

        private void UpdateSunshineGroupBar()
        {
            if (_sunGroupBar == null || _sunGroupSegments == null || _sunGroupPointers == null)
                return;

            int maximum = Math.Max(4, _sunLedCounts[_sunUiTargetIndex]);
            int[] boundaries = _sunUiTargetIndex == 0 ? _sunFioBoundaries : _sunIseBoundaries;
            int width = Math.Max(1, _sunGroupBar.ClientSize.Width);
            int height = Math.Max(1, _sunGroupBar.ClientSize.Height);
            int start = 0;
            for (int group = 0; group < 4; group++)
            {
                int end = group < 3
                    ? (int)Math.Round((double)boundaries[group] * width / maximum)
                    : width;
                _sunGroupSegments[group].Location = new Point(start, 0);
                _sunGroupSegments[group].Size = new Size(Math.Max(1, end - start), height);
                start = end;
            }

            for (int index = 0; index < 3; index++)
            {
                int x = _sunGroupBar.Left + (int)Math.Round((double)boundaries[index] * width / maximum);
                _sunGroupPointers[index].Left = x - (_sunGroupPointers[index].Width / 2);
                _sunGroupBoundaryLabels[index].Left = x - (_sunGroupBoundaryLabels[index].Width / 2);
                _sunGroupBoundaryLabels[index].Text = boundaries[index].ToString();
            }
            sunGroupEndLabel.Text = "End " + _sunLedCounts[_sunUiTargetIndex];
        }

        private void SunshineSelectionChanged(object sender, EventArgs e)
        {
            if (_sunLoadingUi)
                return;
            _sunSelectionDirty[_sunUiTargetIndex] = true;
        }

        private void SunshineTuningChanged(object sender, EventArgs e)
        {
            if (_sunLoadingUi)
                return;
            _sunTuningEnabled = _sunTuning.Checked;
            _sunTuningDirty = true;
        }

        private void SunshineTemperatureChanged(object sender, EventArgs e)
        {
            if (_sunLoadingUi)
                return;
            _sunTemperatureEnabled = _sunTemperature.Checked;
            _sunTemperatureDirty = true;
        }

        private void SunshineOspCurrentModeChanged(object sender, EventArgs e)
        {
            byte mode;

            if (_sunLoadingUi || (!_sunDay.Checked && !_sunNight.Checked))
                return;
            mode = _sunNight.Checked ? SUN_OSP_CURRENT_NIGHT : SUN_OSP_CURRENT_DAY;
            if (_sunOspCurrentMode == mode)
                return;
            _sunOspCurrentMode = mode;
            _sunOspCurrentModeDirty = true;
            UpdateSunshineReadModeHeaders();
            if (_sunGrid != null)
                _sunGrid.Rows.Clear();
            if (_sunStatus != null)
                _sunStatus.Text = "OSP " + SunshineOspCurrentModeText() + " 선택됨 - 설정 적용 또는 1회 TX가 필요합니다.";
        }

        private string SunshineOspCurrentModeText()
        {
            return _sunOspCurrentMode == SUN_OSP_CURRENT_NIGHT ? "야간" : "주간";
        }

        private void UpdateSunshineReadModeHeaders()
        {
            string mode = SunshineOspCurrentModeText();

            if (TuneR == null || TuneG == null || TuneB == null)
                return;
            TuneR.HeaderText = "Tune R\n" + mode;
            TuneG.HeaderText = "Tune G\n" + mode;
            TuneB.HeaderText = "Tune B\n" + mode;
        }

        private bool TrySaveSunshineUi(bool showError)
        {
            int index = _sunUiTargetIndex;
            uint[] masks;
            int maximum = _sunLedCounts[index];
            if (!TryParseSunshineLedText(_sunIndividualText.Text, maximum, out masks))
            {
                if (showError)
                    MessageBox.Show("개별 LED 번호를 1~" + maximum + " 범위에서 쉼표(,)로 구분해 입력하세요.", "Sunshine LED 선택");
                return false;
            }
            _sunModes[index] = _sunIndividual.Checked ? (byte)1 : (_sunGroup.Checked ? (byte)2 : (byte)0);
            Array.Copy(masks, _sunIndividualMasks[index], 3);
            UpdateSunshineLedSelector();
            byte selected = 0;
            for (int group = 0; group < 4; group++)
                if (_sunGroupChecks[group].Checked)
                    selected |= (byte)(1 << group);
            _sunSelectedGroups[index] = selected;
            return true;
        }

        private static bool TryParseSunshineLedText(string text, int maximum, out uint[] masks)
        {
            masks = new uint[3];
            if (string.IsNullOrWhiteSpace(text))
                return true;
            string[] values = text.Split(new[] { ',', ' ', ';' }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string value in values)
            {
                int led;
                if (!int.TryParse(value.Trim(), out led) || led < 1 || led > maximum)
                    return false;
                int index = (led - 1) / 32;
                int bit = (led - 1) % 32;
                masks[index] |= 1U << bit;
            }
            return true;
        }

        private static string MaskToSunshineText(uint[] masks, int maximum)
        {
            List<int> leds = new List<int>();
            for (int led = 1; led <= maximum; led++)
                if ((masks[(led - 1) / 32] & (1U << ((led - 1) % 32))) != 0)
                    leds.Add(led);
            return string.Join(",", leds);
        }

        private void SunshineApply_Click(object sender, EventArgs e)
        {
            if (!TrySaveSunshineUi(true))
                return;
            byte targetMask = (byte)(1 << _sunUiTargetIndex);
            BeginSunshineCountSync(targetMask, () => BeginSunshineSelectionApply(targetMask, null));
        }

        private bool TryBeginSunshineSelectionBeforeTx(bool advanceRgbwShift)
        {
            if (!_ad3306CommandMode || sunshinePanel == null || !sunshinePanel.Visible)
                return false;
            if (_sunConfigTimer.Enabled)
            {
                _sunConfigCompleted = () => DoTxImmediate(advanceRgbwShift);
                _sunStatus.Text = "선택 설정 적용 후 1회 TX 예정";
                return true;
            }
            if (!TrySaveSunshineUi(true))
                return true;

            AD3306LedCommand command = BuildAD3306Command();
            byte target = (byte)((command.TargetAndFid >> 4) & 0x03);
            byte dirtyTargets = 0;
            if ((target & SUN_TARGET_FIO) != 0 && IsSunshineSelectionDirty(0))
                dirtyTargets |= 0x01;
            if ((target & SUN_TARGET_ISELED) != 0 && IsSunshineSelectionDirty(1))
                dirtyTargets |= 0x02;
            if ((target & SUN_TARGET_FIO) != 0 && _sunOspCurrentModeDirty)
                dirtyTargets |= SUN_TARGET_FIO;
            if (dirtyTargets == 0 && !_sunTuningDirty && !_sunTemperatureDirty)
                return false;

            if (dirtyTargets != 0)
            {
                BeginSunshineCountSync(dirtyTargets, () => BeginSunshineSelectionApply(dirtyTargets, () => DoTxImmediate(advanceRgbwShift)));
                return true;
            }
            BeginSunshineSelectionApply(dirtyTargets, () => DoTxImmediate(advanceRgbwShift));
            return true;
        }

        private bool IsSunshineSelectionDirty(int index)
        {
            return _sunSelectionDirty[index] || _sunIndividualMaskDirty[index] || _sunGroupMaskDirty[index];
        }

        private void CancelSunshineDeferredImmediateTx()
        {
            /* If Stop is pressed while the initial selection packets are still
             * being applied, do not let their completion callback restart the
             * Shift immediately after STOP. */
            _sunConfigCompleted = null;
        }

        private void BeginSunshineSelectionApply(byte targetMask, Action completed)
        {
            if (_gw == null || !_gw.IsConnected)
            {
                _sunStatus.Text = "연결된 Sunshine 통신이 없습니다.";
                return;
            }
            _sunConfigPackets.Clear();
            _sunConfigTargetMask = 0;
            _sunConfigIndividualMask = 0;
            _sunConfigGroupMask = 0;
            _sunConfigCompleted = completed;
            _sunConfigFailed = false;
            _sunConfigOspCurrentModeIncluded = false;
            for (int index = 0; index < 2; index++)
            {
                if ((targetMask & (1 << index)) == 0)
                    continue;
                byte target = index == 0 ? SUN_TARGET_FIO : SUN_TARGET_ISELED;
                if (_sunIndividualMaskDirty[index])
                {
                    for (byte segment = 0; segment < 3; segment++)
                        _sunConfigPackets.Enqueue(BuildSunshinePacket(SUN_OP_INDIVIDUAL_MASK, target, segment, _sunIndividualMasks[index][segment]));
                    _sunConfigIndividualMask |= (byte)(1 << index);
                }

                if (_sunGroupMaskDirty[index])
                {
                    int[] boundaries = index == 0 ? _sunFioBoundaries : _sunIseBoundaries;
                    uint value = (uint)(boundaries[1] | (boundaries[2] << 8) | (_sunSelectedGroups[index] << 16) | (_sunModes[index] << 24));
                    _sunConfigPackets.Enqueue(BuildSunshinePacket(SUN_OP_GROUP_BOUNDARIES, target, (byte)boundaries[0], value));
                    _sunConfigGroupMask |= (byte)(1 << index);
                    if (_sunSelectionDirty[index])
                        _sunConfigTargetMask |= (byte)(1 << index);
                }
                if (_sunSelectionDirty[index] && !_sunGroupMaskDirty[index])
                {
                    _sunConfigPackets.Enqueue(BuildSunshinePacket(SUN_OP_SELECT_MODE, target, _sunModes[index], _sunSelectedGroups[index]));
                    _sunConfigTargetMask |= (byte)(1 << index);
                }
            }
            if (((targetMask & SUN_TARGET_FIO) != 0) && _sunOspCurrentModeDirty)
            {
                _sunConfigPackets.Enqueue(BuildSunshinePacket(SUN_OP_OSP_CURRENT_MODE, _sunOspCurrentMode, 0, 0));
                _sunConfigOspCurrentModeIncluded = true;
            }
            if (_sunTuningDirty)
                _sunConfigPackets.Enqueue(BuildSunshinePacket(SUN_OP_TUNING_ENABLE, _sunTuningEnabled ? (byte)1 : (byte)0, 0, 0));
            if (_sunTemperatureDirty)
                _sunConfigPackets.Enqueue(BuildSunshinePacket(SUN_OP_TEMPERATURE_ENABLE, _sunTemperatureEnabled ? (byte)1 : (byte)0, 0, 0));
            _sunStatus.Text = "선택 설정 전송 중...";
            _sunApply.Enabled = false;
            _sunConfigTimer.Start();
            SunshineConfigTimer_Tick(null, EventArgs.Empty);
        }

        private void SunshineConfigTimer_Tick(object sender, EventArgs e)
        {
            if (_sunConfigPackets.Count != 0)
            {
                byte[] packet = _sunConfigPackets.Dequeue();
                if (!SendSunshinePublisher(packet))
                {
                    _sunConfigFailed = true;
                    _sunConfigPackets.Clear();
                }
                if (_sunConfigPackets.Count != 0)
                    return;
            }

            _sunConfigTimer.Stop();
            _sunApply.Enabled = true;
            if (_sunConfigFailed)
            {
                _sunStatus.Text = "선택 설정 전송 실패";
                return;
            }
            for (int index = 0; index < 2; index++)
            {
                if ((_sunConfigTargetMask & (1 << index)) != 0)
                    _sunSelectionDirty[index] = false;
                if ((_sunConfigIndividualMask & (1 << index)) != 0)
                    _sunIndividualMaskDirty[index] = false;
                if ((_sunConfigGroupMask & (1 << index)) != 0)
                    _sunGroupMaskDirty[index] = false;
            }
            if (_sunConfigOspCurrentModeIncluded)
                _sunOspCurrentModeDirty = false;
            _sunTuningDirty = false;
            _sunTemperatureDirty = false;
            _sunStatus.Text = "선택 설정 적용 완료 / OSP " + SunshineOspCurrentModeText() +
                              " / OTP " + (_sunTuningEnabled ? "ON" : "OFF") +
                              " / 온도 " + (_sunTemperatureEnabled ? "ON" : "OFF");
            Action completed = _sunConfigCompleted;
            _sunConfigCompleted = null;
            if (completed != null)
                completed();
        }

        private byte[] BuildSunshinePacket(byte opcode, byte parameter2, byte parameter3, uint value)
        {
            return BuildSunshinePacketForFid(GetFID(), opcode, parameter2, parameter3, value);
        }

        private static byte[] BuildSunshinePacketForFid(byte fid, byte opcode, byte parameter2, byte parameter3, uint value)
        {
            byte[] packet = new byte[8];
            packet[0] = (byte)(SUN_EXT_REQUEST | (fid & 0x0F));
            packet[1] = opcode;
            packet[2] = parameter2;
            packet[3] = parameter3;
            packet[4] = (byte)value;
            packet[5] = (byte)(value >> 8);
            packet[6] = (byte)(value >> 16);
            packet[7] = (byte)(value >> 24);
            return packet;
        }

        private byte[] BuildSunshineReadPacketForFid(byte fid, byte opcode,
            byte parameter2, byte parameter3, uint value)
        {
            byte[] packet = BuildSunshinePacketForFid(fid, opcode,
                parameter2, parameter3, value);
            AD3306LedBackend backend = _backendSelection;
            if (backend == AD3306LedBackend.Auto)
                backend = AD3306LedBackend.Adi;

            // Read requests do not otherwise use byte 7.  Sending the backend
            // explicitly prevents a previous ADI/Microchip TX selection from
            // being reused by a later Read operation.  Both requests keep the
            // combined mask so LAN8661 virtual FID 1 and every active AD3304
            // FID are returned in the same Read result.
            packet[7] = (byte)backend;
            return packet;
        }

        private byte[] BuildSunshineReadPacket(byte opcode, byte parameter2,
            byte parameter3, uint value)
        {
            return BuildSunshineReadPacketForFid(GetFID(), opcode,
                parameter2, parameter3, value);
        }

        private bool SendSunshinePublisher(byte[] packet)
        {
            if (_gw == null || !_gw.IsConnected)
                return false;
            if (packet[1] == SUN_OP_LIN_FAST_CONTROL)
            {
                _sunLinFastControlSequence++;
                packet[7] = _sunLinFastControlSequence;
            }
            LINFrame frame = new LINFrame
            {
                FrameID = AD3306LedCommand.FrameId,
                DataLength = 8,
                ChecksumType = LINChecksumType.Enhanced
            };
            Array.Copy(packet, frame.Data, 8);
            return _gw.SendPublisher(frame);
        }

        private bool QueueSunshineLinFastPublisher(byte[] packet)
        {
            if (packet == null || _gw == null || !_gw.IsConnected)
                return false;
            lock (_sunLinFastPublisherSync)
            {
                if (_sunLinFastPublisherQueue.Count == 0)
                    _sunLinFastPublisherDueAt = DateTime.UtcNow.AddMilliseconds(SUN_LIN_FAST_CONTROL_DELAY_MS);
                _sunLinFastPublisherQueue.Enqueue(packet);
            }
            return true;
        }

        private bool TrySendSunshineLinFastPublisher()
        {
            byte[] packet;
            lock (_sunLinFastPublisherSync)
            {
                if (_sunLinFastPublisherQueue.Count == 0)
                    return false;
                if (DateTime.UtcNow < _sunLinFastPublisherDueAt)
                    return true;
                packet = _sunLinFastPublisherQueue.Dequeue();
                if (_sunLinFastPublisherQueue.Count != 0)
                    _sunLinFastPublisherDueAt = DateTime.UtcNow.AddMilliseconds(SUN_LIN_FAST_CONTROL_DELAY_MS);
            }
            if (!SendSunshineLinFastPublisherNow(packet))
                BeginInvoke(new Action(() => AbortSunshineRead("LIN 제어 요청 전송 실패")));
            _sunLinFastNextSubscriberAt = DateTime.UtcNow.AddMilliseconds(SUN_LIN_FAST_SUBSCRIBER_DELAY_MS);
            return true;
        }

        private bool StartSunshineLinSchedule()
        {
            PeakLINGateway peak = _gw as PeakLINGateway;
            if (peak == null)
                return false;
            _sunLinHardwareSchedule = peak.StartSubscriberSchedule(SUN_EXT_LIN_RESPONSE_ID, 8, LINChecksumType.Enhanced, SUN_LIN_FAST_SCHEDULE_PERIOD_MS);
            return _sunLinHardwareSchedule;
        }

        private void StopSunshineLinSchedule()
        {
            PeakLINGateway peak = _gw as PeakLINGateway;
            if (peak != null)
                peak.StopSubscriberSchedule();
            _sunLinHardwareSchedule = false;
        }

        private bool SendSunshineLinFastPublisherNow(byte[] packet)
        {
            PeakLINGateway peak = _gw as PeakLINGateway;
            bool resumeSchedule = _sunLinHardwareSchedule && peak != null;
            if (resumeSchedule && !peak.SuspendSubscriberSchedule())
            {
                peak.StopSubscriberSchedule();
                _sunLinHardwareSchedule = false;
                resumeSchedule = false;
            }
            bool sent = SendSunshinePublisher(packet);
            if (sent && resumeSchedule)
                System.Threading.Thread.Sleep(SUN_LIN_FAST_SCHEDULE_PERIOD_MS);
            if (resumeSchedule && !peak.ResumeSubscriberSchedule())
            {
                peak.StopSubscriberSchedule();
                _sunLinHardwareSchedule = false;
                _sunLinFastNextSubscriberAt = DateTime.UtcNow;
            }
            return sent;
        }

        private void BeginSunshineCountSync(byte targetMask, Action completed)
        {
            if (_gw == null || !_gw.IsConnected || _sunReadActive)
            {
                _sunStatus.Text = "LED 수량을 확인할 수 없습니다.";
                return;
            }
            byte unknownTargetMask = 0;
            for (int index = 0; index < 2; index++)
                if ((targetMask & (1 << index)) != 0 && !_sunLedCountKnown[index])
                    unknownTargetMask |= (byte)(1 << index);
            if (unknownTargetMask == 0)
            {
                completed();
                return;
            }

            _sunReadRequests.Clear();
            _sunReadCurrent = null;
            _sunReadTotal = 0;
            _sunReadCompleted = 0;
            _sunReadActive = true;
            AD3306LedCommand.ServiceReadActive = true;
            _sunReadStartedAt = DateTime.UtcNow;
            _sunReadShowGrid = false;
            _sunReadFinished = completed;
            _sunRead.Enabled = false;
            _sunStatus.Text = "초기화된 LED 수량 확인 중...";
            for (int index = 0; index < 2; index++)
            {
                if ((unknownTargetMask & (1 << index)) == 0)
                    continue;
                int capturedIndex = index;
                byte target = index == 0 ? SUN_TARGET_FIO : SUN_TARGET_ISELED;
                EnqueueSunshineRead(BuildSunshineReadPacket(SUN_OP_SELECTION_READ, target, 0, 0), response =>
                {
                    ApplySunshineLedCount(capturedIndex, response[6]);
                    _sunLedCountKnown[capturedIndex] = true;
                });
            }
            SendNextSunshineRead();
        }

        private void SunshineRead_Click(object sender, EventArgs e)
        {
            if (_sunGrid == null || _sunTarget == null || _sunRead == null || _sunStatus == null)
            {
                MessageBox.Show("Sunshine Read 화면 초기화에 실패했습니다. CIE 화면을 다시 열어 주세요.", "Sunshine Read");
                return;
            }
            if (_gw == null || !_gw.IsConnected)
            {
                MessageBox.Show("먼저 LIN/CAN/USB를 연결하세요.", "Sunshine Read");
                return;
            }
            if (_sunReadActive)
                return;

            // Read owns the USB/LIN response stream until all bulk pages have
            // arrived.  A CIE periodic sender can otherwise keep inserting LED
            // command/ACK traffic and overflow the small MCU response queue.
            // Stop both PC periodic TX and an autonomous MCU effect before the
            // Read request.  STOP preserves the current LED output.
            CancelSunshineDeferredImmediateTx();
            if (txTimer.Enabled)
            {
                txTimer.Stop();
                SetAdiLinRunningState(false);
            }
            if (_ad3306CommandMode)
            {
                SendAdiLinStop();
                SetAdiImmediateEffectState(false);
            }
            if ((_sunTarget.SelectedIndex == 0) && _sunOspCurrentModeDirty)
            {
                if (!TrySaveSunshineUi(true))
                    return;
                _sunStatus.Text = "OSP " + SunshineOspCurrentModeText() + " 모드 적용 후 Read 예정";
                BeginSunshineSelectionApply(SUN_TARGET_FIO, () => SunshineRead_Click(sender, e));
                return;
            }

            _sunGrid.Rows.Clear();
            _sunReadRequests.Clear();
            _sunReadTotal = 0;
            _sunReadCompleted = 0;
            _sunReadTargetIndex = _sunTarget.SelectedIndex;
            _sunRequestedFid = GetFID();
            _sunReadFid = _sunRequestedFid;
            _sunReadMaximumLedCount = 0;
            _sunReadTotalLeds = 0;
            _sunPendingFidReads.Clear();
            _sunOspRecordsByFid.Clear();
            _sunIseRecordsByFid.Clear();
            _sunBulkReadActive = false;
            _sunBulkProtocolVersion = 0;
            _sunBulkActiveMask = 0;
            _sunBulkCompletedMask = 0;
            _sunBulkError = 0;
            _sunBulkRemoteCode = 0;
            _sunBulkPages.Clear();
            _sunBulkRequestPacket = null;
            _sunBulkStartRetry = 0;
            _sunBulkStarted = false;
            ResetSunshineLinFastState();
            _sunLinFastFailedFids.Clear();
            _sunReadActive = true;
            AD3306LedCommand.ServiceReadActive = true;
            _sunReadStartedAt = DateTime.UtcNow;
            _sunReadShowGrid = true;
            _sunReadFinished = null;
            _sunRead.Enabled = false;
            StartSunshineUsbBulkRead();
        }

        private byte SunshineReadScanFlags()
        {
            if (_sunReadTargetIndex != 0)
                return 0x0C;

            // OSP OTP/Tune is static and is already cached during initialization.
            // Request live PWM/temperature/diagnostics here; the MCU automatically
            // refreshes OTP only when the selected FID has no valid Tune cache.
            return 0x0A;
        }

        private void StartSunshineUsbBulkRead()
        {
            byte target = _sunReadTargetIndex == 0 ? SUN_TARGET_FIO : SUN_TARGET_ISELED;
            byte scanFlags = SunshineReadScanFlags();
            string transport = SunshineReadTransportName();

            _sunBulkReadActive = true;
            _sunLinFastMode = IsSunshineLinBulkGateway();
            _sunBulkRequestPacket = BuildSunshineReadPacketForFid(_sunRequestedFid,
                _sunLinFastMode ? SUN_OP_LIN_FAST_BULK_READ : SUN_OP_USB_BULK_READ,
                target, scanFlags, 0);
            _sunBulkLastRequestAt = DateTime.UtcNow;
            _sunStatus.Text = _sunRequestedFid == 0
                ? transport + " 전체 FID Read 요청..."
                : string.Format("{0} FID {1} Read 요청...", transport, _sunRequestedFid);
            if (!SendSunshinePublisher(_sunBulkRequestPacket))
            {
                _sunBulkReadActive = false;
                AbortSunshineRead(transport + " 전체 Read 요청 전송 실패");
                return;
            }
            if (_sunLinFastMode)
            {
                if (!StartSunshineLinSchedule())
                    _sunLinFastNextSubscriberAt = DateTime.UtcNow.AddMilliseconds(SUN_LIN_FAST_SUBSCRIBER_DELAY_MS);
            }
            if (IsSunshineLinBulkGateway())
            {
                _sunReadTimer.Stop();
                ChangeSunshineUsbTimer(1, _sunLinFastMode ? 1 : 8);
            }
            else
            {
                _sunReadTimer.Start();
            }
        }

        private void ResetSunshineLinFastState()
        {
            StopSunshineLinSchedule();
            _sunLinFastMode = false;
            _sunLinFastFid = 0;
            _sunLinFastLedCount = 0;
            _sunLinFastChunkCount = 0;
            _sunLinFastData = null;
            _sunLinFastReceived = null;
            _sunLinFastExpectedCrc = 0;
            _sunLinFastMissingChunks.Clear();
            _sunLinFastRetryWaiting = false;
            _sunLinFastRetryChunk = 0;
            _sunLinFastRetrySentAt = DateTime.MinValue;
            _sunLinFastRetryCount = 0;
            _sunLinFastCrcRetryCount = 0;
            _sunLinFastLastFrameAt = DateTime.MinValue;
            _sunLinFastStatusSentAt = DateTime.MinValue;
            _sunLinFastStatusRetryCount = 0;
            _sunLinFastNextSubscriberAt = DateTime.MinValue;
            _sunLinFastSubscriberPending = false;
            _sunLinFastSubscriberSentAt = DateTime.MinValue;
            _sunLinFastPendingFidAck = false;
            _sunLinFastPendingFid = 0;
            _sunLinFastPendingFidSentAt = DateTime.MinValue;
            _sunLinFastPendingFidRetryCount = 0;
            _sunLinFastControlSequence = 0;
            lock (_sunLinFastPublisherSync)
            {
                _sunLinFastPublisherQueue.Clear();
                _sunLinFastPublisherDueAt = DateTime.MinValue;
            }
        }

        private bool HandleSunshineLinFastFrame(LINFrame frame)
        {
            if (frame.Direction != LINDirection.RX || !frame.IsValid || frame.Data == null || frame.DataLength < 8)
                return true;

            byte[] response = frame.Data;
            _sunLinFastSubscriberPending = false;
            _sunLinFastNextSubscriberAt = DateTime.UtcNow.AddMilliseconds(SUN_LIN_FAST_SUBSCRIBER_DELAY_MS);
            if (response.All(value => value == 0xFF))
                return true;

            _sunBulkStarted = true;
            _sunLinFastLastFrameAt = DateTime.UtcNow;

            switch (response[0])
            {
                case SUN_LIN_FAST_START:
                    StartSunshineLinFastRead(response);
                    break;
                case SUN_LIN_FAST_FID_BEGIN:
                    _sunLinFastPendingFidAck = false;
                    _sunLinFastPendingFidRetryCount = 0;
                    StartSunshineLinFastFid(response);
                    break;
                case SUN_LIN_FAST_FID_END:
                    _sunLinFastStatusRetryCount = 0;
                    EndSunshineLinFastFid(response);
                    break;
                case SUN_LIN_FAST_COMPLETE:
                    CompleteSunshineLinFastRead(response);
                    break;
                case SUN_LIN_FAST_ERROR:
                    AbortSunshineRead(string.Format("LIN 고속 Read 실패: {0}", SunshineUsbBulkErrorText(response[1])));
                    break;
                default:
                    StoreSunshineLinFastData(response);
                    break;
            }

            return true;
        }

        private void StartSunshineLinFastRead(byte[] response)
        {
            byte target = _sunReadTargetIndex == 0 ? SUN_TARGET_FIO : SUN_TARGET_ISELED;
            if (response[1] != SUN_OP_LIN_FAST_BULK_READ || response[2] != target || response[5] != SUN_LIN_FAST_PROTOCOL_VERSION || response[6] != SUN_LIN_FAST_RECORD_SIZE || response[7] != SUN_LIN_FAST_PAYLOAD_SIZE)
            {
                AbortSunshineRead("LIN 고속 Read 시작 정보가 올바르지 않습니다.");
                return;
            }

            _sunBulkStarted = true;
            _sunBulkProtocolVersion = response[5];
            _sunBulkActiveMask = response[4];
            _sunStatus.Text = string.Format("LIN 고속 Read 시작... Active FID Mask=0x{0:X2} / Protocol V{1}", _sunBulkActiveMask, _sunBulkProtocolVersion);
        }

        private void CompleteSunshineLinFastRead(byte[] response)
        {
            if (_sunLinFastFid != 0)
            {
                AbortSunshineRead("LIN 고속 Read FID 확인 전에 완료 응답을 받았습니다.");
                return;
            }

            _sunBulkCompletedMask = response[4];
            _sunBulkError |= response[5];
            ApplySunshineLedCount(_sunReadTargetIndex, _sunReadMaximumLedCount);
            byte readError = _sunBulkError;
            FinishSunshineRead();
            if (readError != 0)
                _sunStatus.Text += string.Format(" / 진단 상태 0x{0:X2}", readError);
        }

        private void StoreSunshineLinFastData(byte[] response)
        {
            int chunk = response[0];
            if (_sunLinFastFid == 0 || chunk >= _sunLinFastChunkCount || _sunLinFastData == null || _sunLinFastReceived == null)
                return;

            int offset = chunk * SUN_LIN_FAST_PAYLOAD_SIZE;
            int copyLength = Math.Min(SUN_LIN_FAST_PAYLOAD_SIZE, _sunLinFastData.Length - offset);
            if (copyLength <= 0)
                return;

            byte payloadXor = (_sunLinFastRetryWaiting && chunk == _sunLinFastRetryChunk) ? SUN_LIN_FAST_RETRY_XOR : SUN_LIN_FAST_PAYLOAD_XOR;
            for (int index = 0; index < copyLength; index++)
                _sunLinFastData[offset + index] = (byte)(response[index + 1] ^ payloadXor);
            _sunLinFastReceived[chunk] = true;
            if (_sunLinFastRetryWaiting && chunk == _sunLinFastRetryChunk)
            {
                _sunLinFastRetryWaiting = false;
                _sunLinFastRetryCount = 0;
                RequestNextSunshineLinFastChunk();
            }
        }

        private bool InitializeSunshineLinFastFid(byte fid, int ledCount, int chunkCount)
        {
            int maximumLedCount = _sunReadTargetIndex == 0 ? FIO_LED_COUNT : ISE_LED_COUNT;
            int expectedChunks = ((ledCount * SUN_LIN_FAST_RECORD_SIZE) + SUN_LIN_FAST_PAYLOAD_SIZE - 1) / SUN_LIN_FAST_PAYLOAD_SIZE;
            if (fid == 0 || ledCount <= 0 || ledCount > maximumLedCount || chunkCount != expectedChunks)
                return false;
            if (_sunLinFastFid == fid && _sunLinFastData != null && _sunLinFastReceived != null)
                return _sunLinFastLedCount == ledCount && _sunLinFastChunkCount == chunkCount;
            if (_sunLinFastFid != 0)
                return false;

            _sunLinFastFid = fid;
            _sunLinFastLedCount = ledCount;
            _sunLinFastChunkCount = chunkCount;
            _sunLinFastData = new byte[ledCount * SUN_LIN_FAST_RECORD_SIZE];
            _sunLinFastReceived = new bool[chunkCount];
            _sunLinFastExpectedCrc = 0;
            _sunLinFastMissingChunks.Clear();
            _sunLinFastRetryWaiting = false;
            _sunLinFastRetryCount = 0;
            _sunLinFastCrcRetryCount = 0;
            _sunLinFastStatusRetryCount = 0;
            _sunLinFastPendingFidAck = false;
            _sunLinFastPendingFidRetryCount = 0;
            _sunReadFid = fid;
            _sunReadMaximumLedCount = Math.Max(_sunReadMaximumLedCount, ledCount);
            _sunReadTotalLeds += ledCount;
            if (_sunReadTargetIndex == 0)
            {
                _sunOspRecords = Enumerable.Range(0, ledCount).Select(value => new SunshineOspRecord()).ToArray();
                _sunOspRecordsByFid[fid] = _sunOspRecords;
            }
            else
            {
                _sunIseRecords = Enumerable.Range(0, ledCount).Select(value => new SunshineIseRecord()).ToArray();
                _sunIseRecordsByFid[fid] = _sunIseRecords;
            }
            _sunStatus.Text = string.Format("LIN FID {0} 고속 데이터 수신 중... {1}개 / {2}프레임", fid, ledCount, chunkCount);
            return true;
        }

        private void StartSunshineLinFastFid(byte[] response)
        {
            byte target = response[6];
            if (response[3] != SUN_LIN_FAST_RECORD_SIZE || response[7] != SUN_LIN_FAST_PROTOCOL_VERSION || target != (_sunReadTargetIndex == 0 ? SUN_TARGET_FIO : SUN_TARGET_ISELED) || !InitializeSunshineLinFastFid(response[1], response[2], response[4]))
                AbortSunshineRead("LIN 고속 Read FID 시작 정보가 올바르지 않습니다.");
        }

        private void EndSunshineLinFastFid(byte[] response)
        {
            byte fid = response[1];
            if (_sunLinFastPendingFidAck && fid == _sunLinFastPendingFid)
                return;
            if (_sunLinFastFid == 0)
            {
                if (!InitializeSunshineLinFastFid(fid, response[7], response[2]))
                {
                    AbortSunshineRead("LIN 고속 Read FID 수량 정보를 복구할 수 없습니다.");
                    return;
                }
                _sunStatus.Text = string.Format("LIN FID {0} 수량 정보 복구 완료 / {1}개", fid, _sunLinFastLedCount);
            }
            if (fid != _sunLinFastFid)
                return;
            if (response[2] != _sunLinFastChunkCount || response[7] != _sunLinFastLedCount)
            {
                AbortSunshineRead("LIN 고속 Read FID 종료 정보가 일치하지 않습니다.");
                return;
            }
            _sunLinFastExpectedCrc = (ushort)(response[3] | (response[4] << 8));
            _sunBulkError |= response[5];
            _sunBulkCompletedMask = response[6];
            _sunLinFastMissingChunks.Clear();
            for (int index = 0; index < _sunLinFastReceived.Length; index++)
            {
                if (!_sunLinFastReceived[index])
                    _sunLinFastMissingChunks.Enqueue((byte)index);
            }
            RequestNextSunshineLinFastChunk();
        }

        private void RequestNextSunshineLinFastChunk()
        {
            if (_sunLinFastRetryWaiting)
                return;
            if (_sunLinFastMissingChunks.Count == 0)
            {
                CompleteSunshineLinFastFid();
                return;
            }

            _sunLinFastRetryChunk = _sunLinFastMissingChunks.Dequeue();
            byte[] request = BuildSunshinePacketForFid(_sunLinFastFid, SUN_OP_LIN_FAST_CONTROL, SUN_LIN_FAST_RETRY, _sunLinFastRetryChunk, 0);
            if (!QueueSunshineLinFastPublisher(request))
            {
                AbortSunshineRead("LIN 누락 프레임 재요청 전송 실패");
                return;
            }
            _sunLinFastRetryWaiting = true;
            _sunLinFastRetrySentAt = DateTime.UtcNow;
            _sunStatus.Text = string.Format("LIN FID {0} 누락 프레임 {1} 재수신 중...", _sunLinFastFid, _sunLinFastRetryChunk);
        }

        private void CompleteSunshineLinFastFid()
        {
            ushort crc = SunshineLinFastCrc16(_sunLinFastData);
            if (crc != _sunLinFastExpectedCrc)
            {
                RetrySunshineLinFastFid(crc);
                return;
            }
            ParseSunshineLinFastRecords();
            byte completedFid = _sunLinFastFid;
            int completedLeds = _sunLinFastLedCount;
            byte[] next = BuildSunshinePacketForFid(completedFid, SUN_OP_LIN_FAST_CONTROL, SUN_LIN_FAST_NEXT, 0, 0);
            ClearSunshineLinFastFid();
            _sunReadCompleted += completedLeds;
            if (!QueueSunshineLinFastPublisher(next))
            {
                AbortSunshineRead("LIN 다음 FID 요청 전송 실패");
                return;
            }
            _sunLinFastPendingFidAck = true;
            _sunLinFastPendingFid = completedFid;
            _sunLinFastPendingFidSentAt = DateTime.UtcNow;
            _sunLinFastPendingFidRetryCount = 0;
            _sunStatus.Text = string.Format("LIN FID {0} CRC 확인 완료 / {1}개", completedFid, completedLeds);
        }

        private void RetrySunshineLinFastFid(ushort actualCrc)
        {
            if (_sunLinFastCrcRetryCount >= 3)
            {
                SkipSunshineLinFastFid(actualCrc);
                return;
            }
            _sunLinFastCrcRetryCount++;
            _sunLinFastMissingChunks.Clear();
            Array.Clear(_sunLinFastReceived, 0, _sunLinFastReceived.Length);
            for (int index = 0; index < _sunLinFastChunkCount; index++)
                _sunLinFastMissingChunks.Enqueue((byte)index);
            _sunLinFastRetryWaiting = false;
            _sunLinFastRetryCount = 0;
            _sunStatus.Text = string.Format("LIN FID {0} CRC 오류 / 전체 데이터 재수신 {1}/3", _sunLinFastFid, _sunLinFastCrcRetryCount);
            RequestNextSunshineLinFastChunk();
        }

        private void SkipSunshineLinFastFid(ushort actualCrc)
        {
            byte failedFid = _sunLinFastFid;
            ushort expectedCrc = _sunLinFastExpectedCrc;
            byte[] next = BuildSunshinePacketForFid(failedFid, SUN_OP_LIN_FAST_CONTROL, SUN_LIN_FAST_NEXT, 0, 0);
            _sunLinFastFailedFids.Add(failedFid);
            if (_sunReadTargetIndex == 0)
                _sunOspRecordsByFid.Remove(failedFid);
            else
                _sunIseRecordsByFid.Remove(failedFid);
            ClearSunshineLinFastFid();
            if (!QueueSunshineLinFastPublisher(next))
            {
                AbortSunshineRead("LIN CRC 오류 FID 건너뛰기 전송 실패");
                return;
            }
            _sunLinFastPendingFidAck = true;
            _sunLinFastPendingFid = failedFid;
            _sunLinFastPendingFidSentAt = DateTime.UtcNow;
            _sunLinFastPendingFidRetryCount = 0;
            _sunStatus.Text = string.Format("LIN FID {0} CRC 오류 지속(MCU 0x{1:X4} / PC 0x{2:X4}) / 다음 FID 계속", failedFid, expectedCrc, actualCrc);
        }

        private void ClearSunshineLinFastFid()
        {
            _sunLinFastFid = 0;
            _sunLinFastLedCount = 0;
            _sunLinFastChunkCount = 0;
            _sunLinFastData = null;
            _sunLinFastReceived = null;
            _sunLinFastExpectedCrc = 0;
            _sunLinFastMissingChunks.Clear();
            _sunLinFastRetryWaiting = false;
            _sunLinFastRetryCount = 0;
            _sunLinFastCrcRetryCount = 0;
        }

        private void ParseSunshineLinFastRecords()
        {
            _sunReadFid = _sunLinFastFid;
            for (int led = 1; led <= _sunLinFastLedCount; led++)
            {
                int recordOffset = (led - 1) * SUN_LIN_FAST_RECORD_SIZE;
                for (int page = 0; page < 5; page++)
                {
                    byte[] response = new byte[8];
                    Array.Copy(_sunLinFastData, recordOffset + (page * 4), response, 4, 4);
                    if (_sunReadTargetIndex == 0)
                    {
                        _sunOspRecords = _sunOspRecordsByFid[_sunLinFastFid];
                        ParseSunshineOsp((byte)led, (byte)page, response);
                    }
                    else
                    {
                        _sunIseRecords = _sunIseRecordsByFid[_sunLinFastFid];
                        ParseSunshineIse((byte)led, (byte)page, response);
                    }
                }
            }
        }

        private static ushort SunshineLinFastCrc16(byte[] data)
        {
            ushort crc = 0xFFFF;
            if (data == null)
                return crc;
            foreach (byte value in data)
            {
                crc ^= (ushort)(value << 8);
                for (int bit = 0; bit < 8; bit++)
                    crc = (crc & 0x8000) != 0 ? (ushort)((crc << 1) ^ 0x1021) : (ushort)(crc << 1);
            }
            return crc;
        }

        private bool HandleSunshineUsbBulkFrame(LINFrame frame)
        {
            string transport = SunshineReadTransportName();
            if (frame.Direction != LINDirection.RX || !frame.IsValid || frame.Data == null || frame.DataLength < 8)
                return true;

            if (_sunLinFastMode && IsSunshineLinBulkGateway())
                return HandleSunshineLinFastFrame(frame);

            byte[] response = frame.Data;
            if ((response[0] & 0xF0) != SUN_EXT_RESPONSE)
                return true;
            byte fid = (byte)(response[0] & 0x0F);
            byte opcode = response[1];
            if (opcode == SUN_OP_USB_BULK_READ)
            {
                HandleSunshineUsbBulkControl(fid, response);
                return true;
            }
            if (opcode == SUN_OP_OSP_DATA && _sunReadTargetIndex == 0)
            {
                SunshineOspRecord[] records;
                if (!_sunOspRecordsByFid.TryGetValue(fid, out records))
                {
                    AbortSunshineRead(string.Format("{0} FID {1} OSP 시작 정보가 없습니다.", transport, fid));
                    return true;
                }
                _sunReadFid = fid;
                _sunOspRecords = records;
                ParseSunshineOsp(response[2], response[3], response);
                _sunBulkPages.Add(SunshineBulkPageKey(fid, response[2], response[3]));
                if (_sunBulkReadActive && response[3] == 4)
                {
                    _sunReadCompleted++;
                    if ((_sunReadCompleted & 0x07) == 0)
                        _sunStatus.Text = string.Format("{0} Read 수신 중... {1}/{2}개", transport, _sunReadCompleted, _sunReadTotalLeds);
                }
                return true;
            }
            if (opcode == SUN_OP_ISELED_DATA && _sunReadTargetIndex != 0)
            {
                SunshineIseRecord[] records;
                if (!_sunIseRecordsByFid.TryGetValue(fid, out records))
                {
                    AbortSunshineRead(string.Format("{0} FID {1} ISELED 시작 정보가 없습니다.", transport, fid));
                    return true;
                }
                _sunReadFid = fid;
                _sunIseRecords = records;
                ParseSunshineIse(response[2], response[3], response);
                _sunBulkPages.Add(SunshineBulkPageKey(fid, response[2], response[3]));
                byte lastPage = _sunBulkProtocolVersion >= 2 ? (byte)4 : (byte)3;
                if (_sunBulkReadActive && response[3] == lastPage)
                {
                    _sunReadCompleted++;
                    if ((_sunReadCompleted & 0x07) == 0)
                        _sunStatus.Text = string.Format("{0} Read 수신 중... {1}/{2}개", transport, _sunReadCompleted, _sunReadTotalLeds);
                }
                return true;
            }
            return true;
        }

        private void HandleSunshineUsbBulkControl(byte fid, byte[] response)
        {
            byte eventCode = response[2];
            byte target = response[3];
            string transport = SunshineReadTransportName();
            if (target != (_sunReadTargetIndex == 0 ? SUN_TARGET_FIO : SUN_TARGET_ISELED))
            {
                AbortSunshineRead(transport + " 전체 Read 대상 정보가 일치하지 않습니다.");
                return;
            }
            if (eventCode == SUN_USB_BULK_START)
            {
                _sunBulkStarted = true;
                _sunBulkProtocolVersion = response[7];
                _sunBulkActiveMask = response[5];
                _sunStatus.Text = string.Format("{0} Scan 시작... Active FID Mask=0x{1:X2} / Protocol V{2}", transport, _sunBulkActiveMask, _sunBulkProtocolVersion);
                return;
            }
            if (eventCode == SUN_USB_BULK_FID_BEGIN)
            {
                int ledCount = response[4];
                if (fid == 0 || ledCount <= 0)
                {
                    AbortSunshineRead(transport + " FID 시작 정보가 올바르지 않습니다.");
                    return;
                }
                _sunReadFid = fid;
                _sunReadMaximumLedCount = Math.Max(_sunReadMaximumLedCount, ledCount);
                _sunReadTotalLeds += ledCount;
                if (_sunReadTargetIndex == 0)
                {
                    _sunOspRecords = Enumerable.Range(0, ledCount).Select(value => new SunshineOspRecord()).ToArray();
                    _sunOspRecordsByFid[fid] = _sunOspRecords;
                }
                else
                {
                    _sunIseRecords = Enumerable.Range(0, ledCount).Select(value => new SunshineIseRecord()).ToArray();
                    _sunIseRecordsByFid[fid] = _sunIseRecords;
                }
                _sunStatus.Text = string.Format("{0} FID {1} 데이터 수신 중... {2}개", transport, fid, ledCount);
                return;
            }
            if (eventCode == SUN_USB_BULK_FID_END)
            {
                _sunBulkCompletedMask = response[6];
                _sunBulkError |= response[5];
                _sunBulkRemoteCode = response[7];
                _sunStatus.Text = response[5] == 0
                    ? string.Format("{0} FID {1} 수신 완료", transport, fid)
                    : string.Format("{0} FID {1} 수신 완료 / 상태 0x{2:X2}", transport, fid, response[5]);
                return;
            }
            if (eventCode == SUN_USB_BULK_COMPLETE)
            {
                _sunBulkCompletedMask = response[6];
                _sunBulkError |= response[7];
                ApplySunshineLedCount(_sunReadTargetIndex, _sunReadMaximumLedCount);
                if (IsSunshineLinBulkGateway() && QueueSunshineLinMissingPages())
                    return;
                byte readError = _sunBulkError;
                FinishSunshineRead();
                if (readError != 0)
                    _sunStatus.Text += string.Format(" / 진단 상태 0x{0:X2}", readError);
                if (_sunBulkRemoteCode != 0)
                    _sunStatus.Text += string.Format(" / LAN8661 응답 0x{0:X2}", _sunBulkRemoteCode);
                return;
            }
            if (eventCode == SUN_USB_BULK_ERROR)
            {
                byte error = response[4];
                AbortSunshineRead(string.Format("{0} 전체 Read 실패: {1}", transport, SunshineUsbBulkErrorText(error)));
            }
        }

        private static int SunshineBulkPageKey(byte fid, byte led, byte page)
        {
            return (fid << 16) | (led << 8) | page;
        }

        private bool QueueSunshineLinMissingPages()
        {
            int missingCount = 0;
            _sunBulkReadActive = false;
            _sunReadTimer.Stop();
            ChangeSunshineUsbTimer(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
            _sunReadRequests.Clear();
            _sunReadCurrent = null;
            _sunReadTotal = 0;
            _sunReadCompleted = 0;

            if (_sunReadTargetIndex == 0)
            {
                foreach (KeyValuePair<byte, SunshineOspRecord[]> item in _sunOspRecordsByFid.OrderBy(value => value.Key))
                {
                    byte fid = item.Key;
                    for (byte led = 1; led <= item.Value.Length; led++)
                    {
                        for (byte page = 0; page <= 4; page++)
                        {
                            int key = SunshineBulkPageKey(fid, led, page);
                            if (_sunBulkPages.Contains(key))
                                continue;
                            byte capturedFid = fid;
                            byte capturedLed = led;
                            byte capturedPage = page;
                            EnqueueSunshineRead(BuildSunshineReadPacketForFid(fid, SUN_OP_OSP_DATA, led, page, 0), repair =>
                            {
                                _sunReadFid = capturedFid;
                                _sunOspRecords = _sunOspRecordsByFid[capturedFid];
                                ParseSunshineOsp(capturedLed, capturedPage, repair);
                                _sunBulkPages.Add(SunshineBulkPageKey(capturedFid, capturedLed, capturedPage));
                            });
                            missingCount++;
                        }
                    }
                }
            }
            else
            {
                foreach (KeyValuePair<byte, SunshineIseRecord[]> item in _sunIseRecordsByFid.OrderBy(value => value.Key))
                {
                    byte fid = item.Key;
                    for (byte led = 1; led <= item.Value.Length; led++)
                    {
                        for (byte page = 0; page <= 4; page++)
                        {
                            int key = SunshineBulkPageKey(fid, led, page);
                            if (_sunBulkPages.Contains(key))
                                continue;
                            byte capturedFid = fid;
                            byte capturedLed = led;
                            byte capturedPage = page;
                            EnqueueSunshineRead(BuildSunshineReadPacketForFid(fid, SUN_OP_ISELED_DATA, led, page, 0), repair =>
                            {
                                _sunReadFid = capturedFid;
                                _sunIseRecords = _sunIseRecordsByFid[capturedFid];
                                ParseSunshineIse(capturedLed, capturedPage, repair);
                                _sunBulkPages.Add(SunshineBulkPageKey(capturedFid, capturedLed, capturedPage));
                            });
                            missingCount++;
                        }
                    }
                }
            }

            if (missingCount == 0)
                return false;
            _sunStatus.Text = string.Format("LIN 누락 데이터 {0}개 재수신 중...", missingCount);
            SendNextSunshineRead();
            return true;
        }

        private static string SunshineUsbBulkErrorText(byte error)
        {
            if (error == 0x01)
                return "MCU Read 작업 중입니다.";
            if (error == 0xE1)
                return "요청 값이 올바르지 않습니다.";
            if (error == 0xE3)
                return "연결된 FID 또는 LED 데이터가 없습니다.";
            return "Error 0x" + error.ToString("X2");
        }

        private void QueueSunshineFidDiscovery(byte fid)
        {
            byte target = _sunReadTargetIndex == 0 ? SUN_TARGET_FIO : SUN_TARGET_ISELED;
            EnqueueSunshineRead(BuildSunshineReadPacketForFid(fid, SUN_OP_SELECTION_READ, target, 0, 0), response =>
            {
                byte responseFid = (byte)(response[0] & 0x0F);
                int ledCount = response[6];
                if (responseFid == fid && ledCount > 0)
                {
                    _sunPendingFidReads.Enqueue(new SunshineFidRead { Fid = fid, LedCount = ledCount });
                    _sunReadMaximumLedCount = Math.Max(_sunReadMaximumLedCount, ledCount);
                    _sunReadTotalLeds += ledCount;
                }

                if (_sunRequestedFid == 0 && fid < 8)
                {
                    QueueSunshineFidDiscovery((byte)(fid + 1));
                    return;
                }
                if (_sunPendingFidReads.Count == 0)
                {
                    AbortSunshineRead(_sunRequestedFid == 0
                        ? "연결된 FID에서 Read 가능한 LED를 찾지 못했습니다."
                        : string.Format("FID {0}에 Read 가능한 LED가 없습니다.", _sunRequestedFid));
                    return;
                }
                ApplySunshineLedCount(_sunReadTargetIndex, _sunReadMaximumLedCount);
                QueueNextSunshineFidRead();
            });
        }

        private void QueueNextSunshineFidRead()
        {
            if (_sunPendingFidReads.Count == 0)
                return;

            SunshineFidRead read = _sunPendingFidReads.Dequeue();
            _sunReadFid = read.Fid;
            if (_sunReadTargetIndex == 0)
            {
                _sunOspRecords = Enumerable.Range(0, read.LedCount).Select(value => new SunshineOspRecord()).ToArray();
                _sunOspRecordsByFid[read.Fid] = _sunOspRecords;
            }
            else
            {
                _sunIseRecords = Enumerable.Range(0, read.LedCount).Select(value => new SunshineIseRecord()).ToArray();
                _sunIseRecordsByFid[read.Fid] = _sunIseRecords;
            }

            _sunStatus.Text = string.Format("FID {0} Scan 요청...", read.Fid);
            byte scanFlags = SunshineReadScanFlags();
            _sunScanStartedAt = DateTime.UtcNow;
            EnqueueSunshineRead(BuildSunshineReadPacketForFid(read.Fid,
                SUN_OP_SCAN_START, scanFlags, 0, 0), response =>
            {
                if (response[4] != 0)
                {
                    AbortSunshineRead("FID " + read.Fid + ": " + SunshineScanStartError(response[4]));
                    return;
                }
                QueueSunshineScanStatus(read);
            });
        }

        private void QueueSunshineScanStatus(SunshineFidRead read)
        {
            EnqueueSunshineRead(BuildSunshineReadPacketForFid(read.Fid,
                SUN_OP_SCAN_STATUS, 0, 0, 0), response =>
            {
                byte state = response[4];
                byte progress = response[5];
                _sunStatus.Text = string.Format("FID {0} Scan {1}%  State={2}  Error=0x{3:X2}", read.Fid, progress, state, response[6]);
                if (state == 4)
                {
                    QueueSunshineDataReads(read);
                }
                else if ((DateTime.UtcNow - _sunScanStartedAt).TotalSeconds >= 30)
                    AbortSunshineRead(string.Format("FID {0} Scan 제한 시간(30초) 초과", read.Fid));
                else
                    QueueSunshineScanStatus(read);
            });
        }

        private void QueueSunshineDataReads(SunshineFidRead read)
        {
            _sunStatus.Text = string.Format("FID {0} LED별 데이터 Read...", read.Fid);
            if (_sunReadTargetIndex == 0)
            {
                for (byte led = 1; led <= read.LedCount; led++)
                {
                    for (byte page = 0; page <= 4; page++)
                    {
                        byte capturedLed = led;
                        byte capturedPage = page;
                        bool last = led == read.LedCount && page == 4;
                        EnqueueSunshineRead(BuildSunshineReadPacketForFid(read.Fid,
                            SUN_OP_OSP_DATA, capturedLed, capturedPage, 0), response =>
                        {
                            ParseSunshineOsp(capturedLed, capturedPage, response);
                            if (last && _sunReadActive)
                            {
                                PopulateSunshineGrid();
                                QueueNextSunshineFidRead();
                            }
                        });
                    }
                }
                return;
            }
            for (byte led = 1; led <= read.LedCount; led++)
            {
                for (byte page = 0; page <= 4; page++)
                {
                    byte capturedLed = led;
                    byte capturedPage = page;
                    bool last = led == read.LedCount && page == 4;
                    EnqueueSunshineRead(BuildSunshineReadPacketForFid(read.Fid,
                        SUN_OP_ISELED_DATA, capturedLed, capturedPage, 0), response =>
                    {
                        ParseSunshineIse(capturedLed, capturedPage, response);
                        if (last && _sunReadActive)
                        {
                            PopulateSunshineGrid();
                            QueueNextSunshineFidRead();
                        }
                    });
                }
            }
        }

        private void EnqueueSunshineRead(byte[] packet, Action<byte[]> handler)
        {
            _sunReadRequests.Enqueue(new SunshineReadRequest { Packet = packet, Handler = handler });
            _sunReadTotal++;
        }

        private void SendNextSunshineRead()
        {
            if (_sunReadCurrent != null)
                return;
            if (_sunReadRequests.Count == 0)
            {
                if (_sunReadActive)
                    FinishSunshineRead();
                return;
            }
            _sunReadCurrent = _sunReadRequests.Dequeue();
            _sunReadRetry = 0;
            SendCurrentSunshineRead();
        }

        private void SendCurrentSunshineRead()
        {
            SunshineReadRequest current = _sunReadCurrent;
            if (current == null || current.Packet == null)
            {
                AbortSunshineRead("Read 요청 상태가 초기화되었습니다.");
                return;
            }
            if (!SendSunshinePublisher(current.Packet))
            {
                AbortSunshineRead("Read 요청 전송 실패");
                return;
            }
            _sunReadSentAt = DateTime.UtcNow;
            _sunSubscriberSent = IsSunshineCanGateway();
            _sunSubscriberSentAt = DateTime.MinValue;
            if (IsSunshineUsbGateway())
            {
                _sunReadTimer.Stop();
                ChangeSunshineUsbTimer(1, 4);
            }
            else
            {
                ChangeSunshineUsbTimer(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
                _sunReadTimer.Start();
            }
        }

        private void SunshineReadTimer_Tick(object sender, EventArgs e)
        {
            if (_sunBulkReadActive)
            {
                if (_gw != null)
                    _gw.Poll();
                double timeoutSeconds = IsSunshineLinBulkGateway() ? 60 : 30;
                if ((DateTime.UtcNow - _sunReadStartedAt).TotalSeconds >= timeoutSeconds)
                {
                    _sunBulkReadActive = false;
                    AbortSunshineRead(string.Format("{0} 전체 Read 제한 시간({1:F0}초) 초과", SunshineReadTransportName(), timeoutSeconds));
                }
                return;
            }
            SunshineReadRequest current = _sunReadCurrent;
            if (current == null || current.Packet == null || _gw == null)
                return;
            _gw.Poll();
            double elapsed = (DateTime.UtcNow - _sunReadSentAt).TotalMilliseconds;
            double subscriberDelay = 15;
            if (!IsSunshineCanGateway() && elapsed >= subscriberDelay && !_sunSubscriberSent)
            {
                byte responseId = IsSunshineUsbGateway() ? AD3306LedCommand.FrameId : SUN_EXT_LIN_RESPONSE_ID;
                _sunSubscriberSent = _gw.SendSubscriberRequest(responseId, 8, LINChecksumType.Enhanced);
                if (_sunSubscriberSent)
                    _sunSubscriberSentAt = DateTime.UtcNow;
            }
            double responseTimeoutMs = IsSunshineLinBulkGateway() ? 600 : 1500;
            if (elapsed < responseTimeoutMs)
                return;
            int maximumRetry = IsSunshineLinBulkGateway() ? 3 : 1;
            if (_sunReadRetry < maximumRetry)
            {
                _sunReadRetry++;
                SendCurrentSunshineRead();
                return;
            }
            AbortSunshineRead("응답 시간 초과: opcode 0x" + current.Packet[1].ToString("X2"));
        }

        private void SunshineUsbReadTimer_Tick(object state)
        {
            if (System.Threading.Interlocked.Exchange(ref _sunUsbTimerBusy, 1) != 0)
                return;
            try
            {
                if (_sunBulkReadActive && IsSunshineLinBulkGateway())
                {
                    if (_gw == null)
                        return;
                    _gw.Poll();
                    if (TrySendSunshineLinFastPublisher())
                        return;
                    DateTime now = DateTime.UtcNow;
                    if (_sunLinFastRetryWaiting && (now - _sunLinFastRetrySentAt).TotalMilliseconds >= SUN_LIN_FAST_CONTROL_TIMEOUT_MS)
                    {
                        if (_sunLinFastRetryCount >= 3)
                        {
                            BeginInvoke(new Action(() => AbortSunshineRead("LIN 누락 프레임 재수신 실패")));
                            return;
                        }
                        _sunLinFastRetryCount++;
                        _sunLinFastRetrySentAt = now;
                        QueueSunshineLinFastPublisher(BuildSunshinePacketForFid(_sunLinFastFid, SUN_OP_LIN_FAST_CONTROL, SUN_LIN_FAST_RETRY, _sunLinFastRetryChunk, 0));
                        return;
                    }
                    if (_sunLinFastPendingFidAck && (now - _sunLinFastPendingFidSentAt).TotalMilliseconds >= SUN_LIN_FAST_CONTROL_TIMEOUT_MS)
                    {
                        _sunLinFastPendingFidRetryCount++;
                        if (_sunLinFastPendingFidRetryCount >= 3 && SunshineLinFastRequestedFidsComplete())
                        {
                            _sunLinFastPendingFidAck = false;
                            byte[] completion = new byte[8];
                            completion[0] = SUN_LIN_FAST_COMPLETE;
                            completion[4] = _sunBulkCompletedMask;
                            completion[5] = _sunBulkError;
                            if (!IsDisposed && !Disposing && IsHandleCreated)
                            {
                                BeginInvoke(new Action(() =>
                                {
                                    if (_sunBulkReadActive && _sunLinFastMode)
                                        CompleteSunshineLinFastRead(completion);
                                }));
                            }
                            return;
                        }
                        _sunLinFastPendingFidSentAt = now;
                        QueueSunshineLinFastPublisher(BuildSunshinePacketForFid(_sunLinFastPendingFid, SUN_OP_LIN_FAST_CONTROL, SUN_LIN_FAST_NEXT, 0, 0));
                        return;
                    }
                    if (_sunBulkStarted && !_sunLinFastRetryWaiting && !_sunLinFastPendingFidAck &&
                        (now - _sunLinFastLastFrameAt).TotalMilliseconds >= SUN_LIN_FAST_CONTROL_TIMEOUT_MS &&
                        (now - _sunLinFastStatusSentAt).TotalMilliseconds >= SUN_LIN_FAST_CONTROL_TIMEOUT_MS)
                    {
                        _sunLinFastStatusRetryCount++;
                        _sunLinFastStatusSentAt = now;
                        QueueSunshineLinFastPublisher(BuildSunshinePacketForFid(_sunLinFastFid, SUN_OP_LIN_FAST_CONTROL, SUN_LIN_FAST_STATUS, 0, 0));
                        return;
                    }
                    if (!_sunLinHardwareSchedule)
                    {
                        if (_sunLinFastSubscriberPending && (now - _sunLinFastSubscriberSentAt).TotalMilliseconds >= SUN_LIN_FAST_SUBSCRIBER_TIMEOUT_MS)
                        {
                            _sunLinFastSubscriberPending = false;
                            _sunLinFastNextSubscriberAt = now;
                        }
                        if (!_sunLinFastSubscriberPending && now >= _sunLinFastNextSubscriberAt)
                        {
                            if (_gw.SendSubscriberRequest(SUN_EXT_LIN_RESPONSE_ID, 8, LINChecksumType.Enhanced))
                            {
                                _sunLinFastSubscriberPending = true;
                                _sunLinFastSubscriberSentAt = now;
                            }
                            else
                            {
                                _sunLinFastNextSubscriberAt = now.AddMilliseconds(SUN_LIN_FAST_SUBSCRIBER_DELAY_MS);
                            }
                        }
                    }
                    if (!_sunBulkStarted && _sunBulkRequestPacket != null &&
                        (DateTime.UtcNow - _sunBulkLastRequestAt).TotalMilliseconds >= 1500 &&
                        _sunBulkStartRetry < 3)
                    {
                        _sunBulkStartRetry++;
                        _sunBulkLastRequestAt = DateTime.UtcNow;
                        SendSunshineLinFastPublisherNow(_sunBulkRequestPacket);
                    }
                    else if (_sunLinFastMode && !_sunBulkStarted && _sunBulkStartRetry >= 3 && (DateTime.UtcNow - _sunBulkLastRequestAt).TotalMilliseconds >= 1500)
                    {
                        byte target = _sunReadTargetIndex == 0 ? SUN_TARGET_FIO : SUN_TARGET_ISELED;
                        byte scanFlags = SunshineReadScanFlags();
                        StopSunshineLinSchedule();
                        SendSunshinePublisher(BuildSunshinePacketForFid(_sunRequestedFid, SUN_OP_LIN_FAST_CONTROL, SUN_LIN_FAST_ABORT, 0, 0));
                        _sunLinFastMode = false;
                        _sunBulkStartRetry = 0;
                        _sunBulkRequestPacket = BuildSunshineReadPacketForFid(
                            _sunRequestedFid, SUN_OP_USB_BULK_READ,
                            target, scanFlags, 0);
                        _sunBulkLastRequestAt = DateTime.UtcNow;
                        _sunStatus.Text = "LIN 고속 프로토콜 미응답 / 기존 Read 방식으로 전환 중...";
                        SendSunshinePublisher(_sunBulkRequestPacket);
                        ChangeSunshineUsbTimer(1, 8);
                    }
                    if ((DateTime.UtcNow - _sunReadStartedAt).TotalSeconds >= 120)
                    {
                        ChangeSunshineUsbTimer(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
                        if (!IsDisposed && !Disposing && IsHandleCreated)
                            BeginInvoke(new Action(() => AbortSunshineRead("LIN 전체 Read 제한 시간(120초) 초과")));
                    }
                    return;
                }
                SunshineReadRequest current = _sunReadCurrent;
                if (current == null || _gw == null || !IsSunshineUsbGateway())
                    return;

                _gw.Poll();
                double elapsed = (DateTime.UtcNow - _sunReadSentAt).TotalMilliseconds;
                if (elapsed >= 4 && !_sunSubscriberSent)
                {
                    _sunSubscriberSent = _gw.SendSubscriberRequest(AD3306LedCommand.FrameId, 8, LINChecksumType.Enhanced);
                    if (_sunSubscriberSent)
                        _sunSubscriberSentAt = DateTime.UtcNow;
                }
                if (elapsed < 1500 || !ReferenceEquals(current, _sunReadCurrent))
                    return;

                ChangeSunshineUsbTimer(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
                if (IsDisposed || Disposing || !IsHandleCreated)
                    return;
                BeginInvoke(new Action(() =>
                {
                    if (!ReferenceEquals(current, _sunReadCurrent) || current.Packet == null)
                        return;
                    if (_sunReadRetry == 0)
                    {
                        _sunReadRetry = 1;
                        SendCurrentSunshineRead();
                    }
                    else
                    {
                        AbortSunshineRead("응답 시간 초과: opcode 0x" + current.Packet[1].ToString("X2"));
                    }
                }));
            }
            finally
            {
                System.Threading.Volatile.Write(ref _sunUsbTimerBusy, 0);
            }
        }

        private bool SunshineLinFastRequestedFidsComplete()
        {
            byte expectedMask;
            if (_sunRequestedFid == 0)
                expectedMask = _sunBulkActiveMask;
            else if (_sunRequestedFid <= 8)
                expectedMask = (byte)(1 << (_sunRequestedFid - 1));
            else
                expectedMask = 0;
            return expectedMask != 0 && (_sunBulkCompletedMask & expectedMask) == expectedMask;
        }

        private void SunshineFrameReceived(LINFrame frame)
        {
            bool bulkFrame = frame != null &&
                ((IsSunshineUsbGateway() && frame.FrameID == SUN_USB_BULK_FRAME_ID) ||
                 (IsSunshineCanGateway() && frame.FrameID == AD3306LedCommand.FrameId) ||
                 (IsSunshineLinBulkGateway() && frame.FrameID == SUN_EXT_LIN_RESPONSE_ID));
            if (_sunBulkReadActive && bulkFrame)
            {
                if (InvokeRequired)
                {
                    if (!IsDisposed && !Disposing && IsHandleCreated)
                        BeginInvoke(new Action<LINFrame>(SunshineFrameReceived), frame);
                    return;
                }
                if (HandleSunshineUsbBulkFrame(frame))
                    return;
            }
            SunshineReadRequest current = _sunReadCurrent;
            if (!_sunReadActive || current == null || current.Packet == null || frame == null || frame.Direction != LINDirection.RX || !frame.IsValid || frame.Data == null || frame.DataLength < 8)
                return;
            if ((frame.Data[0] & 0xF0) != SUN_EXT_RESPONSE || frame.Data[1] != current.Packet[1] || frame.Data[2] != current.Packet[2] || frame.Data[3] != current.Packet[3])
                return;
            byte requestedFid = (byte)(current.Packet[0] & 0x0F);
            byte responseFid = (byte)(frame.Data[0] & 0x0F);
            if (requestedFid != 0 && responseFid != 0 && requestedFid != responseFid)
                return;
            if (current.Packet[1] == SUN_OP_SELECTION_READ)
            {
                int maximumLedCount = current.Packet[2] == SUN_TARGET_FIO ? FIO_LED_COUNT : ISE_LED_COUNT;
                if (frame.Data[6] == 0 || frame.Data[6] > maximumLedCount)
                    return;
            }
            if (InvokeRequired)
            {
                ChangeSunshineUsbTimer(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
                if (IsDisposed || Disposing || !IsHandleCreated)
                    return;
                BeginInvoke(new Action<LINFrame>(SunshineFrameReceived), frame);
                return;
            }

            if (!ReferenceEquals(current, _sunReadCurrent))
                return;
            SunshineReadRequest completed = current;
            _sunReadTimer.Stop();
            ChangeSunshineUsbTimer(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
            _sunReadCurrent = null;
            _sunReadCompleted++;
            if (_sunReadTotal > 0 && completed.Packet[1] != SUN_OP_SCAN_STATUS)
                _sunStatus.Text = string.Format("Read {0}/{1}", _sunReadCompleted, _sunReadTotal);
            if (completed.Handler != null)
                completed.Handler((byte[])frame.Data.Clone());
            SendNextSunshineRead();
        }

        private void ParseSunshineOsp(byte led, byte page, byte[] response)
        {
            if (_sunOspRecords == null || led == 0 || led > _sunOspRecords.Length || response == null || response.Length < 8)
            {
                AbortSunshineRead("OSP Read 응답 상태가 올바르지 않습니다.");
                return;
            }
            SunshineOspRecord record = _sunOspRecords[led - 1];
            if (page == 0)
            {
                record.Valid = response[4];
                record.Error = response[5];
                record.Open = response[6];
                record.Short = response[7];
            }
            else if (page == 1)
            {
                record.Factor[0] = ReadSunshineUInt16(response, 4);
                record.Factor[1] = ReadSunshineUInt16(response, 6);
            }
            else if (page == 2)
            {
                record.Factor[2] = ReadSunshineUInt16(response, 4);
                record.Temperature = unchecked((short)ReadSunshineUInt16(response, 6));
            }
            else if (page == 3)
            {
                record.Pwm[0] = ReadSunshineUInt16(response, 4);
                record.Pwm[1] = ReadSunshineUInt16(response, 6);
            }
            else if (page == 4)
            {
                record.Pwm[2] = ReadSunshineUInt16(response, 4);
                record.TemperatureRaw = response[6];
                record.LedStatus = response[7];
            }
        }

        private void ParseSunshineIse(byte led, byte page, byte[] response)
        {
            if (_sunIseRecords == null || led == 0 || led > _sunIseRecords.Length || response == null || response.Length < 8)
            {
                AbortSunshineRead("ISELED Read 응답 상태가 올바르지 않습니다.");
                return;
            }
            SunshineIseRecord record = _sunIseRecords[led - 1];
            if (page == 0)
            {
                record.Valid = (byte)(response[4] & 0x0F);
                record.Error = (byte)(response[5] & 0x0F);
                record.OpenShort = (byte)(response[6] & 0x07);
                record.PwmMax[0] = (ushort)((record.PwmMax[0] & 0x00FF) | ((response[4] & 0xF0) << 4));
                record.PwmMax[1] = (ushort)((record.PwmMax[1] & 0x00FF) | ((response[5] & 0xF0) << 4));
                record.PwmMax[2] = (ushort)((record.PwmMax[2] & 0x00FF) | ((response[6] & 0xF0) << 4));
                record.PwmValidMask = (byte)(response[7] & 0x07);
                if ((_sunBulkProtocolVersion < 2) && (record.PwmValidMask == 0) && ((record.Valid & 0x02) != 0))
                    record.PwmValidMask = 0x07;
            }
            else if (page == 1)
            {
                record.TemperatureRaw = ReadSunshineUInt16(response, 4);
                record.Temperature = unchecked((short)ReadSunshineUInt16(response, 6));
            }
            else if (page == 2)
            {
                record.Pwm[0] = ReadSunshineUInt16(response, 4);
                record.Pwm[1] = ReadSunshineUInt16(response, 6);
            }
            else if (page == 3)
            {
                record.Pwm[2] = ReadSunshineUInt16(response, 4);
                record.DeviceStatus = ReadSunshineUInt16(response, 6);
            }
            else if (page == 4)
            {
                record.PwmMax[0] = (ushort)((record.PwmMax[0] & 0x0F00) | response[4]);
                record.PwmMax[1] = (ushort)((record.PwmMax[1] & 0x0F00) | response[5]);
                record.PwmMax[2] = (ushort)((record.PwmMax[2] & 0x0F00) | response[6]);
                record.PwmMaxValidMask = (byte)(response[7] & 0x07);
            }
        }

        private static ushort ReadSunshineUInt16(byte[] data, int offset)
        {
            return (ushort)(data[offset] | (data[offset + 1] << 8));
        }

        private void FinishSunshineRead()
        {
            _sunReadTimer.Stop();
            ChangeSunshineUsbTimer(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
            _sunBulkReadActive = false;
            ResetSunshineLinFastState();
            _sunReadActive = false;
            AD3306LedCommand.ServiceReadActive = false;
            _sunRead.Enabled = true;
            if (_sunReadShowGrid)
            {
                PopulateSunshineGrid();
                double elapsed = (DateTime.UtcNow - _sunReadStartedAt).TotalSeconds;
                _sunStatus.Text = _sunReadTargetIndex == 0
                    ? string.Format("Read 완료 ({0}): {1} / 총 {2}개 / {3:F1}초", SunshineOspCurrentModeText(), BuildSunshineFidSummary(_sunOspRecordsByFid), _sunReadTotalLeds, elapsed)
                    : string.Format("Read 완료: {0} / 총 {1}개 / {2:F1}초", BuildSunshineFidSummary(_sunIseRecordsByFid), _sunReadTotalLeds, elapsed);
                if (_sunLinFastFailedFids.Count != 0)
                    _sunStatus.Text += " / CRC 실패 FID " + string.Join(",", _sunLinFastFailedFids.OrderBy(value => value));
            }
            else
            {
                _sunStatus.Text = "수량 확인 완료: OSP " + _sunLedCounts[0] + " / ISE " + _sunLedCounts[1];
            }
            Action completed = _sunReadFinished;
            _sunReadFinished = null;
            if (completed != null)
                completed();
        }

        private static string BuildSunshineFidSummary<T>(Dictionary<byte, T[]> recordsByFid)
        {
            if (recordsByFid == null || recordsByFid.Count == 0)
                return "FID 없음";
            return string.Join(", ", recordsByFid.OrderBy(item => item.Key)
                .Select(item => "FID" + item.Key + "=" + item.Value.Length + "개"));
        }

        private void AbortSunshineRead(string message)
        {
            _sunReadTimer.Stop();
            ChangeSunshineUsbTimer(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
            StopSunshineLinSchedule();
            if (_sunLinFastMode && _gw != null && _gw.IsConnected)
                SendSunshinePublisher(BuildSunshinePacketForFid(_sunLinFastFid, SUN_OP_LIN_FAST_CONTROL, SUN_LIN_FAST_ABORT, 0, 0));
            _sunBulkReadActive = false;
            ResetSunshineLinFastState();
            _sunReadRequests.Clear();
            _sunPendingFidReads.Clear();
            _sunReadCurrent = null;
            _sunReadActive = false;
            AD3306LedCommand.ServiceReadActive = false;
            _sunRead.Enabled = true;
            _sunReadFinished = null;
            _sunStatus.Text = message;
            OspMessage?.Invoke("[Sunshine Read] " + message);
        }

        private void PopulateSunshineGrid()
        {
            if (_sunGrid == null)
                return;
            UpdateSunshineReadModeHeaders();
            _sunGrid.Rows.Clear();
            if (_sunReadTargetIndex == 0)
            {
                if (_sunOspRecordsByFid.Count == 0)
                {
                    _sunStatus.Text = "OSP Read 결과가 없습니다.";
                    return;
                }
                foreach (KeyValuePair<byte, SunshineOspRecord[]> fidRecords in _sunOspRecordsByFid.OrderBy(item => item.Key))
                {
                    AddSunshineFidSeparator("OSP", fidRecords.Key, fidRecords.Value.Length);
                    for (int index = 0; index < fidRecords.Value.Length; index++)
                    {
                        SunshineOspRecord record = fidRecords.Value[index];
                        _sunGrid.Rows.Add("OSP FID" + fidRecords.Key, index + 1,
                            FormatSunshineFactor(record, 0), FormatSunshineFactor(record, 1), FormatSunshineFactor(record, 2),
                            (record.Valid & 0x02) != 0 ? record.Pwm[0].ToString() : "-",
                            (record.Valid & 0x02) != 0 ? record.Pwm[1].ToString() : "-",
                            (record.Valid & 0x02) != 0 ? record.Pwm[2].ToString() : "-",
                            (record.Valid & 0x04) != 0 ? (record.Temperature / 10.0).ToString("F1") + " °C" : "-",
                            SunshineFaultText(record.Open), SunshineFaultText(record.Short),
                            SunshineOspStatus(record), SunshineOspDetail(record));
                    }
                }
                return;
            }
            if (_sunIseRecordsByFid.Count == 0)
            {
                _sunStatus.Text = "ISELED Read 결과가 없습니다.";
                return;
            }
            foreach (KeyValuePair<byte, SunshineIseRecord[]> fidRecords in _sunIseRecordsByFid.OrderBy(item => item.Key))
            {
                AddSunshineFidSeparator("ISE", fidRecords.Key, fidRecords.Value.Length);
                for (int index = 0; index < fidRecords.Value.Length; index++)
                {
                    SunshineIseRecord record = fidRecords.Value[index];
                    string diagnostic = (record.Valid & 0x08) != 0 ? SunshineFaultText(record.OpenShort) : "N/A";
                    string shortDiagnostic = (record.Valid & 0x08) == 0 ? "N/A" : (record.OpenShort == 0 ? "None" : "구분 불가");
                    string status = SunshineIseStatus(record);
                    _sunGrid.Rows.Add("ISE FID" + fidRecords.Key, index + 1,
                        FormatSunshineIseTune(record, 0), FormatSunshineIseTune(record, 1), FormatSunshineIseTune(record, 2),
                        (record.PwmValidMask & 0x01) != 0 ? record.Pwm[0].ToString() : "-",
                        (record.PwmValidMask & 0x02) != 0 ? record.Pwm[1].ToString() : "-",
                        (record.PwmValidMask & 0x04) != 0 ? record.Pwm[2].ToString() : "-",
                        ((record.Valid & 0x04) != 0 && record.Temperature != short.MinValue)
                            ? (record.Temperature / 10.0).ToString("F1") + " °C"
                            : "Raw " + record.TemperatureRaw,
                        diagnostic, shortDiagnostic,
                        status, SunshineIseDetail(record));
                }
            }
        }

        private void AddSunshineFidSeparator(string path, byte fid, int ledCount)
        {
            int rowIndex = _sunGrid.Rows.Add("\u25b6 " + path + " FID " + fid + "   (" + ledCount + " LEDs)");
            DataGridViewRow row = _sunGrid.Rows[rowIndex];
            row.Height = 26;
            row.DefaultCellStyle.BackColor = Color.Gainsboro;
            row.DefaultCellStyle.ForeColor = Color.Black;
            row.DefaultCellStyle.SelectionBackColor = Color.LightSteelBlue;
            row.DefaultCellStyle.SelectionForeColor = Color.Black;
            row.DefaultCellStyle.Font = new Font("Consolas", 8.5F, FontStyle.Bold);
        }

        private static string FormatSunshineFactor(SunshineOspRecord record, int channel)
        {
            return (record.Valid & 0x01) != 0 ? (record.Factor[channel] / 4096.0).ToString("F4") : "-";
        }

        private static string FormatSunshineIseTune(SunshineIseRecord record, int channel)
        {
            byte mask = (byte)(1 << channel);
            return (record.PwmMaxValidMask & mask) != 0 ? record.PwmMax[channel].ToString() : "-";
        }

        private static string SunshineScanStartError(byte status)
        {
            if (status == 0x01)
                return "MCU 초기 Tune Read 진행 중입니다. 잠시 후 다시 Read해 주세요.";
            if (status == 0xE1)
                return "선택한 FID에 Read 가능한 LED가 없습니다.";
            if (status == 0xE2)
                return "MCU가 지원하지 않는 Read 명령입니다.";
            return "Scan 시작 실패: 0x" + status.ToString("X2");
        }

        private static List<string> SunshineOspErrors(byte error)
        {
            List<string> errors = new List<string>();
            if ((error & 0x01) != 0) errors.Add("Tune/OTP Read 실패");
            if ((error & 0x02) != 0) errors.Add("PWM Read 실패");
            if ((error & 0x04) != 0) errors.Add("온도 Read 실패");
            if ((error & 0x08) != 0) errors.Add("Open/Short 상태 Read 실패");
            return errors;
        }

        private static string SunshineOspStatus(SunshineOspRecord record)
        {
            List<string> errors = SunshineOspErrors(record.Error);
            if (errors.Count == 1)
                return errors[0];
            if (errors.Count > 1)
                return "ERR 0x" + record.Error.ToString("X2");
            if (record.Open != 0 || record.Short != 0)
                return "LED Open/Short 감지";
            return "OK";
        }

        private static string SunshineOspDetail(SunshineOspRecord record)
        {
            List<string> errors = SunshineOspErrors(record.Error);
            if (errors.Count > 1)
                return string.Join(" / ", errors);
            if (errors.Count == 1)
                return "해당 항목을 다시 Read합니다.";
            if (record.Open != 0 || record.Short != 0)
                return "Open: " + SunshineFaultText(record.Open) + " / Short: " + SunshineFaultText(record.Short);
            return (record.Valid & 0x01) != 0 ? "Tune 저장 완료" : "Tune 값 없음";
        }

        private static string SunshineIseStatus(SunshineIseRecord record)
        {
            List<string> errors = SunshineIseErrors(record.Error);
            List<string> deviceErrors = SunshineIseDeviceErrors(record.DeviceStatus);
            if (errors.Count == 1) return errors[0];
            if (errors.Count > 1) return "ERR 0x" + record.Error.ToString("X2");
            if (deviceErrors.Count == 1) return deviceErrors[0];
            if (deviceErrors.Count > 1) return "Status 0x" + record.DeviceStatus.ToString("X3");
            if (record.OpenShort != 0) return "LED Open/Short 감지";
            return "OK";
        }

        private static string SunshineIseDetail(SunshineIseRecord record)
        {
            List<string> details = SunshineIseErrors(record.Error);
            string[] channel = { "R", "G", "B" };
            List<string> missingPwm = new List<string>();
            List<string> missingPwmMax = new List<string>();

            for (int index = 0; index < 3; index++)
            {
                byte mask = (byte)(1 << index);
                if ((record.PwmValidMask & mask) == 0)
                    missingPwm.Add(channel[index]);
                if ((record.PwmMaxValidMask & mask) == 0)
                    missingPwmMax.Add(channel[index]);
            }
            if (missingPwm.Count != 0)
                details.Add("PWM " + string.Join("/", missingPwm) + " Read 실패");
            if (missingPwmMax.Count != 0)
                details.Add("PWMMax " + string.Join("/", missingPwmMax) + " Read 실패");
            details.AddRange(SunshineIseDeviceErrors(record.DeviceStatus));
            if (record.OpenShort != 0)
                details.Add("Open/Short: " + SunshineFaultText(record.OpenShort) + " (ISELED는 Open/Short-to-GND 구분 불가)");
            return details.Count == 0 ? "-" : string.Join(" / ", details);
        }

        private static List<string> SunshineIseErrors(byte error)
        {
            List<string> errors = new List<string>();
            if ((error & 0x01) != 0) errors.Add("온도/Offset Read 실패");
            if ((error & 0x02) != 0) errors.Add("PWM/PWMMax Read 실패");
            if ((error & 0x04) != 0) errors.Add("Open/Short ADC Read 실패");
            if ((error & 0x08) != 0) errors.Add("ISELED Status Read 실패");
            return errors;
        }

        private static List<string> SunshineIseDeviceErrors(ushort status)
        {
            List<string> errors = new List<string>();
            if ((status & 0x100) != 0) errors.Add("Protocol 오류");
            if ((status & 0x080) != 0) errors.Add("Timeout 오류");
            if ((status & 0x040) != 0) errors.Add("Upstream CRC 오류");
            if ((status & 0x020) != 0) errors.Add("Downstream CRC 오류");
            if ((status & 0x010) != 0) errors.Add("저전압 감지");
            if ((status & 0x008) != 0) errors.Add("Command Overrun 오류");
            if ((status & 0x004) != 0) errors.Add("Undefined Command 오류");
            if ((status & 0x002) != 0) errors.Add("Frame/Frequency Sync 오류");
            if ((status & 0x001) != 0) errors.Add("Symbol Encoding 오류");
            return errors;
        }

        private static string SunshineFaultText(byte mask)
        {
            if (mask == 0)
                return "None";
            List<string> channels = new List<string>();
            if ((mask & 0x01) != 0) channels.Add("R");
            if ((mask & 0x02) != 0) channels.Add("G");
            if ((mask & 0x04) != 0) channels.Add("B");
            return string.Join("/", channels);
        }

        private bool IsSunshineCanGateway()
        {
            return _gw is LINMaster.CAN.CANoeCANGateway;
        }

        private bool IsSunshineUsbGateway()
        {
            return _gw is LINMaster.USB.USBGateway;
        }

        private bool IsSunshineLinBulkGateway()
        {
            return !IsSunshineUsbGateway() && !IsSunshineCanGateway();
        }

        private string SunshineReadTransportName()
        {
            if (IsSunshineUsbGateway())
                return "USB";
            if (IsSunshineCanGateway())
                return "CAN";
            return "LIN";
        }

        private void AttachSunshineGateway(ILINGateway gateway)
        {
            if (gateway != null)
                gateway.FrameReceived += SunshineFrameReceived;
        }

        private void DetachSunshineGateway(ILINGateway gateway)
        {
            if (gateway != null)
                gateway.FrameReceived -= SunshineFrameReceived;
        }

        private void UpdateSunshineGateway(ILINGateway gateway)
        {
            DetachSunshineGateway(_gw);
            AttachSunshineGateway(gateway);
            for (int index = 0; index < 2; index++)
            {
                _sunLedCountKnown[index] = false;
                _sunSelectionDirty[index] = true;
                _sunIndividualMaskDirty[index] = true;
                _sunGroupMaskDirty[index] = true;
            }
            _sunTuningDirty = true;
            _sunTemperatureDirty = true;
            _sunOspCurrentModeDirty = true;
        }

        private void CloseSunshineExtension()
        {
            _sunConfigTimer.Stop();
            _sunReadTimer.Stop();
            StopSunshineLinSchedule();
            if (_sunLinFastMode && _gw != null && _gw.IsConnected)
                SendSunshinePublisher(BuildSunshinePacketForFid(_sunLinFastFid, SUN_OP_LIN_FAST_CONTROL, SUN_LIN_FAST_ABORT, 0, 0));
            _sunBulkReadActive = false;
            ResetSunshineLinFastState();
            AD3306LedCommand.ServiceReadActive = false;
            System.Threading.Timer timer = System.Threading.Interlocked.Exchange(ref _sunUsbReadTimer, null);
            if (timer != null)
            {
                timer.Change(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
                timer.Dispose();
            }
            DetachSunshineGateway(_gw);
        }

        private void ChangeSunshineUsbTimer(int dueTime, int period)
        {
            System.Threading.Timer timer = _sunUsbReadTimer;
            if (timer == null)
                return;
            try
            {
                timer.Change(dueTime, period);
            }
            catch (ObjectDisposedException)
            {
            }
        }
    }
}
