# OSP Logo 실제 보드 진단 — 2026-09-15

## 결론

PC의 기존 CJ 화면 송신 처리와 실제 PEAK LIN을 사용했다.
Chair1 MCU의 LIN 수신과 진단 응답은 확인했다.
이전 TX 형식의 시험 펌웨어에서 Logo 전체 RED 점등을 사용자가 확인했다.
정식 펌웨어에서도 Logo 24개를 검출하고, PC RED 명령과 실제 LED PWM Read 값의 일치를 확인했다.
INIT 직후 SLEEP(40), GO_ACTIVE 직후 ACTIVE(80)를 실제 LED 응답으로 확인했다.
이전 프로젝트에 있던 최초 RESET 전 5 ms 대기가 현재 코드에서 누락되어 있었다.
이 대기만 추가한 비교에서 검출 실패가 해소되어 정식 코드에 반영했다.
LED 전원 상승 파형 자체를 측정한 것은 아니므로, 전원 안정화라는 전기적 설명은 초기화 순서와 비교 결과에 따른 추정이다.
사용자가 확인한 핀맵과 전원 연결은 변경하지 않았다.
임시 TX/Read 계측 코드는 제거하고 정식 Debug 펌웨어를 다운로드했다.
SRGB/Strobe는 미연결 상태이므로 이번 실제 보드 검증 대상이 아니다.

## 수정 전 실제 결과 — 과거 시험 기록

아래의 Read 실패와 미점등 기록은 5 ms 대기 수정 전 결과다.
최종 수정 후 결과는 문서 마지막에 정리했다.

| 확인 항목 | 결과 |
|---|---|
| 초기 대기 | SysTick 시작부터 첫 main 반복문까지 약 60 ms. 전원 투입부터의 전체 시간 측정은 아님 |
| Logo 최초 READ_SETUP | 3회 실패. 검출 노드 수 0 |
| Strobe 최초 READ_SETUP | 3회 실패. 현재 미연결 |
| 미검출 OSP | OTP와 주기적 내부 상태 Read 생략. 통신 이상으로 보고 |
| Logo RX DMA | 첫 Read에서 falling-to-falling period 26개. 요청 A0 04 4C FA의 송신 에코만 복원. LED 응답 없음 |
| 실제 빨강 SET_PWM 에코 | 69개 period로 A0 03 4F 85 04 80 00 80 00 16 복원, CRC 16 일치. TC 캡처 ERR 없음 |
| 빨강 송신 후 READ_PWM 직접 시험 | 주소 1에 3회 요청. 모두 Read 실패. 각 캡처는 요청 A0 04 4E A4의 에코 25개 period뿐이며 10-byte LED 응답 없음 |
| 실제 MCU 수신 코드 시험 | 정상 응답의 이상적인 period 32개를 RAM에 주입. A0 04 CC 24 19 복원, 검사 결과 1. 실제 LED 응답이 아님 |
| 호스트 모델 시험 | 65,536개 두 바이트 payload 조합, 타이머 half-bit 9.2/10/10.8 ticks, CRC/주소/명령 오류, 송신 에코 거절 및 idle 뒤 응답: 통과 |
| Chair1 LIN 진단 | 반복 수신 성공, Enhanced Checksum 정상 |
| Chair2~4 | 응답 없음. PEAK의 FF 데이터 / Checksum 00 / VALID=False는 정상 Slave 응답이 아님 |
| 최종 상태 | 낮은 밝기의 Logo 빨강 명령 전송, MCU 실행 재개, 시험 중 설정한 중단점 제거 |

실제 Chair1 진단 송수신:

```text
Master Header : BREAK 55 CA
Slave Response: 1F 3F 5F 7F 92 B2 00 00 B2
               └──────── Data 8 Byte ────────┘ CS
```

마스터 진단 요청은 Header만 송신한다.
이 때 PC 로그 객체의 빈 Data/Checksum을 실제 마스터 송신으로 해석하면 안 된다.
시험 도구 로그는 이 요청을 `TX HEADER`로 구분한다.
Slave는 ID를 다시 보내지 않고 Data와 Checksum을 보낸다.

