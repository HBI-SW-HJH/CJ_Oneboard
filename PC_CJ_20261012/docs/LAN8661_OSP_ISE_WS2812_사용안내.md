# LAN8661 Microchip CIE 시험 안내

## 실행

`bin\Debug\LINMaster.exe`를 실행합니다.

1. 메인 화면에서 연결 방식을 `LAN8661`로 선택합니다.
2. PC IP `192.168.0.10`, 보드 IP `192.168.0.50`, RTP 포트 `5001`로 연결합니다.
3. `CIE`를 누르면 Microchip 전용 CIE 창이 열립니다.
4. `Output`에서 `전체`, `기존 LED`, `OSP LED`, `ISE LED` 중 하나를 선택합니다.
5. 기존과 같은 CIE 색상, 밝기, Shift 모드를 설정합니다.
6. `1회 TX` 또는 `주기 TX 시작`을 누릅니다.

초기 밝기는 `10%`입니다. 최솟값과 조절 단위는 `1%`입니다. 진단 테스트는 RGB 채널 최댓값을 `1/255`로 제한하며 마지막에는 OFF 데이터를 보냅니다.

연결 직후 기존 Click1에는 100개 전체 OFF 프레임을 먼저 보내 잔류 점등을 지웁니다. `1회 TX`나 `주기 TX 시작`을 누르면 타이머를 기다리지 않고 현재 색상과 밝기를 즉시 전송하므로, 밝기 값을 다시 움직여야 기존 LED가 반응하던 현상은 수정됐습니다.

## PC 연결 문제 수정

PC의 일반 Realtek 이더넷(`192.168.0.17`)과 10BASE-T1S USB 어댑터(`192.168.0.10`)가 같은 `192.168.0.x` 대역이라, Windows가 보드 패킷을 일반 이더넷으로 보내던 문제가 있었습니다. 현재 실행 파일과 Native SOME/IP DLL은 LAN8661 통신을 `192.168.0.10`에 고정하므로 관리자 권한이나 PowerShell 라우트 명령이 필요하지 않습니다.

검증 결과는 다음과 같습니다.

- `192.168.0.10`을 출발지로 지정한 보드 ping: 4/4 성공
- Click 1 제어 포트: Ready. 일반 색상 모드에서는 기존 LED 10×10, 총 100개에 같은 RGB를 전송
- Click 3 OSP RTP 네트워크 송신: 성공, 실제 OSP 점등은 아직 미확인
- Click 4 ISE 전송 포트: Ready
- 최초 검사는 Fire-and-Forget 전송 성공만 확인한 것으로, 보드 처리나 LED 점등을 입증하지 못했습니다.
- 응답을 요청하는 SPI 검사에서 `0x31`(FIFO 용량 부족)을 실제 수신했습니다. 요청 FIFO를 `960`에서 `1536`바이트로 늘린 패키지를 다운로드했습니다.
- 수정 후 기존 LED는 SPI 정상 응답 160회와 사용자의 실제 점등 확인을 모두 통과했습니다.
- ISELED/ILaS 코덱(RLC, downstream CRC-8, upstream CRC-4)과 Click4 반이중 Read를 구현했습니다.
- Click4 SPI Mode 1에서 ISE 주소 1~21에 같은 RGB `1/255` 및 OFF 전송을 완료했고, 온도 Read 4/4가 RLC/CRC 검증을 통과했습니다. 응답 주소 `27`로 연결된 체인의 응답도 확인했습니다.

기존 ADI/FT4222 코드와 기존 CIE Designer 파일은 변경하지 않았습니다. Microchip CIE도 기존 Designer 컨트롤을 그대로 사용하고 통신 코드만 연결했습니다.

## 현재 보드 구성

현재 보드에는 `OSP-CLICK1-ISE-MCUA-FIX-0907`이 적용되어 있습니다.
패키지: `D:\jhhan\LAN8661X\Firmware\LAN8661_OSP_ISE_WS2812_V1.4.0\LAN8661_OSP_CLICK1_ISE_MCUA_FIXED_20260907.mchpkg`.

