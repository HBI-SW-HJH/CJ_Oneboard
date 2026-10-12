# Flicker 100 Hz / 진단 Read 분산 적용 결과

현재 코드는 사용자 요청에 따라 초기값 30/60/90과 Main의 독립된 세 if문으로 변경했다.
추가 요청에 따라 SysTick과 Flicker는 1 ms, Duration은 실제 ON 비율 10% 간격으로 변경했다.
tick500us는 제거했다.
Main의 tick1ms >= 1 조건에서 tick1ms를 0으로 초기화한 뒤 두 Process를 호출한다.
updateNormal 변수와 함수 인자는 삭제했다.
Main의 1 ms 블록은 SRGBControl_Process / OSPControl_Process를 한 번씩 호출한다.
각 Process는 Flicker ON/OFF를 1 ms마다 처리하고, tick10ms가 10 이상이면 일반 색상과 Fade 밝기도 갱신한다.
Process는 LIN D6(Duration) 및 D7(Hz)을 그대로 사용한다.
SRGB는 일반 색상/Fade와 Flicker 변경분을 한 번에 송신한다.
기존 내부 UpdateZone / UpdatePort를 통합했으므로 전체 함수 수는 늘어나지 않았다.
Process 내부의 lastTick1ms 및 실행 주기 비교는 삭제했다.
누적 timeMs는 초기화하지 않고 LIN 명령 시각과 Fade/Flicker의 실제 경과 시간 계산에 사용한다.
일반 색상/Fade는 10 ms 갱신하며 LIN D6 raw 0~250은 유지한다.
10% 단위의 D6 raw는 0, 25, 50 ... 250이다.
현재 PC 가상 시험 결과는 150 PASS / 0 FAIL이다.
각 if문에서 타이머를 0으로 초기화한 뒤 해당 그룹 전체를 한 번에 Read한다.
여유시간 계산, 순환 스케줄 및 항목별 분할 진행 변수는 제거했다.
아래 측정값은 변경 전 구조의 기록이며 현재 구조의 100 Hz 성능 검증 결과가 아니다.
현재 구조는 IAR Debug/Release 빌드와 Main1msTimingModel.cjs의 호스트 로직 시험을 확인했다.
보드 다운로드와 실물 타이밍 재시험은 수행하지 않았다.
전체 그룹 Read가 실행되는 동안에는 실제 LED 출력 전환이 지연될 수 있다.

2026-09-16.
이 문서는 이전 Flicker_Hardware_Timing_20260915.md의 원인 분석 이후 적용한 수정 결과다.

## 적용 기준

| 항목 | 적용 내용 |
|---|---|
| Flicker 주파수 | 0~100 Hz. 0은 OFF |
| PC Duration | ON 비율 0, 10, 20 ... 100% |
| LIN Duration | 기존 D6 raw 0~250 유지. MCU에서 가장 가까운 10%로 반올림 |
| 제어 시간 기준 | SysTick 1 ms. 호출 횟수가 아닌 실제 경과 시간 사용 |
| 일반 색상 / Fade | 10 ms마다 갱신. Fade 시간 입력 1 = 100 ms |
| Fade + Flicker | Fade 밝기 위에 Flicker ON/OFF 적용. 기본 In/Out 500 ms |
| Fade 기억 | Normal + Fade로 시간 설정 후 해당 LED 영역의 In/Out 값 재사용. 재부팅 시 기본값 |
| 같은 명령 반복 | Sync=0이면 진행 유지. 명령 변경 또는 Sync=1이면 재시작 |

Duration 10%는 D6=25, 50%는 D6=125, 90%는 D6=225, 100%는 D6=250으로 송신한다.
주파수 100 Hz는 한 주기 10 ms다.
10%는 1 ms ON / 9 ms OFF를 뜻한다.
1 ms의 정수배가 아닌 목표 구간은 시간 해상도에 따라 양자화된다.
Hz=0은 Duration 값과 관계없이 OFF다.
D6 및 Fade의 D7 범위 초과 입력은 250, Flicker D7 범위 초과 입력은 100으로 제한한다.
RGB 0~255, CIE 좌표 0~250 및 밝기 0~255는 유지했다.

## Read 구조

| 타이머 | 초기 값 | 직접 실행하는 Read | 한 번 호출할 때 |
|---|---:|---|---|
| oAppTimer.tick100ms | 30 | SRGBControl_Read() | 전체 SRGB 진단 및 이미지/설정값 읽기 |
| oAppTimer.tick100ms_1 | 60 | OSPControl_Read(OSP_PORT_LOGO) | 현재 Logo LED의 동적 항목 읽기 |
| oAppTimer.tick100ms_2 | 90 | OSPControl_Read(OSP_PORT_STROBE) | 현재 Strobe LED의 동적 항목 읽기 |

세 변수는 이름과 관계없이 1 ms마다 증가한다.
100 이상이면 해당 그룹의 스캔을 진행한다.
전체 스캔 완료 시 100을 빼서 다음 100 ms 기준을 유지한다.
Process에서는 Read를 실행하거나 Read 요청 플래그를 처리하지 않는다.
main에서 다음 색상/Fade/Flicker 전환 전의 여유 시간을 확인한 후 Read를 호출한다.

