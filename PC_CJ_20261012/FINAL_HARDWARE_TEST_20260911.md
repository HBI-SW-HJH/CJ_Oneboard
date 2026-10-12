# Sunshine AD3304 + LAN8661 최종 시험 결과 (2026-09-11)

## 시험 환경

- Sunshine MCU: PIC32CM5164JH, J-Link SWD
- PC 연결: USB COM12, 460800 bps
- LED 시험 밝기: 10 (30% 미만)
- 연결 AD3304: FID2, FID3, FID4 (`DiscoveredMask=0x0E`, `InitializedMask=0x0E`)
- 연결 LAN8661 앱 보드: 1대 (가상 FID1)

## 결과

| 구성 | R/G/B | Fade | Shift | Shift Stack | Shift Up | Shift Stack Up | Move | OSP Read | ISE Read |
|---|---|---|---|---|---|---|---|---|---|
| ADI 단독 | 통과 | 통과 | 통과 | 통과 | 통과 | 통과 | 통과 | 75/75 | 실장 LED 241/241 정상, FID3 LED1~11 하드웨어 누락 제외 |
| LAN8661 단독 | 통과 | 통과 | 통과 | 통과 | 통과 | 통과 | 통과 | 12/12 | 27/27 |
| ADI + LAN8661 | 통과 | 통과 | 통과 | 통과 | 통과 | 통과 | 통과 | 87/87 | 실장 LED 268/268 정상, 동일한 FID3 LED1~11 하드웨어 누락 제외 |

동시 시험에서 OSP는 LAN FID1 12개와 ADI FID2~4 각 25개가 모두 조회됐다. ISE는 LAN FID1 27개와 ADI FID2~4 각 84개가 모두 열거됐지만, ADI FID3 LED1~11의 PWM 응답만 반복해서 누락됐다. 수신 대기시간 및 OA-SPI 회수 청크 수를 늘린 재시험에서도 누락 위치가 같아 PC/USB 시간 부족 원인은 배제했다.

## 1 ms 요청의 실제 효과 갱신 주기

| 구성 | 실제 주기 |
|---|---:|
| ADI OSP 3채널 | 12.0 ms |
| ADI OSP + ISE 3채널 | 15.9 ms |
| LAN8661 OSP | 11.1 ms |
| LAN8661 ISE | 5.4 ms |
| LAN8661 OSP + ISE | 15.9 ms |
| ADI + LAN8661 OSP + ISE | 23.3 ms |

요청값 1 ms는 스케줄 요청 간격이며, 모든 LED의 직렬화된 Ethernet/OA-SPI/SPI 전송 시간이 더 길면 실제 갱신 주기는 처리시간으로 제한된다.

## 빌드 및 시뮬레이션

- IAR Debug: 오류 0, 경고 0
- PC Release x86: 오류 0 (Microchip SDK `strncpy` 기존 경고 3개)
- CJ 프로토콜 시뮬레이션: PASS 79, FAIL 0
- USB 실보드 전체 시나리오: ADI 단독 0 실패, LAN8661 단독 0 실패, 동시 선택 0 실패
- 최종 `SunShine.out`을 J-Link로 Program/Verify하고 실행 상태로 전환함

## 코드 구조 정리

- `main.c`: AD3304 연결 Flag와 LAN8661 연결 Flag를 각각 판정한다. 공용 색상/효과 계산은 한 번만 실행해 동시 선택 시 두 백엔드의 Shift 위치가 어긋나지 않게 했다.
- `AD3304_LED_Control.c/.h`: AD3304 전용 LED 송신 경계다.
- `Lan8661_Init.c/.h`: LAN8661 SOME/IP 연결·SPI handle·수신 유지보수 전용이다.
- `Lan8661_LED_Control.c/.h`: LAN8661 OSP/ISE 송수신 전용이다.
- `Lan8661_LED_Service.c/.h`: LAN8661 LED Read 상태 전용이다.
- `LED_Backend.c/.h`: PC가 선택한 ADI/LAN/Both를 명시적으로 라우팅한다. `AD3304_Initialize()`와 `Lan8661_Initialize()`를 독립 실행하고 각각의 Connect Flag를 따로 설정한다.
- LAN 전용 동작을 숨기던 `LED_BackendServiceProcess()`를 제거했다. 메인에서 `Lan8661_Process()`를 LAN 연결 Flag로 직접 호출한다.
- 비어 있던 `AD3304_LED_Service.c/.h`와 IAR 프로젝트 항목을 제거했다.
- 핵심 제어 파일의 여러 줄 `if` 조건은 한 줄로 통일했다.

## ISE Tune R/G/B 및 Read 속도 최종 확인

PC의 ISE Read 표가 수신된 `PWMMax` 값을 사용하지 않고 Tune 열을 항상 `-`로 표시하던 문제를 수정했다. 펌웨어가 반환한 PWMMax R/G/B와 채널 유효 비트를 그대로 표시한다.

| 구성 | 실제 Read 시간 | Tune 확인 결과 |
|---|---:|---|
| ADI 단독, FID2 84개 | 약 0.4초 | LED1 `2642/2563/3004`, R/G/B 유효 |
| LAN8661 단독, FID1 27개 | 약 0.4초 | LED1 `2647/2474/2831`, R/G/B 유효 |
| ADI + LAN8661, 총 279개 | 약 2.9초 | FID1/2/4 및 FID3의 실장 LED에서 Tune 표시 확인 |

동시 Read는 두 번 연속 실행해 모두 약 2.9초로 확인했다. ADI FID3 LED1~11의 일부 채널 누락은 확인된 LED 하드웨어 상태이므로 이번 수정의 실패로 판정하지 않는다.

## OSP Read 속도 최적화

일반 OSP Read가 초기화 때 저장된 Tune/OTP 캐시를 사용하도록 변경했다. 캐시가 없는 FID만 펌웨어가 자동으로 OTP를 다시 읽는다. 또한 CRC와 READ_PWM opcode가 정상인 응답을 전류 모드 비트 차이만으로 폐기하고 세 번 재시도하던 조건을 제거했다.

| 구성 | 변경 전 | 변경 후 | 결과 |
|---|---:|---:|---|
| ADI FID2, OSP 25개 | 약 3.1초 | 약 1.1초 | 25/25, 오류 0 |
| LAN8661 FID1, OSP 12개 | - | 약 0.4초 | 12/12, 오류 0 |
| ADI + LAN8661, OSP 총 87개 | 약 9.9초 | 약 3.8초 | 87/87, 오류 0 |

전체 OSP Read는 두 번 연속 약 3.8초로 확인했다. 실시간 PWM·온도·Open/Short 상태는 계속 실제 LED에서 읽고, 정적인 Tune/OTP만 초기화 캐시를 사용한다.

## 확인된 제한

- 펌웨어의 AD3304 테이블은 최대 8 FID를 지원하며, 현재 실물 3대가 검색·초기화됐다.
- 현재 LAN8661 구현은 `192.168.0.50` 한 엔드포인트와 한 쌍의 SPI handle을 사용하는 단일 보드 구조다. 실물도 1대뿐이므로 LAN8661 4대 동시 검색/초기화는 구현·검증 완료로 판정할 수 없다. 4대 지원에는 각 보드의 고유 IP/MAC, 가상 FID 규칙 및 보드별 SOME/IP 세션/handle을 먼저 확정해야 한다.
