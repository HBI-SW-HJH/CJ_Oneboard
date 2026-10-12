using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using Microsoft.Win32;

namespace LINMaster.CANoe
{
    public sealed class CANoeBridge
    {
        private const string ProgId = "CANoe.Application";
        private const string ApplicationClsid = "{7F31DEB1-5BCC-11d3-8562-00105A3E017B}";
        private const string PanelNamespace = "AD3304LED25";
        private const string CanNamespace = "SunshineCAN";
        private const string CanConfigurationFileName = "Sunshine_CAN.cfg";
        private const string CanConfigurationPathFileName = "CANoeConfigPath.txt";
        private const string CanConfigurationEnvironmentVariable = "SUNSHINE_CANOE_CONFIG";
        private const int CANoeReadyTimeoutMs = 15000;
        private const int CanSendConfirmTimeoutMs = 200;

        private global::CANoe.Application _application;
        private bool _startedByBridge;
        private readonly Dictionary<string, object> _systemVariableCache = new Dictionary<string, object>(StringComparer.OrdinalIgnoreCase);
        private readonly object _systemVariableCacheLock = new object();
        private readonly object _canSendLock = new object();

        public static string TargetConfigurationPath
        {
            get
            {
                return Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory),
                    @"jhhan\vector\Canoe_AD3304_LED_20260819\Ethernet\Ethernet.cfg");
            }
        }

        public static string TargetCanConfigurationPath
        {
            get { return FindCanConfigurationPath(); }
        }

        public static string CanConfigurationPathFile
        {
            get { return Path.Combine(AppDomain.CurrentDomain.BaseDirectory, CanConfigurationPathFileName); }
        }

        public bool IsAttached => _application != null;
        public bool IsInitialized { get; private set; }
        public string LastError { get; private set; }

        public void MarkInitialized()
        {
            IsInitialized = true;
        }

        public void MarkDisconnected()
        {
            IsInitialized = false;
            ReleaseApplication();
        }

        public bool TryAttach(bool allowStart, out string message)
        {
            if (_application != null)
            {
                try
                {
                    string applicationName = _application.Name;
                    message = "already attached to " + applicationName;
                    LastError = null;
                    return true;
                }
                catch
                {
                    ReleaseApplication();
                }
            }

            Type canoeType = Type.GetTypeFromProgID(ProgId);
            if (canoeType == null)
            {
                message = BuildRegistrationMessage("CANoe COM ProgID가 등록되어 있지 않습니다.");
                LastError = message;
                return false;
            }

            Exception activeObjectError = null;
            try
            {
                _application = (global::CANoe.Application)Marshal.GetActiveObject(ProgId);
                _startedByBridge = false;
                message = "attached to running CANoe";
                LastError = null;
                return true;
            }
            catch (Exception ex)
            {
                activeObjectError = ex;
                ReleaseApplication();
                if (!allowStart)
                {
                    message = "실행 중인 CANoe COM 인스턴스를 찾지 못했습니다: " + ex.Message;
                    LastError = message;
                    return false;
                }
            }

            string registeredServer = GetRegisteredServerPath();
            if (!IsCANoe20Registration(registeredServer))
            {
                message = BuildRegistrationMessage(
                    "CANoe.Application이 CANoe 20에 등록되어 있지 않습니다. 현재 등록 서버: " +
                    (string.IsNullOrWhiteSpace(registeredServer) ? "(없음)" : registeredServer));
                LastError = message;
                return false;
            }

            try
            {
                _application = (global::CANoe.Application)new global::CANoe.ApplicationClass();
                _startedByBridge = true;
                message = "started CANoe 20 through COM";
                LastError = null;
                return true;
            }
            catch (Exception ex)
            {
                ReleaseApplication();
                message = "CANoe 20 COM 시작 실패: " + ex.Message;
                if (activeObjectError != null)
                    message += " / 실행 인스턴스 연결 실패: " + activeObjectError.Message;
                LastError = message;
                return false;
            }
        }

        public bool TryPreparePanel(out string message)
        {
            if (!TryAttach(allowStart: false, out message))
                return false;

            if (!TryEnsurePanelConfiguration(out message))
                return false;

            if (!TryEnsureMeasurementRunning(out message))
                return false;

            if (!TrySetPanelVariable("AutoSend", 0, out message))
                return false;

            if (!TrySetPanelVariable("BtnInit", 1, out message))
                return false;

            message = "CANoe 20 / AD3304LED25 연결 및 Init 완료";
            LastError = null;
            return true;
        }

        public bool TryPrepareCan(out string message)
        {
            if (!TryAttach(allowStart: false, out message))
                return false;

            if (!TryEnsureCanConfiguration(out message))
                return false;

            if (!TryEnsureMeasurementRunning(out message))
                return false;

            Stopwatch wait = Stopwatch.StartNew();
            while (wait.ElapsedMilliseconds < CANoeReadyTimeoutMs)
            {
                object value;
                if (TryGetSystemVariable(CanNamespace, "Ready", out value, out message)
                    && Convert.ToInt32(value) == 1)
                {
                    message = "CANoe 20 / VN5650 CAN1 ready (Classic CAN 500 kbit/s)";
                    LastError = null;
                    return true;
                }
                Thread.Sleep(100);
            }

            message = "Sunshine CAN CAPL bridge did not become ready. Check the CANoe Write window.";
            LastError = message;
            return false;
        }

        public bool TrySetPanelVariable(string variableName, object value, out string message)
        {
            return TrySetSystemVariable(PanelNamespace, variableName, value, out message);
        }

        public bool TrySetSystemVariable(string namespaceName, string variableName, object value, out string message)
        {
            if (!TryAttach(allowStart: false, out message))
            {
                LastError = message;
                return false;
            }

            try
            {
                dynamic variable = ResolveSystemVariable(namespaceName, variableName);
                variable.Value = value;
                message = namespaceName + "::" + variableName + "=" + value;
                LastError = null;
                return true;
            }
            catch (Exception ex)
            {
                message = namespaceName + "::" + variableName + " 쓰기 실패: " + ex.Message;
                LastError = message;
                return false;
            }
        }

        public bool TryGetSystemVariable(
            string namespaceName,
            string variableName,
            out object value,
            out string message)
        {
            value = null;
            if (!TryAttach(allowStart: false, out message))
            {
                LastError = message;
                return false;
            }

            try
            {
                dynamic variable = ResolveSystemVariable(namespaceName, variableName);
                value = variable.Value;
                message = namespaceName + "::" + variableName + "=" + value;
                LastError = null;
                return true;
            }
            catch (Exception ex)
            {
                message = namespaceName + "::" + variableName + " read failed: " + ex.Message;
                LastError = message;
                return false;
            }
        }

        public bool TryGetSystemVariableData(
            string namespaceName,
            string variableName,
            out byte[] data,
            out string message)
        {
            data = null;
            if (!TryAttach(allowStart: false, out message))
            {
                LastError = message;
                return false;
            }

            try
            {
                dynamic variable = ResolveSystemVariable(namespaceName, variableName);
                object value = variable.Value;
                byte[] bytes = value as byte[];
                if (bytes != null)
                {
                    data = (byte[])bytes.Clone();
                }
                else
                {
                    Array values = value as Array;
                    if (values == null)
                        throw new InvalidOperationException("data system variable did not return an array");
                    data = new byte[values.Length];
                    int index = 0;
                    foreach (object item in values)
                        data[index++] = unchecked((byte)Convert.ToInt32(item));
                }
                message = namespaceName + "::" + variableName + " (" + data.Length + " bytes)";
                LastError = null;
                return true;
            }
            catch (Exception ex)
            {
                RemoveCachedSystemVariable(namespaceName, variableName);
                message = namespaceName + "::" + variableName + " data read failed: " + ex.Message;
                LastError = message;
                return false;
            }
        }

        public bool TrySendCanCommand(byte[] data, out string message)
        {
            if (data == null || data.Length != 8)
            {
                message = "CAN LED command must contain exactly 8 bytes.";
                LastError = message;
                return false;
            }

            if (!TryAttach(allowStart: false, out message))
                return false;

            try
            {
                lock (_canSendLock)
                {
                    dynamic request = ResolveSystemVariable(CanNamespace, "SendRequest");
                    dynamic txCount = ResolveSystemVariable(CanNamespace, "TxCount");
                    Stopwatch ready = Stopwatch.StartNew();
                    while (Convert.ToInt32(request.Value) != 0 && ready.ElapsedMilliseconds < CanSendConfirmTimeoutMs)
                        Thread.Sleep(1);
                    if (Convert.ToInt32(request.Value) != 0)
                        throw new TimeoutException("previous CANoe send request was not completed");

                    int previousCount = Convert.ToInt32(txCount.Value);
                    for (int i = 0; i < data.Length; i++)
                        ResolveSystemVariable(CanNamespace, "Data" + i).Value = data[i];
                    request.Value = 1;

                    Stopwatch confirm = Stopwatch.StartNew();
                    while (Convert.ToInt32(txCount.Value) == previousCount && confirm.ElapsedMilliseconds < CanSendConfirmTimeoutMs)
                        Thread.Sleep(1);
                    if (Convert.ToInt32(txCount.Value) == previousCount)
                        throw new TimeoutException("CANoe did not confirm the CAN transmit request");

                    message = "CAN EXT 0x18FFFF02 sent (DLC 8)";
                    LastError = null;
                    return true;
                }
            }
            catch (Exception ex)
            {
                message = "CAN command send failed: " + ex.Message;
                LastError = message;
                return false;
            }
        }

        public bool TryConfigureAd3304Panel(
            int fid,
            int node,
            int targetPath,
            int firstNode,
            int lastNode,
            int shiftWidth,
            int effectPeriodMs,
            int r,
            int g,
            int b,
            int brightnessPercent,
            out string message)
        {
            int clampedTargetPath = Clamp(targetPath, 0, 2);
            int maxNode = clampedTargetPath == 1 ? 25 : 84;

            if (!TrySetPanelVariable("Fid", Clamp(fid, 0, 8), out message)) return false;
            if (!TrySetPanelVariable("TargetPath", clampedTargetPath, out message)) return false;
            if (!TrySetPanelVariable("Node", Clamp(node, 0, maxNode), out message)) return false;
            if (!TrySetPanelVariable("FirstNode", Clamp(firstNode, 1, maxNode), out message)) return false;
            if (!TrySetPanelVariable("LastNode", Clamp(lastNode, 1, maxNode), out message)) return false;
            if (!TrySetPanelVariable("ShiftWidth", Clamp(shiftWidth, 1, maxNode), out message)) return false;
            if (!TrySetPanelVariable("EffectPeriodMs", Clamp(effectPeriodMs, 1, 10000), out message)) return false;
            if (!TrySetPanelVariable("R", Clamp(r, 0, 255), out message)) return false;
            if (!TrySetPanelVariable("G", Clamp(g, 0, 255), out message)) return false;
            if (!TrySetPanelVariable("B", Clamp(b, 0, 255), out message)) return false;
            if (!TrySetPanelVariable("Brightness", Clamp(brightnessPercent, 0, 100), out message)) return false;
            if (!TrySetPanelVariable("AutoSend", 0, out message)) return false;

            message = "AD3304 panel variables configured";
            LastError = null;
            return true;
        }

        public bool TryStartEffect(int mode, out string message)
        {
            int clampedMode = Clamp(mode, 1, 16);
            if (TrySetPanelVariable("Command", clampedMode, out message))
                return true;

            if (clampedMode == 10)
                return TrySetPanelVariable("BtnReadback", 1, out message);

            return false;
        }

        public bool TryStopEffect(out string message)
        {
            string firstMessage;
            TrySetPanelVariable("EffectRunning", 0, out firstMessage);

            if (TrySetPanelVariable("Command", 14, out message))
                return true;

            if (string.IsNullOrEmpty(message))
                message = firstMessage;
            return false;
        }

        private bool TryEnsurePanelConfiguration(out string message)
        {
            try
            {
                ResolveSystemVariable(PanelNamespace, "AutoSend");
                ResolveSystemVariable(PanelNamespace, "BtnInit");
                message = "AD3304LED25 system variables ready";
                return true;
            }
            catch (Exception firstError)
            {
                string currentConfiguration = GetCurrentConfigurationPath();
                bool noConfigurationLoaded = string.IsNullOrWhiteSpace(currentConfiguration)
                    || !File.Exists(currentConfiguration);

                if (!_startedByBridge && !noConfigurationLoaded)
                {
                    message = "현재 CANoe configuration에 AD3304LED25 시스템 변수가 없습니다.\r\n" +
                              "현재: " + currentConfiguration + "\r\n" +
                              "대상: " + TargetConfigurationPath + "\r\n" +
                              "상세: " + firstError.Message;
                    LastError = message;
                    return false;
                }

                if (!File.Exists(TargetConfigurationPath))
                {
                    message = "대상 CANoe configuration을 찾을 수 없습니다: " + TargetConfigurationPath;
                    LastError = message;
                    return false;
                }

                try
                {
                    _application.Open(TargetConfigurationPath, false, false);
                    ClearSystemVariableCache();
                }
                catch (Exception ex)
                {
                    message = "CANoe configuration 열기 실패: " + ex.Message;
                    LastError = message;
                    return false;
                }

                Stopwatch wait = Stopwatch.StartNew();
                Exception lastError = firstError;
                while (wait.ElapsedMilliseconds < CANoeReadyTimeoutMs)
                {
                    try
                    {
                        ResolveSystemVariable(PanelNamespace, "AutoSend");
                        ResolveSystemVariable(PanelNamespace, "BtnInit");
                        message = "loaded " + TargetConfigurationPath;
                        LastError = null;
                        return true;
                    }
                    catch (Exception ex)
                    {
                        lastError = ex;
                        Thread.Sleep(100);
                    }
                }

                message = "CANoe configuration을 열었지만 AD3304LED25 시스템 변수를 찾지 못했습니다: " +
                          lastError.Message;
                LastError = message;
                return false;
            }
        }

        private bool TryEnsureCanConfiguration(out string message)
        {
            string currentConfiguration = GetCurrentConfigurationPath();
            try
            {
                ResolveSystemVariable(CanNamespace, "Ready");
                ResolveSystemVariable(CanNamespace, "SendRequest");
                message = "using open CANoe configuration: " + currentConfiguration;
                LastError = null;
                return true;
            }
            catch
            {
                ClearSystemVariableCache();
            }

            string targetConfiguration = TargetCanConfigurationPath;
            if (string.IsNullOrWhiteSpace(targetConfiguration) || !File.Exists(targetConfiguration))
            {
                message = "Sunshine_CAN.cfg를 자동으로 찾지 못했습니다.\r\n" +
                          CanConfigurationPathFile + " 파일에 cfg 절대경로를 한 줄로 입력하거나,\r\n" +
                          CanConfigurationEnvironmentVariable + " 환경변수에 cfg 경로를 지정하세요.";
                LastError = message;
                return false;
            }

            bool targetLoaded = !string.IsNullOrWhiteSpace(currentConfiguration)
                && string.Equals(
                    Path.GetFullPath(currentConfiguration),
                    Path.GetFullPath(targetConfiguration),
                    StringComparison.OrdinalIgnoreCase);

            if (!targetLoaded)
            {
                try
                {
                    dynamic measurement = _application.Measurement;
                    if ((bool)measurement.Running)
                    {
                        measurement.Stop();
                        Stopwatch stopWait = Stopwatch.StartNew();
                        while ((bool)measurement.Running
                            && stopWait.ElapsedMilliseconds < CANoeReadyTimeoutMs)
                            Thread.Sleep(100);
                    }

                    _application.Open(targetConfiguration, false, false);
                    ClearSystemVariableCache();
                }
                catch (Exception ex)
                {
                    message = "CANoe CAN configuration open failed: " + ex.Message;
                    LastError = message;
                    return false;
                }
            }

            Stopwatch wait = Stopwatch.StartNew();
            Exception lastError = null;
            while (wait.ElapsedMilliseconds < CANoeReadyTimeoutMs)
            {
                try
                {
                    ResolveSystemVariable(CanNamespace, "Ready");
                    ResolveSystemVariable(CanNamespace, "SendRequest");
                    message = "loaded " + targetConfiguration;
                    LastError = null;
                    return true;
                }
                catch (Exception ex)
                {
                    lastError = ex;
                    Thread.Sleep(100);
                }
            }

            message = "CANoe CAN system variables were not found: " +
                      (lastError == null ? "unknown error" : lastError.Message);
            LastError = message;
            return false;
        }

        private static string FindCanConfigurationPath()
        {
            string environmentPath = NormalizeConfigurationPath(
                Environment.GetEnvironmentVariable(CanConfigurationEnvironmentVariable), null);
            if (File.Exists(environmentPath)) return environmentPath;

            string[] pathFiles =
            {
                CanConfigurationPathFile,
                Path.Combine(Environment.CurrentDirectory, CanConfigurationPathFileName),
                Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                    "SunshineControl", CanConfigurationPathFileName)
            };
            foreach (string pathFile in pathFiles)
            {
                try
                {
                    if (!File.Exists(pathFile)) continue;
                    string configuredPath = NormalizeConfigurationPath(
                        File.ReadAllText(pathFile), Path.GetDirectoryName(pathFile));
                    if (File.Exists(configuredPath)) return configuredPath;
                }
                catch { }
            }

            string found = FindCanConfigurationNear(AppDomain.CurrentDomain.BaseDirectory);
            if (File.Exists(found)) return found;

            found = FindCanConfigurationNear(Environment.CurrentDirectory);
            if (File.Exists(found)) return found;

            string desktopPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory),
                @"jhhan\vector\Sunshine_CAN_VN5650_20260824\Sunshine_CAN.cfg");
            return File.Exists(desktopPath) ? Path.GetFullPath(desktopPath) : null;
        }

        private static string FindCanConfigurationNear(string startPath)
        {
            try
            {
                DirectoryInfo directory = new DirectoryInfo(startPath);
                for (int depth = 0; directory != null && depth < 8; depth++, directory = directory.Parent)
                {
                    string[] candidates =
                    {
                        Path.Combine(directory.FullName, CanConfigurationFileName),
                        Path.Combine(directory.FullName, "Sunshine_CAN_VN5650_20260824", CanConfigurationFileName),
                        Path.Combine(directory.FullName, "vector", "Sunshine_CAN_VN5650_20260824", CanConfigurationFileName)
                    };
                    foreach (string candidate in candidates)
                        if (File.Exists(candidate)) return Path.GetFullPath(candidate);
                }
            }
            catch { }
            return null;
        }

        private static string NormalizeConfigurationPath(string path, string basePath)
        {
            if (string.IsNullOrWhiteSpace(path)) return null;
            try
            {
                string value = Environment.ExpandEnvironmentVariables(path.Trim().Trim('"'));
                if (!Path.IsPathRooted(value) && !string.IsNullOrWhiteSpace(basePath))
                    value = Path.Combine(basePath, value);
                return Path.GetFullPath(value);
            }
            catch
            {
                return null;
            }
        }

        private bool TryEnsureMeasurementRunning(out string message)
        {
            try
            {
                dynamic measurement = _application.Measurement;
                if ((bool)measurement.Running)
                {
                    message = "CANoe measurement already running";
                    return true;
                }

                measurement.Start();
                Stopwatch wait = Stopwatch.StartNew();
                while (wait.ElapsedMilliseconds < CANoeReadyTimeoutMs)
                {
                    if ((bool)measurement.Running)
                    {
                        message = "CANoe measurement started";
                        LastError = null;
                        return true;
                    }
                    Thread.Sleep(100);
                }

                message = "CANoe Measurement 시작 시간 초과. CAPL compile 오류와 CANoe Write 창을 확인하세요.";
                LastError = message;
                return false;
            }
            catch (Exception ex)
            {
                message = "CANoe Measurement 시작 실패: " + ex.Message;
                LastError = message;
                return false;
            }
        }

        private dynamic ResolveSystemVariable(string namespaceName, string variableName)
        {
            string key = namespaceName + "::" + variableName;
            object cached;
            lock (_systemVariableCacheLock)
            {
                if (_systemVariableCache.TryGetValue(key, out cached))
                    return cached;
            }
            dynamic system = _application.System;
            object resolved = null;

            try { resolved = system.Namespaces.Item(namespaceName).Variables.Item(variableName); }
            catch { }

            if (resolved == null)
            {
                try { resolved = system.Namespaces[namespaceName].Variables[variableName]; }
                catch { }
            }

            if (resolved == null)
            {
                try { resolved = system.GetVariable(key); }
                catch { }
            }

            if (resolved == null)
                throw new InvalidOperationException("system variable not found");

            lock (_systemVariableCacheLock)
                _systemVariableCache[key] = resolved;
            return resolved;
        }

        private void RemoveCachedSystemVariable(string namespaceName, string variableName)
        {
            lock (_systemVariableCacheLock)
                _systemVariableCache.Remove(namespaceName + "::" + variableName);
        }

        private void ClearSystemVariableCache()
        {
            lock (_systemVariableCacheLock)
                _systemVariableCache.Clear();
        }

        private string GetCurrentConfigurationPath()
        {
            try
            {
                dynamic configuration = _application.Configuration;
                return Convert.ToString(configuration.FullName);
            }
            catch
            {
                return null;
            }
        }

        private static string GetRegisteredServerPath()
        {
            try
            {
                using (RegistryKey key = Registry.ClassesRoot.OpenSubKey(
                    @"CLSID\" + ApplicationClsid + @"\LocalServer32"))
                {
                    string command = key == null ? null : Convert.ToString(key.GetValue(null));
                    if (string.IsNullOrWhiteSpace(command)) return null;

                    command = command.Trim();
                    if (command.StartsWith("\"", StringComparison.Ordinal))
                    {
                        int closingQuote = command.IndexOf('\"', 1);
                        return closingQuote > 1 ? command.Substring(1, closingQuote - 1) : command.Trim('\"');
                    }

                    int exeEnd = command.IndexOf(".exe", StringComparison.OrdinalIgnoreCase);
                    return exeEnd >= 0 ? command.Substring(0, exeEnd + 4) : command;
                }
            }
            catch
            {
                return null;
            }
        }

        private static bool IsCANoe20Registration(string serverPath)
        {
            return !string.IsNullOrWhiteSpace(serverPath)
                && File.Exists(serverPath)
                && serverPath.IndexOf("Vector CANoe 20", StringComparison.OrdinalIgnoreCase) >= 0;
        }

        private static string BuildRegistrationMessage(string reason)
        {
            string registerTool = FindCANoe20RegisterTool();
            return reason + "\r\n" +
                   "CANoe 20을 닫은 뒤 관리자 권한으로 COM 서버를 등록하세요:\r\n" +
                   (string.IsNullOrWhiteSpace(registerTool)
                       ? @"C:\Program Files\Vector CANoe 20\Exec64\RegisterComponents.exe"
                       : registerTool);
        }

        private static string FindCANoe20RegisterTool()
        {
            string programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
            string expected = Path.Combine(
                programFiles, "Vector CANoe 20", "Exec64", "RegisterComponents.exe");
            if (File.Exists(expected)) return expected;

            try
            {
                return Directory.GetDirectories(programFiles, "Vector CANoe 20*")
                    .OrderByDescending(path => path)
                    .Select(path => Path.Combine(path, "Exec64", "RegisterComponents.exe"))
                    .FirstOrDefault(File.Exists);
            }
            catch
            {
                return null;
            }
        }

        private void ReleaseApplication()
        {
            global::CANoe.Application application = _application;
            _application = null;
            _startedByBridge = false;
            ClearSystemVariableCache();

            if (application == null || !Marshal.IsComObject(application)) return;
            try { Marshal.FinalReleaseComObject(application); }
            catch { }
        }

        private static int Clamp(int value, int min, int max)
        {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }
    }
}