Logo 진단 바이트 92의 80은 Logo 구분 값, 10은 통신 이상, 02는 전압 이상 비트다.
통신 이상은 LED Read 실패로 확인했다.
전압 이상은 PA00의 `+5V_LED_FLT` 디지털 입력으로 판단한다.
PA00이 High이면 정상, Low이면 전압 이상이다. 회로에서 미연결된 PA02 ADC는 사용하지 않는다.
Open/Short/온도는 LED 내부 데이터를 읽지 못했으므로 정상 여부를 확인하지 못했다.

## 회로도와 코드의 MCU 핀 비교

| 신호 | 회로도 | 수정 후 코드 |
|---|---|---|
| LIN TX | PA04 | PA04 / SERCOM0 PAD0 |
| LIN NSLP | PA06 | PA06 |
| LIN RX | PA07 | PA07 / SERCOM0 PAD3 |
| Logo TX | PA24 | PA24 |
| Logo RX | PA22 | PA22 / EXTINT6 |
| Logo STBY | PA23 | PA23, LOW |

기존 LIN NSLP PA05 / RX PA06 설정을 실제 회로도에 맞게 수정했다.
LIN 자동 Baud 측정 모드는 해당 CPU의 fractional SAMPR=1로 수정했다.
OSP TX는 SERCOM3의 SPI 시프트 회로를 Manchester 직렬화기로 사용한다.
PA24 TX만 연결하며 SPI 클럭/SS 핀은 외부에 연결하지 않는다.
실제 LED 통신은 SPI가 아니라 2.4 Mbps Manchester다.
DMA가 4.8 Mbps의 half-bit 패턴을 바이트 단위로 공급한다.
RX는 EIC와 TC2 PWP의 CC1 period만 DMA 한 채널로 캡처한다.
RX를 송신 전에 활성화하여 CAN 모드의 짧은 응답 지연을 놓치지 않도록 했다.
마지막 DMA beat가 write-back 메모리에 반영되도록 캡처 종료 시 suspend 후 개수를 읽는다.
다음 Read는 DMA 채널을 reset하고 새 descriptor로 시작한다.
캡처 버퍼는 요청과 최대 12-byte 응답을 함께 수용하도록 256개다.
Manchester 복원 후 응답의 preamble, 주소, PSI, 명령, CRC를 검사한다.
프레임 송신 종료 후 다음 명령까지 15 us 간격을 확보한다.
일반 색상/Fade 출력은 10 ms 갱신 시에만 송신하며 Flicker는 1 ms다.
색상 명령 직후 이전 색상을 먼저 송신하던 불필요한 dirty 처리를 제거했다.
CPU는 OSC48M 48 MHz, Flash는 클럭 상승 전에 2 Wait State를 적용했다.
송신 에코의 독립 TC 타이머 캡처에서 19/29/39-tick period와 요청 CRC를 확인했다.
그러나 MCU RX에서 보인 에코는 LED가 명령을 수신/처리했다는 증거가 아니다.
LED 쪽 차동 신호와 LED 내부 통신 모드는 아직 외부 측정으로 확인하지 못했다.

## 수정 전 검증과 당시 다음 시험

IAR Debug/Release 및 PC Debug 빌드 성공.
User Row 256 Byte와 Data Flash 8192 Byte를 읽어 기존 백업과 SHA256 비교: 모두 동일.
UDS/ID 변경 명령은 송신하지 않았다.
PC 기존 화면/Designer/게이트웨이 코드는 수정하지 않고 tools/CJLogoHardwareTest.cs만 추가했다.

낮은 밝기의 LIN 빨강 송신값은 0F 10 17 0A 00 00 05 05, Enhanced CS F3다.
MCU R=2570/65535 -> OSP R PWM=1284/32767, G=B=0이다.
OSP SET_PWM의 R=85 04, G=80 00, B=80 00에서 상위 bit 15는 전류 선택이다.
PWM은 각각 하위 15 bit이며, 85 04의 PWM은 05 04 = 1284다.
이 시험은 MCU SYSRESET 기반이며 LED 전원을 별도로 껐다 켠 시험은 아니다.

다음 확인은 U4 RXD(4번) 및 LED 쪽 CAN_H/CAN_L에서 송신 요청 뒤 응답 유무를 측정하는 것이다.
핀을 변경하라는 의미가 아니며, MCU의 송신 에코와 LED 자체 응답을 구분하기 위한 측정이다.
LED 측 CAN 모드 초기 선택 상태도 함께 확인해야 한다.