OSP는 LED별 온도+상태(42), PWM(4E), Open/Short 상태(46)를 나눠 읽는다.
온도와 상태는 READTEMPST 한 응답으로 읽는다.
응답 payload 순서는 TEMP, STATUS이며 실물 응답으로 확인했다.
SETUP와 OTP는 Init에서 읽고 주기 Read에서 반복하지 않는다.
RX DMA 캡처는 TX 전에 시작하며 응답의 주소/PSI/명령/Manchester/CRC를 검사한다.
LED가 탐지되지 않은 OSP 포트는 Init에서 3회 확인 후 주기 Read를 하지 않는다.

SRGB는 Open, Short, Short-to-GND, Thermal/Watchdog, Smart, REXT/Watchdog 및 이미지/설정값을 나눠 읽는다.
진단 명령은 LAT HIGH에서 14 DCK 펄스 후 해당 수의 DI 펄스로 진입하도록 수정했다.
일반 출력의 zero-data sleep은 해제하고 즉시 프레임 적용을 사용했다.
색상 데이터 앞 24 bit는 SPI 12 MHz, 마지막 8 bit 및 LAT는 GPIO로 출력한다.

100 ms는 그룹별 전체 스캔 목표 주기이며 모든 항목의 동시 샘플링을 뜻하지 않는다.
실측 조건에서는 Read가 계속 진행되고 진단 타이머가 장시간 누적되지 않았다.
서로 다른 6개 영역의 위상/주파수 조합, 통신 오류 또는 추가 작업 부하에서는 스캔 지연이 발생할 수 있다.
출력 전환 마감시간을 우선하므로 스캔 지연 중에는 직전 수신값을 유지한다.
실제 Read 응답 검증에 실패하면 dynamicValid=0 및 통신 오류를 기록한다.

## 실제 보드 / PC 검증

PEAK LIN 19200 bps와 기존 FormCjCIE 송신 경로를 사용했다.
연결된 실물 LED는 Logo 24개뿐이다.
아래는 임시 RAM 계측으로 기록한 OSP 색상 송신 완료 간격이지 광센서의 LED 점등 파형은 아니다.
전체 출력 시험은 SRGB 4영역 및 OSP 2포트의 MCU 송신 경로를 함께 실행한 것이다.
SRGB/Strobe 실물 수신 및 광학 출력은 확인하지 않았다.

| 시험 조건 | 평균 ON / OFF | 평균 Hz |
|---|---|---:|
| Logo 10 Hz / 50% | 49.985 / 50.015 ms | 10.000 |
| Logo 50 Hz / 80% | 16.001 / 3.999 ms | 50.000 |
| 전체 출력 100 Hz / 5% | 0.507 / 9.493 ms | 100.000 |
| 전체 출력 100 Hz / 50%, 반복 LIN 명령 | 4.996 / 5.004 ms | 100.000 |
| 전체 출력 100 Hz / 95% | 9.479 / 0.521 ms | 100.000 |

100 Hz / 50% 반복 송신 시험은 5초 실행했다.
100 Hz 조건의 개별 펄스는 CPU 처리 지연으로 수십 us 정도 흔들렸다.
기본 Fade In은 약 500.0 ms, 기억한 Fade In은 약 200.0 ms에 목표 PWM에 도달했다.
Fade Out 500/300 ms 이후 OFF 상태도 확인했다.
Logo 24개 모두 staticValid=1, dynamicValid=1, ACTIVE 상태 80이며 통신/Manchester 오류는 0이다.
낮은 RED R=10의 MCU RGB 값은 2570, OSP PWM R은 1284다.
Flicker 중 서로 다른 LED의 PWM Read가 ON/OFF로 섞이는 것은 Read 시점이 서로 다르기 때문이다.
실제 점등 파형은 LED 내부 PWM과 드라이버/하네스 영향도 있으므로 최종 실물 부하에서 별도 측정이 필요하다.

PC 가상 시뮬레이션은 157 PASS / 0 FAIL이다.
OSP host 모델은 65,536 payload, CRC, 주소/명령, 반전 극성, 클럭 허용 오차 및 요청 echo 분리를 검증했다.
현재 C의 Manchester/CRC nibble 테이블도 이전 TX LUT와 전수 비교했다.
IAR 및 PC Debug/Release 빌드는 오류 0, 경고 0이다.
PC의 기존 비-CJ 화면/통신 코드는 수정하지 않았다.

## 프로젝트 / 이식 주의

SRGBControl.c와 OSPControl.c만 IAR High/Speed 최적화를 사용한다.
나머지 Debug 코드는 기존 디버그 설정을 유지한다.
다른 프로젝트에 C/H를 복사할 때에도 동일한 CPU 클럭/핀/타이머/DMA 자원과 해당 두 파일의 속도 최적화가 필요하다.
1 ms마다 인자 없이 Process를 호출한다.
Process와 LIN 수신은 공통 oAppTimer.timeMs를 실제 경과 시간으로 사용한다.
OSP Read는 호출할 때마다 해당 포트의 LED 한 개를 읽고 다음 LED로 이동한다.

임시 계측 코드와 계측 헤더는 정식 프로젝트에서 제거했다.
원본 계측/통계 및 수정 전 파일은 프로젝트 상위 tmp/flicker_100hz_20260915에 보관했다.
정식 Debug 펌웨어를 다운로드했고 중단점 없이 실행 중이다.
전체 출력을 OFF한 뒤 Logo만 낮은 정적 RED로 복원했다.
Chair ID는 2를 유지하며 이번 시험에서 ID 변경 요청은 송신하지 않았다.

참고 자료: 데이터시트/AL58263.pdf의 p14, p17~18, p20~24 및 OSIRE E3731i Startup Guide의 p32, p36, p54~56.