- Click 1 WS2812: 사용 가능
- Click 2 WS2812: 비활성
- Click 3 OSP: `OSP LED / Click 3`을 선택하면 RTP로 색상 전송
- Click 4 ISE: SERCOM3 Mode 1, RESET/INIT/RGB/온도 Read 사용 가능

PC 프로그램은 실제로 열린 포트를 각각 표시합니다. 현재 정상 표시는 `Click1=Ready, Click2=없음, ISE전송=Ready`입니다. CIE에서는 `기존 LED / Click 1`, `OSP LED / Click 3`, `ISE LED / Click 4`를 선택해 개별 시험합니다. `기존 LED`의 일반 색상 모드는 100개 전체, 현재 Sunshine-LAN8661 구성의 `ISE LED`는 검색된 주소 1~27 전체를 같은 색으로 제어합니다. Shift 계열 모드는 의도적으로 일부 행·열만 이동 표시하므로 전체 동일 점등 시험에서는 Shift 체크를 모두 해제합니다.

## OSP / Click3 신호 연결

회로도 EV12T76A Rev4의 3/7, 6/7 시트 기준입니다. DIP의 1번 레버와 J 커넥터의 1번 핀은 서로 다른 번호입니다.

| 신호 | 연결 헤더 | 칩과 헤더를 연결하는 DIP |
|---|---|---|
| DATA / SIO_P | J8-5 (MISO-2, PA08) | SW9의 1번 ON |
| CLK / SIO_N | J8-4 (SCK-2, PA09) | SW10의 1번 ON |
| +5V | J7-7 | LED 보드의 5V 전원 입력 |
| GND | J7-8 또는 J8-8 | LED 보드 GND와 공통 |

Click3를 OSP MCU 모드로 사용할 때 DIP를 전부 ON으로 두면 안 됩니다.

- `SW9`: **1번과 5번만 ON**, 나머지는 OFF. 1번은 PA08 DATA를 J8-5에 연결하고, 5번은 SIO_P에 4.7 kΩ 풀업을 제공합니다.
- `SW10`: **1번만 ON**, 나머지는 OFF. 특히 5번은 반드시 OFF입니다. 5번까지 ON이면 SIO_N/CLK에 4.7 kΩ 풀업이 걸려 OSIRE가 MCU 모드가 아닌 CAN 모드로 선택될 수 있습니다.
- DIP 변경은 전원을 끈 상태에서 하고, 변경 후 OSP LED의 5 V를 다시 인가하여 POR 모드 선택을 다시 수행합니다.

위 두 신호는 LAN8661의 `OsireDisplayPort`가 OSIRE LED 체인에 직접 출력하는 OSP 쌍입니다. ISE의 CLK/DATA와 같은 프로토콜이 아니며, OSP 쪽에서는 `SIO_P/SIO_N`으로 연결합니다. 상대 커넥터에서 P/N을 DATA/CLK로 임의 해석해 바꾸지 않습니다.

기존 ADI 보드의 `AD3306` SPI 입력을 그대로 사용할 경우 위의 직접 OSP 배선과 호환되지 않습니다. 그 입력은 최소 `J8-4=SCK`, `J8-6=MOSI/SDI`, `J8-3=CS_N`이 필요하고, 응답까지 사용하면 `J8-5=MISO`도 필요합니다. 현재 펌웨어의 Click3는 직접 OSP 출력이므로 AD3306 SPI 입력에 연결하지 않습니다.

DIP가 OFF이면 헤더에 꽂힌 선이 LAN8661 신호에 연결되지 않습니다. 전원을 끈 상태에서 DIP 및 배선을 확인합니다. 기존 ADI에서 쓰던 OSIRE 보드는 첫 노드가 MCU 모드가 되도록 SIO_P 풀업 및 마지막 노드 EOL 설정도 유지되어야 합니다.

### 2026-09-07 하네스 입력 검사

OSP 출력을 일시적으로 해제한 진단 설정에서 PA08/PA09를 풀업·풀다운 없는 입력으로 열어 검사했습니다. 10회 모두 DATA(PA08)=LOW, CLK(PA09)=LOW였습니다. 이는 칩 핀의 디지털 입력 판독값이며, 전압 측정이나 DATA/CLK 뒤바뀜의 증거는 아닙니다. DIP 개방, 하네스 단선·오배선, LED 보드의 모드 설정·풀업 상태를 구분하려면 물리적인 확인이 필요합니다.