시험 원본 로그는 프로젝트 상위 tmp/iar_busy_diagnostic의 CaptureLogoPwmCurrent_Final_Red.log,
OSP_Current_Ideal_ActualC.log, CJ_Logo_Final_Red_LIN.log에 있다.
호스트 모델은 tests/OspCaptureModel.cjs이며 IAR 빌드/다운로드 대상이 아니다.

## 추가 READ_PWM 시험 — 16:08

초기 존재 검출이 실패한 경우에도 읽을 수 있는지 별도 확인했다.
진단용으로 Logo 검출 노드 수를 RAM에서만 1로 지정하고 기존 동적 Read 함수를 3회 실행했다.
READ_PWM 호출의 반환값은 모두 0(실패)이었다.
이는 R/G/B가 0으로 읽혔다는 의미가 아니라 유효한 LED 응답이 없다는 의미다.
MCU 송신 상태의 pwmRed=1284는 계산한 출력값이며 LED에서 읽은 값이 아니다.
시험 뒤 노드 수 0으로 복원하고 중단점을 제거하여 정상 실행을 재개했다.
펌웨어 및 저장 메모리는 변경하지 않았다.
로그: tmp/iar_busy_diagnostic/OSP_Logo_Read_PWM_Three.log.

## 추가 INIT → GO_ACTIVE 단계별 실제 보드 시험

Logo만 임시 계측하여 다운로드 직후 1회와 MCU SYSRESET 후 2회, 총 3회 초기화를 확인했다.
각 Read는 부팅마다 3회 실행하여 항목당 총 9회 확인했다.
LED 전원을 별도로 껐다 켠 시험은 아니다.
전원·핀·STBY·통신 속도·초기화 명령은 기존 설정을 유지했다.
송신부터 캡처 종료까지 중단점 없이 실행한 뒤 기록을 읽었다.

| 순서 | 동작 | 실제 OSP 송신 Raw Byte, CRC 포함 | 결과 |
|---|---|---|---|
| 1 | RESET, 주소 0 브로드캐스트 | A0 00 00 22 | 송신 에코와 CRC 정상. 원래 응답 없는 명령 |
| 2 | SET_SETUP = 24 | A0 00 CD 24 C8 | 송신 에코와 CRC 정상. 원래 응답 없는 명령 |
| 3 | INIT_BIDIR, 시작 주소 1 | A0 04 02 A9 | 송신 에코와 CRC 정상. LED 초기화 완료는 별도 Read로 확인해야 함 |
| 4 | INIT 후 READ_SETUP, 주소 1 | A0 04 4C FA | 9회 모두 유효한 LED 응답 없음 |
| 5 | INIT 후 READ_STATUS, 주소 1 | A0 04 40 11 | 9회 모두 유효한 LED 응답 없음. SLEEP 확인 불가 |
| 6 | INIT 후 READ_COM_STATUS, 주소 1 | A0 04 44 AD | 9회 모두 유효한 LED 응답 없음. LED 내부 CAN 모드 확인 불가 |
| 7 | GO_ACTIVE 브로드캐스트 | A0 00 05 B1 | 송신 에코와 CRC 정상. 원래 응답 없는 명령 |
| 8 | GO_ACTIVE 후 READ_STATUS, 주소 1 | A0 04 40 11 | 9회 모두 유효한 LED 응답 없음. ACTIVE 확인 불가 |
| 9 | GO_ACTIVE 후 READ_PWM, 주소 1 | A0 04 4E A4 | 9회 모두 유효한 LED 응답 없음. RGB/PWM 읽힌 값 없음 |

위 Raw Byte는 MCU RX에서 되돌아온 송신 에코를 복원한 값이다.
LIN 데이터가 아닌 MCU ↔ OSP 구간의 데이터이며 마지막 Byte가 OSP CRC다.
모든 캡처는 요청 프레임의 19/29/39-tick period만 기록되었고, 별도 LED 응답 프레임은 없었다.
송신 에코가 정상이라는 사실만으로 LED 입력까지 도달했거나 명령을 처리했다고 확정할 수 없다.

