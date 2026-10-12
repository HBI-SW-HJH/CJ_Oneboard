using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LINMaster
{
    // =========================================================================
    // LINWebServer – 모바일 앱을 위한 내장 HTTP/WebSocket 서버
    //
    // [동작 방식]
    //   PC: LINMaster.exe 실행 → "🌐 서버 시작" 버튼 클릭
    //       → 화면에 URL 표시 (예: http://192.168.1.100:7000)
    //   모바일: 브라우저에서 해당 URL 접속
    //           → 별도 앱 설치 없이 LIN Master 인터페이스 사용
    //
    // [API]
    //   GET  /          → 모바일 웹 앱 HTML
    //   GET  /api/status → 현재 연결 상태
    //   GET  /api/ports  → 포트/장치 목록
    //   POST /api/connect    → LIN 연결
    //   POST /api/disconnect → LIN 해제
    //   POST /api/send       → LIN 프레임 송신
    //   WS   /ws        → 실시간 로그 스트리밍
    // =========================================================================
    public class LINWebServer : IDisposable
    {
        public const int DEFAULT_PORT = 7000;

        // ── FormMain 에서 주입하는 콜백 delegates ─────────────────────────
        public struct ConnectResult { public bool OK; public string Error; }
        public struct SendResult    { public bool OK; public string Error; }

        public Func<bool>                                     GetConnected;
        public Func<string>                                   GetDeviceInfo;  // "IndieMicro COM4" 등
        public Func<string, string[]>                         GetPorts;       // iface → ports
        public Func<string, string, int, ConnectResult>       DoConnect;      // iface, port, baud
        public Action                                         DoDisconnect;
        public Func<string, bool, string, int, string, SendResult> DoSend;    // id, sub, cs, len, data

        public event Action<string> OnStatus;
        public event Action<string> OnError;

        // ── 내부 ──────────────────────────────────────────────────────────
        private HttpListener            _listener;
        private List<WebSocket>         _wsClients = new List<WebSocket>();
        private readonly object         _wsLock    = new object();
        private CancellationTokenSource _cts;
        private readonly int            _port;
        private readonly string         _localIp;
#pragma warning disable CS0414
        private bool                    _externalAccess;
#pragma warning restore CS0414
        private bool                    _disposed;

        public string ServerUrl  => $"http://{_localIp}:{_port}";
        public bool   IsRunning  => _listener != null && _listener.IsListening;

        // ── 생성자 ────────────────────────────────────────────────────────
        public LINWebServer(int port = DEFAULT_PORT)
        {
            _port    = port;
            _localIp = GetLocalIP();
        }

        private static string GetLocalIP()
        {
            try
            {
                using (var s = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp))
                {
                    s.Connect("8.8.8.8", 80);
                    return ((IPEndPoint)s.LocalEndPoint).Address.ToString();
                }
            }
            catch { return "127.0.0.1"; }
        }

        // ── 서버 시작 / 중지 ──────────────────────────────────────────────
        public bool Start()
        {
            if (IsRunning) return true;
            _cts = new CancellationTokenSource();

            // + 프리픽스: 모바일에서 외부 접속 가능 (netsh 또는 관리자 권한 필요)
            if (TryStart($"http://+:{_port}/"))
            {
                _externalAccess = true;
                OnStatus?.Invoke($"[모바일 서버] 시작됨 → http://{_localIp}:{_port}");
            }
            else if (TryStart($"http://localhost:{_port}/"))
            {
                _externalAccess = false;
                OnStatus?.Invoke($"[모바일 서버] localhost 전용 시작 (외부 접속 불가)\n" +
                                  "외부 접속을 허용하려면 관리자 권한으로 실행하거나\n" +
                                  $"CMD(관리자): netsh http add urlacl url=http://+:{_port}/ user=Everyone");
            }
            else
            {
                OnError?.Invoke("[모바일 서버] 포트 바인딩 실패");
                return false;
            }

            Task.Run(() => AcceptLoop(_cts.Token));
            return true;
        }

        private bool TryStart(string prefix)
        {
            try
            {
                _listener = new HttpListener();
                _listener.Prefixes.Add(prefix);
                _listener.Start();
                return true;
            }
            catch
            {
                try { _listener?.Stop(); } catch { }
                _listener = null;
                return false;
            }
        }

        public void Stop()
        {
            _cts?.Cancel();
            try { _listener?.Stop(); } catch { }
            _listener = null;
            lock (_wsLock) { foreach (var ws in _wsClients) try { ws.Abort(); } catch { } _wsClients.Clear(); }
            OnStatus?.Invoke("[모바일 서버] 중지됨");
        }

        // ── Accept Loop ───────────────────────────────────────────────────
        private async Task AcceptLoop(CancellationToken ct)
        {
            while (!ct.IsCancellationRequested && IsRunning)
            {
                try
                {
                    var ctx = await _listener.GetContextAsync();
                    _ = Task.Run(() => HandleContext(ctx), ct);
                }
                catch when (ct.IsCancellationRequested) { break; }
                catch (Exception ex)
                {
                    if (!ct.IsCancellationRequested)
                    { OnError?.Invoke("[서버 오류] " + ex.Message); await Task.Delay(1000); }
                }
            }
        }

        private async Task HandleContext(HttpListenerContext ctx)
        {
            try
            {
                if (ctx.Request.IsWebSocketRequest)
                {
                    var ws = await ctx.AcceptWebSocketAsync(null);
                    await HandleWebSocket(ws.WebSocket);
                }
                else HandleHttp(ctx);
            }
            catch { try { ctx.Response.Abort(); } catch { } }
        }

        // ── WebSocket 처리 ────────────────────────────────────────────────
        private async Task HandleWebSocket(WebSocket ws)
        {
            lock (_wsLock) _wsClients.Add(ws);
            try
            {
                // 연결 시 현재 상태 전송
                bool conn  = GetConnected?.Invoke() ?? false;
                string info = GetDeviceInfo?.Invoke() ?? "";
                await WsSend(ws, $"{{\"type\":\"status\",\"connected\":{JsBool(conn)},\"port\":{JsStr(info)}}}");

                // keep-alive: 클라이언트 닫을 때까지 대기
                var buf = new byte[256];
                while (ws.State == WebSocketState.Open)
                {
                    var r = await ws.ReceiveAsync(new ArraySegment<byte>(buf), CancellationToken.None);
                    if (r.MessageType == WebSocketMessageType.Close)
                        await ws.CloseAsync(WebSocketCloseStatus.NormalClosure, "", CancellationToken.None);
                }
            }
            catch { }
            lock (_wsLock) _wsClients.Remove(ws);
            try { ws.Dispose(); } catch { }
        }

        private static async Task WsSend(WebSocket ws, string msg)
        {
            if (ws.State != WebSocketState.Open) return;
            var b = Encoding.UTF8.GetBytes(msg);
            await ws.SendAsync(new ArraySegment<byte>(b), WebSocketMessageType.Text, true, CancellationToken.None);
        }

        // ── 브로드캐스트 (FormMain.AppLog 에서 호출) ──────────────────────
        public void BroadcastLog(string text, string colorHex, bool isRaw = false)
        {
            string msg = $"{{\"type\":\"log\",\"text\":{JsStr(text)},\"color\":{JsStr(colorHex)},\"raw\":{JsBool(isRaw)}}}";
            BroadcastAll(msg);
        }

        public void BroadcastStatus(bool connected, string port, string iface)
        {
            string msg = $"{{\"type\":\"status\",\"connected\":{JsBool(connected)},\"port\":{JsStr(port)},\"iface\":{JsStr(iface)}}}";
            BroadcastAll(msg);
        }

        private void BroadcastAll(string msg)
        {
            List<WebSocket> snap;
            lock (_wsLock) snap = new List<WebSocket>(_wsClients);
            foreach (var ws in snap)
                if (ws.State == WebSocketState.Open) _ = WsSend(ws, msg);
        }

        // ── HTTP 처리 ──────────────────────────────────────────────────────
        private void HandleHttp(HttpListenerContext ctx)
        {
            var req = ctx.Request; var resp = ctx.Response;
            resp.Headers.Add("Access-Control-Allow-Origin", "*");
            resp.Headers.Add("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
            resp.Headers.Add("Access-Control-Allow-Headers", "Content-Type");

            if (req.HttpMethod == "OPTIONS") { resp.StatusCode = 200; resp.Close(); return; }

            string path = req.Url.AbsolutePath.TrimEnd('/');
            if (string.IsNullOrEmpty(path)) path = "/";

            try
            {
                switch (req.HttpMethod + " " + path)
                {
                    case "GET /":
                        SendHtml(resp, GetMobileHtml()); break;

                    case "GET /api/status":
                    {
                        bool c = GetConnected?.Invoke() ?? false;
                        string info = GetDeviceInfo?.Invoke() ?? "";
                        SendJson(resp, $"{{\"connected\":{JsBool(c)},\"port\":{JsStr(info)}}}"); break;
                    }
                    case "GET /api/ports":
                    {
                        string iface = req.QueryString["iface"] ?? "IndieMicro";
                        string[] ports = GetPorts?.Invoke(iface) ?? new string[0];
                        SendJson(resp, $"{{\"ports\":[{string.Join(",", Array.ConvertAll(ports, JsStr))}]}}"); break;
                    }
                    case "POST /api/connect":
                    {
                        string body  = ReadBody(req);
                        string iface = ParseJson(body, "iface") ?? "IndieMicro";
                        string port  = ParseJson(body, "port")  ?? "";
                        int    baud  = int.TryParse(ParseJson(body, "baud"), out int b) ? b : 19200;
                        var    res   = DoConnect?.Invoke(iface, port, baud) ?? new ConnectResult { OK = false, Error = "No handler" };
                        bool   conn  = GetConnected?.Invoke() ?? false;
                        string info  = GetDeviceInfo?.Invoke() ?? "";
                        if (res.OK)
                            SendJson(resp, $"{{\"ok\":true,\"connected\":true,\"port\":{JsStr(info)},\"iface\":{JsStr(iface)}}}");
                        else
                            SendJson(resp, $"{{\"ok\":false,\"connected\":false,\"error\":{JsStr(res.Error ?? "연결 실패")}}}");
                        break;
                    }
                    case "POST /api/disconnect":
                        DoDisconnect?.Invoke();
                        SendJson(resp, "{\"ok\":true,\"connected\":false}"); break;

                    case "POST /api/send":
                    {
                        string body = ReadBody(req);
                        string id   = ParseJson(body, "id")   ?? "01";
                        string cs   = ParseJson(body, "cs")   ?? "Enhanced";
                        string data = ParseJson(body, "data") ?? "";
                        int    len  = int.TryParse(ParseJson(body, "len"), out int l) ? l : 8;
                        bool   sub  = ParseJson(body, "subscriber") == "true";
                        var    res  = DoSend?.Invoke(id, sub, cs, len, data) ?? new SendResult { OK = false, Error = "No handler" };
                        SendJson(resp, res.OK ? "{\"ok\":true}" : $"{{\"ok\":false,\"error\":{JsStr(res.Error ?? "전송 실패")}}}");
                        break;
                    }
                    default:
                        resp.StatusCode = 404;
                        SendJson(resp, "{\"error\":\"Not Found\"}"); break;
                }
            }
            catch (Exception ex) { resp.StatusCode = 500; SendJson(resp, $"{{\"error\":{JsStr(ex.Message)}}}"); }
        }

        private static void SendHtml(HttpListenerResponse r, string html)
        {
            var b = Encoding.UTF8.GetBytes(html);
            r.ContentType = "text/html; charset=utf-8"; r.ContentLength64 = b.Length;
            r.OutputStream.Write(b, 0, b.Length); r.Close();
        }
        private static void SendJson(HttpListenerResponse r, string json)
        {
            var b = Encoding.UTF8.GetBytes(json);
            r.ContentType = "application/json; charset=utf-8"; r.ContentLength64 = b.Length;
            r.OutputStream.Write(b, 0, b.Length); r.Close();
        }
        private static string ReadBody(HttpListenerRequest req)
        {
            if (!req.HasEntityBody) return "";
            using (var sr = new System.IO.StreamReader(req.InputStream, req.ContentEncoding))
                return sr.ReadToEnd();
        }

        // ── JSON 헬퍼 ─────────────────────────────────────────────────────
        private static string JsStr(string s)
            => "\"" + (s ?? "").Replace("\\","\\\\").Replace("\"","\\\"").Replace("\r","\\r").Replace("\n","\\n") + "\"";
        private static string JsBool(bool v) => v ? "true" : "false";

        private static string ParseJson(string json, string key)
        {
            if (string.IsNullOrEmpty(json)) return null;
            int idx = json.IndexOf("\"" + key + "\"");
            if (idx < 0) return null;
            idx += key.Length + 2;
            while (idx < json.Length && (json[idx] == ' ' || json[idx] == ':')) idx++;
            if (idx >= json.Length) return null;
            if (json[idx] == '"')
            {
                idx++; var sb = new StringBuilder();
                while (idx < json.Length && json[idx] != '"')
                {
                    if (json[idx] == '\\' && idx+1 < json.Length) { idx++; sb.Append(json[idx]); }
                    else sb.Append(json[idx]);
                    idx++;
                }
                return sb.ToString();
            }
            int end = idx;
            while (end < json.Length && json[end] != ',' && json[end] != '}') end++;
            return json.Substring(idx, end-idx).Trim();
        }

        // ── 모바일 웹 앱 HTML ──────────────────────────────────────────────
        private static string GetMobileHtml() => @"<!DOCTYPE html>
<html lang='ko'>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>
<meta name='apple-mobile-web-app-capable' content='yes'>
<meta name='mobile-web-app-capable' content='yes'>
<title>LIN Master</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
:root{--bg:#1a1a26;--bg2:#222234;--bg3:#2a2a3e;--acc:#78c8ff;--grn:#78f08c;--red:#ff6464;--ylw:#ffc850;--txt:#e0e8f0;--txt2:#a0b0c0;--bdr:#3a3a56}
html,body{height:100%;background:var(--bg);color:var(--txt);font-family:Consolas,monospace;font-size:14px}
#app{display:flex;flex-direction:column;height:100%;max-width:500px;margin:0 auto}
header{background:var(--bg2);border-bottom:1px solid var(--bdr);padding:10px 14px;display:flex;align-items:center;justify-content:space-between;flex-shrink:0}
header h1{color:var(--acc);font-size:14px;letter-spacing:.5px}
#sdot{width:10px;height:10px;border-radius:50%;background:#555;transition:.3s}
#sdot.on{background:var(--grn);box-shadow:0 0 6px var(--grn)}
main{flex:1;overflow:hidden;position:relative}
.tab{display:none;flex-direction:column;height:100%;overflow-y:auto;padding:12px;gap:10px}
.tab.active{display:flex}
.card{background:var(--bg2);border:1px solid var(--bdr);border-radius:8px;padding:12px}
.ctit{color:var(--acc);font-size:11px;font-weight:700;margin-bottom:10px;letter-spacing:1px}
.row{display:flex;align-items:center;gap:8px;margin-bottom:8px}
.lbl{color:var(--txt2);font-size:12px;white-space:nowrap;min-width:76px}
input,select{background:var(--bg3);border:1px solid var(--bdr);color:var(--txt);padding:6px 10px;border-radius:4px;font-family:inherit;font-size:13px;flex:1;min-width:0}
input:focus,select:focus{outline:none;border-color:var(--acc)}
.rdg{display:flex;gap:6px;flex:1}
.rdb{flex:1;text-align:center;padding:6px;border:1px solid var(--bdr);border-radius:4px;cursor:pointer;color:var(--txt2);font-size:12px;background:var(--bg3);transition:.15s;user-select:none}
.rdb.sel{border-color:var(--acc);color:var(--acc);background:rgba(120,200,255,.12)}
.btn{width:100%;padding:10px;border:none;border-radius:6px;font-family:inherit;font-size:14px;font-weight:700;cursor:pointer;transition:opacity .15s}
.btn:active{opacity:.7}
.bc{background:#326c3c;color:#fff}
.bd{background:#7a2828;color:#fff}
.bs{background:#1e5ec8;color:#fff;flex:1}
.br{background:#7a5a1e;color:#fff;flex:1}
.btnr{background:var(--bg3);border:1px solid var(--bdr);color:var(--txt2);font-size:12px;padding:5px 10px;border-radius:4px;cursor:pointer;white-space:nowrap;font-family:inherit}
.srow{display:flex;gap:6px}
.smsg{border-radius:4px;padding:6px 10px;font-size:12px;margin-bottom:8px;color:var(--txt2);background:var(--bg3)}
.smsg.ok{color:var(--grn)}
.smsg.err{color:var(--red)}
#logw{font-size:12px;line-height:1.7;padding:2px 0;min-height:250px;overflow-y:auto;max-height:calc(100vh - 200px)}
#logw div{padding:1px 0;word-break:break-all}
.lbar{display:flex;align-items:center;justify-content:space-between;margin-bottom:8px}
nav{display:flex;background:var(--bg2);border-top:1px solid var(--bdr);flex-shrink:0}
.nb{flex:1;padding:10px 4px 8px;text-align:center;cursor:pointer;color:var(--txt2);font-size:10px;border:none;background:transparent;font-family:inherit;transition:color .15s}
.nb.active{color:var(--acc)}
.nb svg{display:block;margin:0 auto 2px}
#baudrow{display:none}
</style>
</head>
<body>
<div id='app'>
<header>
  <h1>⚡ LIN Master Mobile</h1>
  <div style='display:flex;align-items:center;gap:8px'>
    <span id='stxt' style='font-size:11px;color:#666'>연결 안됨</span>
    <div id='sdot'></div>
  </div>
</header>
<main>
<div class='tab active' id='t-conn'>
  <div class='card'>
    <div class='ctit'>[ 인터페이스 ]</div>
    <div class='row'><span class='lbl'>인터페이스:</span>
      <div class='rdg'>
        <div class='rdb sel' id='ri' onclick='setIface(""IndieMicro"")'>IndieMicro</div>
        <div class='rdb' id='rp' onclick='setIface(""Peak"")'>PEAK PLIN</div>
      </div>
    </div>
    <div class='row'><span class='lbl' id='plbl'>COM Port:</span>
      <select id='sport'></select>
      <button class='btnr' onclick='loadPorts()'>↺</button>
    </div>
    <div class='row' id='baudrow'><span class='lbl'>LIN Baud:</span>
      <select id='sbaud'>
        <option>2400</option><option>9600</option>
        <option selected>19200</option><option>20000</option>
      </select>
    </div>
    <div class='smsg' id='smsg'>연결 안됨</div>
    <button class='btn bc' id='bcn' onclick='doConn()'>연결</button>
  </div>
</div>
<div class='tab' id='t-tx'>
  <div class='card'>
    <div class='ctit'>[ 수동 TX / RX 요청 ]</div>
    <div class='row'><span class='lbl'>Frame ID:</span><input id='tid' value='02' maxlength='4' style='max-width:80px'></div>
    <div class='row'><span class='lbl'>Frame Type:</span>
      <select id='ttype' onchange='onTC()'>
        <option value='Publisher'>Publisher (전송)</option>
        <option value='Subscriber'>Subscriber (요청)</option>
      </select>
    </div>
    <div class='row'><span class='lbl'>Checksum:</span>
      <select id='tcs'><option value='Classic'>Classic</option><option value='Enhanced' selected>Enhanced</option></select>
    </div>
    <div class='row'><span class='lbl'>Data Length:</span>
      <input id='tlen' type='number' value='8' min='1' max='8' style='max-width:60px'>
    </div>
    <div class='row' id='tdrow'><span class='lbl'>Data (HEX):</span>
      <input id='tdata' value='FF 65 3A FA 01 01 FF 7F'>
    </div>
    <div class='srow'>
      <button class='btn bs' id='bsend' onclick='doSend()'>SEND (Publisher)</button>
      <button class='btn br' id='breq' style='display:none' onclick='doReq()'>REQUEST (Sub)</button>
    </div>
  </div>
</div>
<div class='tab' id='t-log'>
  <div class='card' style='flex:1;display:flex;flex-direction:column'>
    <div class='lbar'>
      <div class='ctit' style='margin:0'>[ TX / RX 로그 ]</div>
      <div style='display:flex;gap:6px;align-items:center'>
        <label style='font-size:11px;color:var(--txt2);display:flex;align-items:center;gap:4px'>
          <input type='checkbox' id='craw'> RAW
        </label>
        <button class='btnr' onclick='clrLog()'>지우기</button>
      </div>
    </div>
    <div id='logw'></div>
  </div>
</div>
</main>
<nav>
  <button class='nb active' id='n-conn' onclick='showTab(""conn"")'>
    <svg width='18' height='18' viewBox='0 0 24 24' fill='none' stroke='currentColor' stroke-width='2'><circle cx='12' cy='12' r='3'/><path d='M19.5 7A9 9 0 0 0 4.5 7'/><path d='M21 4.5A12 12 0 0 0 3 4.5'/></svg>연결
  </button>
  <button class='nb' id='n-tx' onclick='showTab(""tx"")'>
    <svg width='18' height='18' viewBox='0 0 24 24' fill='none' stroke='currentColor' stroke-width='2'><line x1='22' y1='2' x2='11' y2='13'/><polygon points='22 2 15 22 11 13 2 9 22 2'/></svg>TX
  </button>
  <button class='nb' id='n-log' onclick='showTab(""log"")'>
    <svg width='18' height='18' viewBox='0 0 24 24' fill='none' stroke='currentColor' stroke-width='2'><path d='M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z'/><polyline points='14 2 14 8 20 8'/><line x1='16' y1='13' x2='8' y2='13'/><line x1='16' y1='17' x2='8' y2='17'/></svg>로그
  </button>
</nav>
</div>
<script>
var ws=null,conn=false,iface='IndieMicro';
function showTab(t){
  document.querySelectorAll('.tab').forEach(e=>e.classList.remove('active'));
  document.querySelectorAll('.nb').forEach(e=>e.classList.remove('active'));
  document.getElementById('t-'+t).classList.add('active');
  document.getElementById('n-'+t).classList.add('active');
  if(t==='log'){var w=document.getElementById('logw');setTimeout(()=>w.scrollTop=w.scrollHeight,50);}
}
function setIface(v){
  iface=v;
  document.getElementById('ri').classList.toggle('sel',v==='IndieMicro');
  document.getElementById('rp').classList.toggle('sel',v==='Peak');
  document.getElementById('plbl').textContent=v==='Peak'?'PEAK 장치:':'COM Port:';
  document.getElementById('baudrow').style.display=v==='Peak'?'flex':'none';
  loadPorts();
}
function loadPorts(){
  fetch('/api/ports?iface='+iface).then(r=>r.json()).then(d=>{
    var s=document.getElementById('sport');s.innerHTML='';
    (d.ports||[]).forEach(p=>{var o=document.createElement('option');o.value=p;o.text=p;s.add(o);});
    if(!d.ports||!d.ports.length){var o=document.createElement('option');o.text='(없음)';s.add(o);}
  }).catch(()=>{});
}
function doConn(){
  if(conn){fetch('/api/disconnect',{method:'POST'}).then(r=>r.json()).then(updSt);return;}
  fetch('/api/connect',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({iface:iface,port:document.getElementById('sport').value,baud:parseInt(document.getElementById('sbaud').value)||19200})
  }).then(r=>r.json()).then(d=>{updSt(d);if(!d.ok)setSMsg(d.error||'연결 실패',false);});
}
function updSt(d){
  conn=d.connected||false;
  var dot=document.getElementById('sdot'),txt=document.getElementById('stxt'),btn=document.getElementById('bcn'),msg=document.getElementById('smsg');
  dot.className=conn?'on':'';
  var pi=d.port||d.iface||'';
  txt.textContent=conn?'연결됨 – '+pi:'연결 안됨';txt.style.color=conn?'var(--grn)':'#666';
  btn.textContent=conn?'연결 해제':'연결';btn.className='btn '+(conn?'bd':'bc');
  msg.textContent=conn?'연결됨 – '+pi:'연결 안됨';msg.className='smsg'+(conn?' ok':'');
}
function setSMsg(t,ok){var e=document.getElementById('smsg');e.textContent=t;e.className='smsg'+(ok?' ok':' err');}
function onTC(){
  var p=document.getElementById('ttype').value==='Publisher';
  document.getElementById('tdrow').style.display=p?'flex':'none';
  document.getElementById('bsend').style.display=p?'block':'none';
  document.getElementById('breq').style.display=p?'none':'block';
}
function doSend(){
  fetch('/api/send',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({id:document.getElementById('tid').value,cs:document.getElementById('tcs').value,
      len:parseInt(document.getElementById('tlen').value),data:document.getElementById('tdata').value})
  }).then(r=>r.json()).then(d=>{if(!d.ok)addLog('[오류] '+(d.error||'전송 실패'),'var(--red)');});
}
function doReq(){
  fetch('/api/send',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({id:document.getElementById('tid').value,cs:document.getElementById('tcs').value,
      len:parseInt(document.getElementById('tlen').value),subscriber:true})
  }).then(r=>r.json()).then(d=>{if(!d.ok)addLog('[오류] '+(d.error||'요청 실패'),'var(--red)');});
}
function addLog(t,c){
  var w=document.getElementById('logw'),d=document.createElement('div');
  d.style.color=c||'var(--txt)';d.textContent=t;w.appendChild(d);
  if(w.children.length>600)w.removeChild(w.children[0]);
  w.scrollTop=w.scrollHeight;
}
function clrLog(){document.getElementById('logw').innerHTML='';}
function openWS(){
  try{
    ws=new WebSocket('ws://'+location.host+'/ws');
    ws.onmessage=e=>{
      try{
        var m=JSON.parse(e.data);
        if(m.type==='log'){if(m.raw&&!document.getElementById('craw').checked)return;addLog(m.text,m.color);}
        else if(m.type==='status')updSt(m);
      }catch(err){addLog(e.data,null);}
    };
    ws.onclose=()=>setTimeout(openWS,3000);
    ws.onerror=()=>ws.close();
  }catch(e){}
}
window.onload=()=>{
  fetch('/api/status').then(r=>r.json()).then(d=>{updSt(d);}).catch(()=>{});
  loadPorts();openWS();
};
</script>
</body>
</html>";

        public void Dispose() { if (!_disposed) { Stop(); _disposed = true; } }
    }
}