검사 후 `OSP-CLICK1-ISE-FIFO-FIX-0907` 설정을 복원하고 보드에서 설정 이름을 재확인했습니다. `TEMP_ONLY_OSP_HARNESS_INPUT_PROBE.mchpkg`는 LED 운전용이 아닙니다.

전원을 모두 끈 상태에서 J8-5 → OSP 입력 SIO1_P(DATA), J8-4 → OSP 입력 SIO1_N(CLK), 양 보드 GND의 도통을 확인합니다. DIP가 아래로 내려가 있다는 정보만으로 ON이라고 판단하지 말고 스위치의 ON 표시를 확인합니다. DATA 선을 5V에 직접 연결하지 않습니다.

진단 기록: `Diagnostics\osp_harness_input_probe_20260907.log`.

### 실제 OSP 응답 읽기 검사 (2026-09-07 11:02)

사용자가 J8-4/5 하네스를 변경한 뒤, GPIO 레벨 검사가 아닌 OSP 프로토콜 읽기를 수행했습니다. 임시 `OSP-DIRECT-READ-PROBE` 설정에서 SERCOM2를 반이중 SPI(PA08=DATA, PA09=수신 CLK)로 열고, MCU-A Manchester 인코딩으로 RESET/INIT_BIDIR/READ_STATUS/READ_TEMP/READ_PWM을 보냈습니다. GO_ACTIVE, 색상·PWM 쓰기, OTP 쓰기는 하지 않았습니다.

- SPI 5 MHz / 4.166 MHz / 6.25 MHz, 즉 Manchester 데이터 비트율 2.5 / 2.083 / 3.125 Mbit/s를 각각 시험했습니다.
- 읽기 18회(초기화 응답 3회, 상태 9회, 온도 3회, PWM 3회): **모두 LAN8661이 `0x21` SPI transfer error를 반환**, 정상 OSP 응답은 0회였습니다.
- RESET 송신 3회는 `0x00`을 반환했지만 이는 LAN8661 송신 처리 응답이지 LED의 응답이 아닙니다.
- 오류 패킷에 데이터처럼 보이는 바이트가 포함되어 있어도 LED 값으로 해석하지 않습니다. 성공 코드, 응답 길이, 프리앰블, 주소, 명령, PSI와 CRC 검사를 모두 통과해야 실제 OSP 응답으로 인정합니다.
- 네트워크 캡처에서도 보드의 오류 응답 18개를 확인했습니다. 따라서 현재 OSP 양방향 통신은 정상 확인되지 않았으나, 이 결과만으로 하네스 오배선·DIP·LED 인터페이스 모드·반이중 수신 타이밍 중 원인을 단정할 수는 없습니다.
- 검사 종료 후 `OSP-CLICK1-ISE-FIFO-FIX-0907` 복원 및 상태 조회를 완료했습니다. 진단 종료 코드 `2`는 프로그램 충돌이 아닌 유효 응답 0회를 의미합니다.

기록: `Diagnostics\osp_direct_read_20260907_110234.log`, 같은 이름의 `.pcapng`.
진단 소스: `Diagnostics\Lan8661BoardDiagnostic.cs` (`osp-read`, `osp-selftest`). 임시 진단 패키지는 일반 LED 운전용이 아닙니다.

프로토콜 근거: 로컬 Microchip V1.4.X User's Guide 5.8절, 기존 `NativeOsp\utils\fioOsp.c`의 명령/CRC 정의(읽기만 참조), [ams-OSRAM 공식 MCU-A Manchester 구현](https://github.com/ams-OSRAM/OSP_aospi/blob/main/src/aospi.cpp). 문서의 반이중 송수신 방식과 오류 코드를 기준으로 검사 및 판정했습니다.

### OSP 재부팅/저밝기 재검사 (2026-09-07 13:17)