정상 판단 기준은 Startup Guide p.13~14 및 p.36의 READSTATUS Table 13/14다.
INIT 완료 후 STATE[1:0] = 01(SLEEP), GO_ACTIVE 완료 후 10(ACTIVE)가 실제 응답에서 확인되어야 한다.
STATUS 상위 2비트 마스크 기준으로 각각 40 / 80이며 하위 비트는 오류·통신 상태에 따라 다르다.
READCOMST의 SIO1 상태는 bits 1:0, SIO2 상태는 bits 3:2이며 CAN은 11이다.
UNINITIALIZED에서는 일반 Read가 무시되므로 RESET 직후 Read 무응답을 고장 판정에 사용하지 않았다.
Startup Guide p.34에 따라 CAN 노드의 INIT 마지막 EOL 응답은 기대하지 않고 직접 주소 1을 Read했다.

따라서 최초 확인 실패 지점은 GO_ACTIVE 이후가 아니라 INIT 직후의 첫 주소 지정 Read다.
LED 초기화 실패인지, 요청 도달 문제인지, 응답 경로 문제인지는 이 기록만으로 구분할 수 없다.
시험용 계측은 제거하고 원래 OSPControl.c로 복원한 뒤 정상 빌드를 다시 다운로드했다.
PC 프로젝트 및 저장 메모리 설정은 변경하지 않았다.
원본: tmp/iar_busy_diagnostic/OSP_Logo_Init_Probe.log, OSP_Logo_Init_Probe_1~3.bin.
분석: tmp/iar_busy_diagnostic/OSP_Logo_Init_Probe_Analysis.log.

## 최종 원인 분리와 수정 후 실제 결과

비교 대상은 D:\jhhan\program\CJ\CJ_Final_20260715_OTP_Test의 applications/src/spi_led.c다.
원본의 Manchester LUT 256개와 현재 인코딩 결과는 모두 일치했다.
원본도 SPI 시프트 회로와 DMA로 Manchester TX를 수행했다.
원본의 CLK/Data RX와 현재 Manchester RX는 다르지만, TX Raw Frame 구성과 인코딩 원리는 동일하다.

초기 원본 형식 시험은 SPI CPOL, 패딩, 주소별 송신, 최초 5 ms 대기를 함께 적용했다.
이 시험에서 전체 RED가 켜졌으므로, 이후에는 조건을 분리해서 확인했다.
한 번 정상 초기화된 LED에서 CPOL 0/1과 패딩 0/4 Byte의 네 조합 모두 유효한 Read가 나왔다.
READ_SETUP/READ_STATUS/READ_PWM 각 3회, 네 조합, 세 라운드로 총 108회 모두 성공했다.
이는 네 조합의 Read 가능성을 확인한 시험이며 모든 전원 투입 조건에서 동등하다는 보장은 아니다.

| 정식 펌웨어 비교 조건 | 확인 횟수 | Logo 검출 | 결과 |
|---|---:|---:|---|
| 최초 RESET 전 대기 없음 | 3회 부팅 | 매회 0개 | 초기 Read 실패 |
| 최초 RESET 전 5 ms 대기만 추가 | 3회 부팅 | 매회 24개 | 전 노드 staticValid/dynamicValid = 1 |

SPI 모드, 패딩, 브로드캐스트 색상 송신, 현재 Manchester RX는 그대로 유지했다.
OSPControl_Init에서 DMA/RX 캡처 설정 다음, 최초 OSP_InitPort 앞에 OSP_DelayMs(5U)만 추가했다.
부팅은 다운로드 및 MCU SYSRESET 기반이며 LED 전원을 별도로 껐다 켠 시험은 아니다.
실제 전원 투입부터의 안정화 시간과 파형은 별도 측정이 필요하다.

### INIT → GO_ACTIVE 실제 LED 응답

아래는 MCU ↔ OSP의 디코딩된 Raw Byte다.
각 행의 마지막 Byte는 OSP CRC이며 LIN Checksum과 다르다.