OSP가 전원이 켜진 뒤 연결된 경우를 고려하여 설정 다운로드 없이 `main/app.bin`만 재부팅해 `OsireDisplayPort`의 부팅 초기화를 다시 실행했습니다. 이후 Click3에 RGB `1/255` 빨강, 초록, 파랑과 OFF를 80회 RTP로 전송했습니다. 보드 설정은 `OSP-CLICK1-ISE-FIFO-FIX-0907`로 유지됐습니다. RTP 출력은 반환 응답이 없는 단방향 경로이므로 이 기록은 PC와 LAN8661의 송신을 입증하지만 OSP LED의 실제 수신·점등까지 입증하지는 않습니다.

기록: `Diagnostics\osp_reboot_low_20260907_131739.log`.

## ISE 연결

ISELED 스트립을 LAN8661 헤더에 직접 연결하면 안 됩니다. ISELED 인터페이스 보드 또는 기존 ISELED Power Adapter Board의 MCU측 입력을 사용합니다.

- Click4 Clock: `J10-4` (`SCK-3`, `PA13`) → 인터페이스의 `SIO_N/CLK`. 기존 ADI ISELED Power Adapter Board를 쓰면 `J5-17`.
- Click4 Data: `J10-5` (`MISO-3`, `PA12`, 반이중 DATA) → 인터페이스의 `SIO_P/DATA`. 기존 ADI ISELED Power Adapter Board를 쓰면 `J5-19`.
- Ground: `J10-8` 또는 `J9-8` → 인터페이스 보드 GND.
- ISELED Power Adapter Board: 별도 전원을 `CON1`에 공급하고 LED 스트립을 어댑터의 `X1`/스트립 `CON1` 방향으로 연결합니다. LAN8661의 3.3V/5V 헤더로 스트립 전원을 공급하지 않습니다.

ISELED 제어는 `RESET → INIT(address 1, configuration 0x005) → 체인 주소/온도 Read → RGB` 순서로 실행됩니다. PC 프로젝트에 96비트 RLC 프레임, downstream CRC-8, upstream CRC-4 검증을 구현했으며 Click4/SERCOM3는 Mode 1, 약 1.923 MHz로 동작합니다. 현재 Sunshine-LAN8661 CIE는 검색된 주소 1~27에 각각 같은 RGB 명령을 한 번에 전송합니다.

### ILAS 21개 최저 밝기/읽기 실제 검사 (2026-09-07 11:59)

`Lan8661BoardDiagnostic.exe ilas-preflight`로 현재 PC CIE와 동일한 통신 코드를 실행했습니다. 보드 다운로드나 ADI 코드 변경 없이 수행했습니다.

- Click4/SERCOM3 반이중 SPI Mode 1 열기: 성공.
- 명령 코덱 자체 시험: RESET/INIT/RGB/READ_TEMP 96비트 프레임 생성 통과.
- 초기화 확인 Read: 성공, 응답 주소 `27`, configuration 설정값 `0x005`.
- 주소 1~21에 RGB `(1,0,0)`, `(0,1,0)`, `(0,0,1)`, `(0,0,0)`을 각각 21프레임/252바이트로 연속 전송: 성공. 채널 최댓값은 `1/255`, 마지막 명령은 21개 모두 OFF입니다.
- 각 색상 뒤 전체 체인 온도 Read: 4/4 성공. 매번 응답 주소 `27`, 원시값 `0x153`, RLC 및 CRC 정상입니다. 온도 원시값은 측정 시점에 따라 달라질 수 있습니다.
- SPI Mode 검사에서는 Mode 1/2만 정상 응답을 만들고 Mode 0/3은 CRC 검증에 실패했습니다. PC 경로는 Mode 1로 고정했습니다.
- 펌웨어 설정 `OSP-CLICK1-ISE-FIFO-FIX-0907`은 변경되지 않았습니다.

최종 PC 경로 기록: `Diagnostics\ilas_pc_path_21_20260907_115907.log`. 주소 1~21 저수준 송수신 기록: `Diagnostics\ilas_21_low_20260907_115820.log`. 기존 Click1 100개 전송/ACK 기록: `Diagnostics\click1_100_low_20260907_115850.log`.

세 출력 동시 저밝기 검사 기록은 `Diagnostics\all_three_low_20260907_131849.log`입니다. 기존 LED 100개 SPI, OSP RTP, ISE 주소 1~21을 같은 R/G/B/OFF 순서로 함께 송신했고 ISE Read 4/4가 통과했습니다.

### 2026-09-07 OSP MCU 모드 및 기존 LED 래치 보정

OSIRE E3731i는 RESET/POR 뒤 첫 포트의 레벨로 통신 모드를 선택합니다. Click3의 첫 노드가 MCU 모드로 들어가도록 현재 보드 설정에 `PA08(SIO_P)=내부 Pull-up`, `PA09(SIO_N)=내부 Pull-down`, 두 핀 `4mA` 구동을 명시했습니다. 새 설정 패키지는 `LAN8661_OSP_CLICK1_ISE_MCUA_FIXED_20260907.mchpkg`, 보드에서 읽은 설정명은 `OSP-CLICK1-ISE-MCUA-FIX-0907`입니다. Click1과 Click4 구성은 바꾸지 않았습니다.

적용 뒤 OSP에는 RGB 1/255 빨강·초록·파랑·OFF를 송신했고 기록은 `Diagnostics\osp_mcua_pullfix_low_20260907.log`입니다. RTP는 단방향이므로 이 ACK는 네트워크 입력까지만 보장합니다. OSIRE 응답은 LED가 만드는 CLK를 LAN8661이 SPI slave로 받아야 하므로 일반 Remote-SPI master Read로 대신 판정할 수 없습니다.

기존 Click1 WS2812는 각 프레임 전후 LOW 래치 구간을 우선 길게 보강했습니다. 당시 100개 전체 저밝기 RGB/OFF 전송 기록은 `Diagnostics\click1_latch307us_low_20260907.log`입니다. 이후 반복 프레임에서 흰색으로 오인되는 현상을 확인하여 아래의 4비트 인코딩으로 최종 변경했습니다. ISE는 동일 설정에서 Read 4/4를 다시 통과했으며 `Diagnostics\ise_after_mcua_fix_low_20260907.log`에 남겼습니다. 모든 시험은 OFF로 끝냈습니다.

추가로 PC가 Click1 원격 SPI를 여는 순간 PA03이 부동 상태가 되지 않도록, `ReleaseDigitalPins` 직후 PA03을 Pull-down/4 mA로 고정한 다음 SERCOM0을 여는 보호 코드를 적용했습니다. OSP 단독 진단은 Click1 포트를 열지 않고 RGB `25/255`(약 9.8%) 이하만 사용합니다.

프레임 인코딩과 CRC는 로컬 INLC10AQ 데이터시트 및 보유한 NXP ISELED 참조 드라이버 실행 파일의 검증된 테이블/배치를 기준으로 구현했습니다.

### 2026-09-07 OSP 12노드 양방향 제어 및 최종 통합 검사

이 절의 결과가 위의 초기 OSP 단방향/Read 실패 기록을 대체합니다. 현재 보드 설정은 `PC-DIRECT-CLICK1-OSP-ISE-0907`이며, SERCOM0(First), SERCOM2(OSP), SERCOM3(ISE)를 PC에서 직접 제어합니다.

## 2026-09-09 First RTP 진단 및 PC 직접 제어 복구

반복 전송 때 First LED의 첫 1~2개가 초록색으로 남거나 밝은 흰색으로 번쩍이는 현상은 SERCOM0 원격 SPI로 만든 WS2812 파형의 선두 비트가 잘못 래치되는 문제로 확인했습니다. 전체 OFF 프레임의 보드 ACK가 정상인데도 두 픽셀이 남았으므로 CIE 색상/Shift 배열 문제는 아닙니다.

Microchip 기본 `WS2812DisplayPort`와 동일한 20×10 RTP 프레임을 연속 송신하는 단독 설정까지 시험했지만, LAN8661의 RTP 수신과 SERCOM0 소유권만 확인되고 사용자가 보는 First LED는 점등되지 않았습니다. 따라서 RTP 성공을 실제 LED 점등 성공으로 판정하지 않습니다. WS2812는 단방향 소자라 LED 자체의 Read-back도 없습니다.