| 확인 시점 | MCU 요청, CRC 포함 | 실제 LED 응답, CRC 포함 | 해석 |
|---|---|---|---|
| INIT 후 READ_SETUP, 주소 1 | A0 04 4C FA | A0 04 CC 24 19 | Setup = 24 |
| INIT 후 READ_STATUS, 주소 1 | A0 04 40 11 | A0 04 C0 40 60 | STATE = SLEEP |
| GO_ACTIVE 후 READ_STATUS, 주소 1 | A0 04 40 11 | A0 04 C0 80 65 | STATE = ACTIVE |

각 단계 3회 × 3회 초기화로 총 27회 모두 정상 응답을 확인했다.
송신 에코가 아니라 주소/명령/길이/CRC 검사를 통과한 LED 응답이다.
시험 계측은 제거한 뒤 동일한 5 ms 수정만 남긴 정식 코드를 다시 빌드/다운로드했다.

### 정식 PC RED 명령 → 정식 MCU → 실제 Logo Read

```text
LIN Master: BREAK 55 C1 | 0F 10 17 0A 00 00 05 05 | F3
                         Data 8 Byte                Enhanced CS

MCU 색상값: R = 2570 / 65535, G = 0, B = 0
OSP PWM   : R = 1284 / 32767, G = 0, B = 0
OSP 색상 데이터: 85 04 | 80 00 | 80 00
                R       G       B

READ_PWM 주소 1 요청: A0 04 4E A4
실제 LED 응답      : A0 07 4E 85 04 80 00 80 00 5D
```

85 04의 bit 15는 전류 선택이고 하위 15 bit 05 04가 PWM 1284다.
80 00은 같은 전류 선택 상태에 PWM 0이다.
최종 정식 펌웨어에서 Logo 주소 1~24 모두 R=1284/G=0/B=0, STATUS=80, staticValid=1, dynamicValid=1로 읽혔다.
Logo 통신 오류/CRC 오류/Manchester 복원 오류 카운터는 모두 0이었다.
최종 캡처 온도는 약 31~34 °C, LED Open/Short 상태 바이트는 00이었다.
이 온도/상태는 해당 시험 시점의 Read 결과이며 실제 고장 주입 시험은 아니다.

정식 PC의 Chair1 진단 송수신:

```text
Master Header : BREAK 55 CA
Slave Response: 1F 3F 5F 7F 82 B2 00 00 C2
               Data 8 Byte             Enhanced CS
```

Logo 바이트가 수정 전 92에서 수정 후 82로 바뀌어 통신 이상 비트 10이 해제되었다.
02 전압 이상 비트는 PA00의 `+5V_LED_FLT`가 Low일 때 설정된다.
이 비트만으로 실제 LED 5 V 공급 전압 이상을 판단하지 않는다.
SRGB 네 채널과 Strobe는 미연결이므로 해당 채널의 통신 이상은 이번 Logo 결과와 분리해야 한다.

### 최종 빌드·저장 메모리·검증 범위

Debug/Release 최종 빌드 모두 오류 0, 경고 0이다.
User Row 256 Byte와 Data Flash 8192 Byte의 SHA256은 시험 전 백업과 동일하다.
UDS/ID 변경 명령이나 저장 메모리 쓰기는 수행하지 않았다.
PC의 기존 CJ 화면/Designer/게이트웨이 코드는 변경하지 않았다.
보드는 임시 시험 모드 없이 정식 펌웨어로 실행 중이며 시험 중단점은 제거했다.
이번 실제 보드 검증은 Logo 초기화, TX/RX, RED 색상/PWM Read, LIN 진단까지다.
Fade/Flicker 및 SRGB/Strobe의 실제 하드웨어 동작 전체를 검증했다는 의미는 아니다.

최종 원본 자료는 프로젝트 상위 tmp/iar_busy_diagnostic에 있다.

- OSP_Settle5ms_Regression_Analysis.log: 대기 없음/5 ms 대기 비교 및 24개 PWM
- OSP_TX_Framing_Read_Analysis.log: 네 TX 조건의 총 108회 실제 Read
- OSP_Fixed_Init_Stages_Analysis.log: SLEEP → ACTIVE 총 27회 실제 응답
- CJ_Logo_Final_TX_Fix_RED_LIN.log: 최종 정식 PC RED/진단 송수신
- OSP_Final_Settled_Logo_RED.log, OSP_Final_Settled_Logo_RED.bin: 최종 정식 MCU Read 결과