현재 보드에는 실제 점등 이력이 있는 PC 직접 제어 구성 `LAN8661-main-osire_V1.4.0-70` / `PC-DIRECT-CLICK1-OSP-ISE-0907`을 다시 적용했습니다.

- First 100개: PC 원격 SPI, SERCOM0 P3/PA03에서 WS2812 단선식 파형 생성
- OSP: PC 원격 SPI, SERCOM2, 12개 자동 감지 및 PWM Read-back
- ISE: PC 원격 SPI, SERCOM3, PC 제어 대상 21개 및 RLC/CRC 응답 확인
- CIE 초기 밝기: 10%, 최소 조절 단위: 1%
- First는 WS2812 계열이며 OSP/ISE와 달리 별도 CLK나 응답선이 없습니다. 24비트 GRB 데이터를 약 800 kbit/s의 HIGH 펄스 폭으로 구분해 직렬 전송하고 LOW 구간으로 래치합니다.
- 회로도상 Click 1의 `DI (WS2812)`는 mikroBUS `PWM-0`(J4의 최상단 PWM 핀)입니다. PA03을 그 핀으로 연결하려면 `SW5-8=ON`, `SW5-7=OFF`로 두어야 합니다. SW5-7과 8을 동시에 ON하면 Click 1의 MOSI와 PWM 네트가 서로 연결될 수 있으므로 사용하지 않습니다.

펌웨어 패키지: `D:\jhhan\LAN8661X\Firmware\LAN8661_OSP_ISE_WS2812_V1.4.0\LAN8661_PC_DIRECT_CLICK1_OSP_ISE_20260907.mchpkg`

PC 실행 파일: `D:\jhhan\LAN8661X\PC_Lan8661_20260904\bin\Debug\LINMaster.exe`

2026-09-09 재다운로드 후 보드에서 위 앱/설정명을 읽었습니다. First 단독으로 RGB `25/255`(약 9.8%) 빨강→초록→파랑을 두 번 전송하고 OFF까지 모든 보드 ACK가 통과했습니다(`Diagnostics\first_direct_retest_20260909.log`). 이 결과는 PC→LAN8661→SERCOM0 송신 요청까지만 증명하며, WS2812에는 Read-back이 없으므로 실제 광출력은 별도로 관찰해야 합니다.

- OSP INIT 원시 응답 `D0-18-81-00-20-31`은 MCU 수신의 1비트 정렬을 보정하면 `A0-31-02-00-40-62`이고 CRC가 일치합니다. 응답 주소가 12이므로 연결된 OSP 체인은 12노드입니다.
- 이전 PC 코드는 주소 1에만 `SETPWM`을 보냈습니다. 현재 코드는 검출된 체인 전체에 broadcast하고 노드 1과 마지막 노드 12의 `READPWM`, `READLEDST`, `READSTATUS`를 검증합니다.
- 말단 노드 응답도 1비트 늦게 샘플링될 수 있어 자동 정렬하고, 유실 가능한 마지막 CRC 비트는 복원된 메시지의 CRC 계산값으로 검증·복원합니다.
- OSP 12개 전체에 주간 전류, RGB `25/255`(9.8%)를 차례로 전송했습니다. 각 색의 PWM Read-back이 송신값과 일치했고 양 끝 노드 모두 `STATUS=0x80`(ACTIVE), `LEDSTATUS=0x00`(R/G/B 단선·단락 없음)이었습니다. 마지막 명령은 전체 OFF입니다. 기록: `Diagnostics\pc_direct_osp_12nodes_crcfix_20260907_171117.log`.
- First 100개 전송 직후 다른 SERCOM을 시작하면 내부 전송 큐가 겹칠 수 있어 포트 전환 보호 시간을 6 ms에서 20 ms로 늘렸습니다. 이는 색 변경 시 First가 흰색으로 번쩍이는 현상과 OSP INIT 간헐 실패를 방지합니다.
- 최종 통합 시험은 First 100 + OSP 12 + ISE 21에 RGB `25/255`(9.8%)를 전송한 뒤 모두 OFF했습니다. OSP 양 끝 Read, ISE 체인 Read가 모두 통과했습니다. 기록: `Diagnostics\pc_direct_all_osp12_guard20_red10_20260907_171248.log`.
- Microchip CIE 밝기는 소수점 없이 1% 단위이며 초기값은 10%입니다.

### 2026-09-07 1회/주기 TX 및 체인 수량 기반 Shift

- `1회 TX`는 타이머를 시작하지 않고 선택된 출력 프레임을 정확히 한 번만 송신합니다. Shift 모드에서는 버튼을 누를 때마다 한 단계만 진행합니다.
- `주기 TX`만 반복 송신하며 왼쪽의 `Interval (ms)` 값 1~60000 ms를 그대로 사용합니다.
- Shift를 선택하지 않으면 별도 조건 없이 현재 CIE 색상과 밝기를 전체 선택 LED에 고정색으로 송신합니다.
- First 프레임은 매 송신마다 600바이트 전체를 OFF로 초기화한 뒤 다시 작성합니다. 이전 대상/Shift 데이터가 남아 First 색상이 임의로 바뀌는 현상을 제거했습니다.
- OSP는 INIT 응답으로 검출한 12노드, ISE는 INIT 후 체인 Read 응답의 마지막 주소 27을 실제 LED 수량으로 사용합니다. 각 체인 전체 길이에 맞춰 다음과 같이 반복합니다.
  - `Shift`: 켜진 LED 하나가 첫 노드부터 마지막 노드까지 한 칸씩 이동한 뒤 처음으로 복귀합니다.
  - `Shift Stack`: 한 LED가 마지막 노드까지 이동하면 그 노드를 켜 둡니다. 다음 LED가 처음부터 그 앞까지 이동하는 과정을 반복하여 전부 켜진 뒤 초기화합니다.
  - `Shift Up`: 중앙에서 좌우 한 쌍이 양 끝으로 한 칸씩 이동한 뒤 중앙으로 복귀합니다. 홀수 체인은 첫 단계에서 중앙 LED 하나만 켜집니다.
  - `Shift Stack Up`: 중앙에서 출발한 좌우 한 쌍이 양 끝에 닿으면 끝을 켜 둡니다. 다음 쌍이 다시 중앙에서 출발하여 안쪽으로 누적하고 전부 켜진 뒤 초기화합니다.
- OSP 주기 송신은 연결 및 첫 프레임에서 체인을 Read 검증한 뒤, 매 주기에는 색상이 바뀐 노드만 ACK 방식으로 씁니다. 따라서 Shift는 보통 OSP 2개 노드만 갱신하고, 매 프레임마다 수행하던 양 끝 `READPWM/READLEDST/READSTATUS` 6회를 생략합니다. 응답 순서를 깨뜨렸던 fire-and-forget 방식은 사용하지 않습니다.
- First+OSP+ISE 동시 Shift의 두 번째 프레임은 실측 약 55 ms였으며 OSP 마지막 노드 Read-back까지 통과했습니다. 따라서 세 채널 동시는 `Interval`을 55 ms 이상으로 두는 것이 안전하며, OSP 또는 ISE 단독은 이보다 빠릅니다. 설정값보다 실제 송신 시간이 길면 물리 전송 시간이 최소 주기가 됩니다.
- 저밝기 실측에서 First 100개는 동일한 빨강 `25/255`로 유지하고 OSP 12개와 ISE 27개에는 서로 한 칸 어긋난 두 Shift 패턴을 송신했습니다. OSP 양 끝 Read가 모두 `STATUS=0x80`, 전체 OFF까지 통과했습니다. 기록: `Diagnostics\pc_direct_detected_shift_20260907_173051.log`.
- 최적화 적용 후 전체 3채널 Shift 검사 기록: `Diagnostics\pc_direct_delta_period_20260907_175223.log` (`OSP=12`, `ISE=27`, 주기 프레임 약 55 ms, OSP Read-back 통과, 최종 OFF).
- First만 선택하여 동일한 빨강 `25/255` 프레임을 10회 반복한 결과 매회 약 30~34 ms로 모두 ACK됐고 최종 OFF까지 통과했습니다. OSP/ISE 색상 송신은 하지 않았습니다. 기록: `Diagnostics\pc_direct_first_stable_20260907_175619.log`.

### 2026-09-07 First 반복 송신 흰색 오인 보정

- 이전 First 파형은 LAN8661 SPI 2.083 MHz에서 WS2812 한 비트를 `0=100`, `1=110`의 3비트로 인코딩했습니다. LED 비트 주기는 약 1.44 µs이고 `0`의 HIGH 시간이 약 0.48 µs라 수신 판정 경계에 가까웠습니다. 이 때문에 반복 프레임의 일부 `0`이 `1`로 오인되면 밝은 흰색이 나타날 수 있었습니다.
- LAN8661이 지원하는 3.125 MHz에서 `0=1000`, `1=1110`의 4비트 인코딩으로 변경했습니다. LED 비트 주기는 약 1.28 µs, T0H는 약 0.32 µs, T1H는 약 0.96 µs입니다.
- 100 LED 데이터는 1200바이트이며 앞뒤 LOW 각 80바이트를 포함한 총 전송량은 1360바이트입니다. PC 제한 1400바이트와 현재 SERCOM0 FIFO 1536바이트 이내입니다.
- First만 RGB `25/255`(9.8%) 빨강→초록→파랑을 두 번 반복하고 OFF까지 전송했습니다. OSP/ISE 색상 전송은 비활성 상태였고 전 프레임이 정상 ACK됐습니다. 기록: `Diagnostics\pc_direct_first_4bit_20260907_180621.log`.
- 같은 파형으로 First 100 + OSP 12 + ISE 21 통합 저밝기 및 OFF를 다시 검사했습니다. OSP `STATUS=0x80`, ISE Read가 통과했습니다. 기록: `Diagnostics\pc_direct_all_after_first4bit_20260907_180644.log`.

펌웨어 설정 패키지는 `D:\jhhan\LAN8661X\Firmware\LAN8661_OSP_ISE_WS2812_V1.4.0\LAN8661_PC_DIRECT_CLICK1_OSP_ISE_20260907.mchpkg`이고, PC 실행 파일은 `bin\Debug\LINMaster.exe`입니다.

### 2026-09-09 OSP/ISE 실제 주기 측정

First 송신을 완전히 비활성화하고 OSP 12개와 ISE 21개에서 한 개의 저밝기 빨간 점(25/255)을 이동시켜 각각 30프레임을 측정했습니다. 마지막에는 OSP/ISE를 모두 OFF하고 Read 검증했습니다. 기록은 `Diagnostics\osp_ise_period_measure_20260909.log`입니다.

- OSP Shift: 최소 10 ms, 평균 10.9 ms, 95% 12 ms, 최대 12 ms
- ISE Shift: 최소 6 ms, 평균 6.1 ms, 95% 7 ms, 최대 7 ms
- OSP+ISE 동시 Shift: 최소 16 ms, 평균 17.5 ms, 95% 18 ms, 최대 19 ms

현재 UI는 WinForms UI 타이머와 동기식 ACK 송신을 사용하므로 입력한 10 ms는 요청값이며 실시간 보장값이 아닙니다. 안정 권장값은 OSP 단독 15 ms 이상, ISE 단독 10 ms 이상(일반 Windows UI에서는 실제 약 10~16 ms), OSP+ISE 동시 20 ms 이상입니다. First까지 포함한 세 채널 동시는 기존 실측상 55 ms 이상을 사용해야 합니다.

## 파일

- 솔루션: `LINMaster.sln`
- 실행 파일: `bin\Debug\LINMaster.exe`
- LAN8661 통신: `LAN8661\Lan8661Ws2812Client.cs`, `LAN8661\Lan8661SomeIpSpi.cs`, `LAN8661\IlasCodec.cs`
- Microchip CIE 동작: `Forms\FormLan8661CIE.cs`
- 연결 시험: `Diagnostics\bin\Lan8661ConnectionSmokeTest.exe`
- 변경 전 백업: `D:\jhhan\LAN8661X\Backups\PC_Lan8661_before_OSP_ISE_20260904`
