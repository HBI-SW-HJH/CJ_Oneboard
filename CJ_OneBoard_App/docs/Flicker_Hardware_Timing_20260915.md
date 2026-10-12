# OSP Logo Flicker 실제 MCU 타이밍 진단 - 2026-09-15

## 결론

사용자가 느리게 보인다고 한 현상이 실제 MCU 타이밍 계측에서도 재현되었다.
Fade를 끄고 Logo만 낮은 밝기의 빨강으로 제어했다.
현재 진단 Read를 유지하면 10 Hz / Duration 50%가 약 1.81 Hz로 느려진다.
진단 Read만 임시 중지하면 10 Hz / 50%와 50 Hz / 20%, 50%, 80%가 정상적인 간격으로 송신된다.
따라서 주요 원인은 Manchester TX 속도가 아니라 블로킹 진단 Read와 누적 제어 tick 폐기다.
같은 Flicker 명령을 반복 수신할 때 위상을 매번 초기화하는 별도 문제도 확인했다.

이번 요청은 진단/확인이므로 영구 수정은 적용하지 않았다.
계측 및 Read 중지 코드는 제거하고 시험 전 정식 펌웨어로 복원했다.
앞서 수정한 OSP 초기 RESET 전 5 ms 대기와 PC 밝기 0~255 수정은 유지했다.

## 측정 방법과 범위

실제 PEAK LIN, 기존 FormCjCIE의 BuildCJCommand/DoTxImmediate, 실제 MCU를 사용했다.
LIN Baudrate는 19200 bps, 제어 Frame ID는 01 / PID는 C1이다.
현재 저장된 Chair ID는 2로 읽혔으며 ID 변경/UDS/PDS 쓰기는 수행하지 않았다.
전체 Chair 마스크 0F와 Logo 마스크 10을 사용했다.
SRGB/Strobe 실물은 이번 검증 대상이 아니다.

10 Hz / Duration 50% 제어 예시:

```text
Master: BREAK 55 C1 0F 10 16 0A 00 00 7D 0A 77
                   Data 8 Byte             Enhanced Checksum
Data2 = 16: RGB / Active / Flicker, Fade 및 Sync 비트는 0
Data6 = 7D: Duration raw 125 = 50%
Data7 = 0A: 10 Hz
RGB = 10 / 0 / 0, OSP ON PWM R = 1284, OFF PWM R = 0
```

SysTick의 단조 증가 ms와 하드웨어 down-counter를 조합해 us 단위 시간을 기록했다.
OSP_WriteColor의 실제 SPI/DMA Manchester 송신 완료 뒤 색상과 시간을 RAM에 기록했다.
송신/수신 중 중단점을 사용하지 않고 실행한 다음 계측만 비활성화하여 RAM을 읽었다.
따라서 아래 수치는 MCU의 OSP 색상 송신 완료 간격이며 광센서나 오실로스코프의 LED 점등 파형 측정은 아니다.
진단 Read를 켠 시험에서는 실제 LED READ_PWM에 R=0 또는 1284가 반환되었고 Logo 통신/CRC/Manchester 오류는 0이었다.
노드별 Read 시점이 달라 스냅샷 안의 서로 다른 ON/OFF 값은 동시 측정값이 아니다.

각 주요 조건은 약 4~6초 계측했으며 1 Hz 조건의 주기 통계는 한 주기다.
이 수치는 이번 보드/설정에서의 측정값이지 모든 보드와 LED 조합의 보증값은 아니다.

## 현재 정식 로직: 진단 Read 켠 상태

Hz는 초당 ON/OFF 한 주기의 반복 횟수다.
10 Hz의 주기는 100 ms이며 Duration 50%는 50 ms ON / 50 ms OFF다.

| 설정 Hz | Duration raw / 비율 | 정상 목표 ON / OFF | 측정 평균 ON / OFF | 측정 Hz |
|---:|---|---|---|---:|
| 1 | 125 / 50% | 500 / 500 ms | 2778.18 / 2752.90 ms | 0.181 |
| 10 | 50 / 20% | 20 / 80 ms | 109.12 / 441.11 ms | 1.814 |
| 10 | 125 / 50% | 50 / 50 ms | 277.62 / 275.42 ms | 1.808 |
| 10 | 200 / 80% | 80 / 20 ms | 441.92 / 108.75 ms | 1.794 |
| 20 | 125 / 50% | 25 / 25 ms | 133.27 / 140.94 ms | 3.647 |
| 50 | 125 / 50% | 10 / 10 ms | 54.38 / 56.23 ms | 8.962 |
| 50 | 200 / 80% | 16 / 4 ms | 87.78 / 23.11 ms | 9.038 |

ON/OFF 평균과 상승 에지 사이 주기 평균은 시작/끝의 불완전한 구간을 제외하는 대상이 달라 합계가 정확히 일치하지 않을 수 있다.

10 Hz / 50%의 실제 주기는 약 521~593 ms로 변동했다.
단순히 일정한 Hz 환산 계수가 잘못된 것이 아니라 Read 처리에 따른 지연도 변동한다는 의미다.

## 원인별 실행 시간

| 항목 | 실제 측정 |
|---|---|
| Logo 색상 한 번 송신 | 평균 약 0.182 ms, 주요 시험 최대 약 0.187 ms |
| OSP Read 한 명령 | 평균 약 1.51 ms, 최대 약 1.73 ms |
| OSP 한 LED의 동적 Read 5개 | 최대 약 7.64 ms |
| SRGB 전체 동적 Read | 최대 약 3.32 ms |
| 1 ms 제어 함수 실행 간격 | 최대 약 11.2 ms |
| 10 Hz / 50% 시험의 처리된 tick | 1168회 |
| 같은 시험에서 버려진 누적 tick | 5285개 |

OSP_Transfer는 응답이 빨리 와도 요청마다 고정으로 약 1 ms를 기다린 다음 캡처를 복원한다.
OSP_ReadDynamicNode는 온도/상태/PWM/LED 상태/통신 상태의 5개 명령을 연속 실행한다.
main에서는 실제 7~11 ms가 지나 tick1ms가 여러 개 쌓여도 tick1ms=0으로 지우고 Process를 한 번만 실행한다.
그러나 Flicker 위상은 Process 호출 한 번당 1 ms만 진행한다.
실제 시간보다 위상 시간이 느리게 흘러 Flicker와 같은 경로의 Fade도 지연될 수 있다.

24개 Logo를 매번 5종 읽을 때 고정 대기만 120 ms다.
따라서 지금처럼 Read를 100 ms마다 요청한다고 모든 LED 진단값이 100 ms마다 갱신되는 것은 아니다.
현재 구조에서는 응답 복원과 기타 처리 시간이 추가되어 전체 스캔이 더 오래 걸린다.

## 원인 분리: 진단 Read만 임시 중지

아래는 수정 완료된 정식 제품 동작이 아니라 원인 분리를 위한 임시 시험 결과다.
새로운 명령이 적용된 후의 같은 조건 구간만 주기 통계에 사용했다.

| 설정 Hz | Duration raw / 비율 | 측정 평균 ON / OFF | 측정 Hz |
|---:|---|---|---:|
| 10 | 125 / 50% | 49.997 / 50.003 ms | 10.000 |
| 50 | 50 / 20% | 3.996 / 16.004 ms | 50.000 |
| 50 | 125 / 50% | 9.996 / 10.004 ms | 50.000 |
| 50 | 200 / 80% | 16.025 / 3.975 ms | 50.000 |

Read를 중지한 정상 구간에서 1 ms Process 간격은 최대 약 1.03 ms였다.
임시 Read 중지 설정을 정식 소스나 보드에 남겨두지 않았다.

## Duration과 1 ms 해상도의 한계

현재 Hz 입력은 0~50이며, 0은 Flicker 출력을 끈다.
Duration raw는 0~250, 비율은 raw x 0.4%다.
raw 0은 OFF, raw 250은 계속 ON이다.
1 ms 제어에서는 ON/OFF 각각 1 ms보다 짧은 구간을 정확히 표현하지 못한다.

| 50 Hz에서의 설정 | 이상적인 목표 | Read 중지 시험에서 실제 송신 |
|---|---|---|
| raw 1 / 0.4% | 0.08 ms ON / 19.92 ms OFF | 약 0.996 ms ON / 19.004 ms OFF, 약 5% |
| raw 50 / 20% | 4 ms ON / 16 ms OFF | 약 4 ms / 16 ms |
| raw 125 / 50% | 10 ms ON / 10 ms OFF | 약 10 ms / 10 ms |
| raw 200 / 80% | 16 ms ON / 4 ms OFF | 약 16 ms / 4 ms |
| raw 249 / 99.6% | 19.92 ms ON / 0.08 ms OFF | OFF가 표현되지 않아 계속 ON |
| raw 0 / 0% | 계속 OFF | PWM 0 |
| raw 250 / 100% | 계속 ON | PWM 1284 |
| Hz 0 / raw 125 | Flicker OFF | PWM 0 |

이는 0.4% 단위로 프로토콜 값을 입력할 수 있다는 것과 그 모든 비율을 실제 출력할 수 있다는 것이 다름을 뜻한다.
50 Hz에서는 1 ms가 주기의 5%다.
1 ms 방식으로 모든 Duration 0.4% 단계를 보장할 수는 없다.

## 같은 명령 반복 수신 시험

PC에서 10 Hz / raw 125 / Sync=0 명령을 약 10 ms마다 반복 송신했다.
실제 LIN 수신 뒤 OSP_SetPort가 매번 elapsedMs와 flickerPhase를 0으로 초기화한다.
같은 Flicker 설정 구간에서 송신된 256개 PWM은 모두 R=1284였고 OFF가 없었다.
초기화 직전 이전 설정의 기록은 이 판단에서 제외했다.
따라서 진단 Read 지연을 해결하더라도 반복 명령의 위상 초기화도 별도로 정리해야 한다.
SRGB_SetZone에도 같은 위상 초기화 로직이 있으나 SRGB 실물 파형은 이번에 측정하지 않았다.

## 다음 수정 방향 - 아직 미적용

1. 응답을 받은 뒤에도 고정 대기를 유지하는 Read 구조를 줄이고, 진단 처리를 제어 마감시간 안에 분산한다.
2. 실제 경과 시간을 버리지 않도록 제어 시간을 관리한다.
   누적 시간을 반영하는 것만으로 평균 Hz는 개선될 수 있지만 7 ms 블로킹이 남으면 4 ms OFF 구간은 여전히 놓칠 수 있다.
3. 같은 명령이 반복되어도 기존 Flicker/Fade 진행을 유지하도록 한다.
   명령 변경 및 명시적인 동기화 요청 때의 재시작 정책은 구분해야 한다.
4. 진단을 유지한 채 10/20/50 Hz와 Duration을 다시 계측한다.
5. SRGB와 Strobe까지 실제 연결한 최종 부하 및 LED 점등 파형은 별도 검증한다.

## 복원과 원본 자료

main.c, sys_init.c, OSPControl.c를 시험 전 내용으로 복원했다.
임시 계측 헤더는 정식 프로젝트에서 제거하고 계측 소스 사본은 진단 폴더에 보관했다.
정식 Debug 빌드는 오류 0, 경고 0이다.
다운로드 첫 시도에서 flash-loader 통신 timeout이 있었으나 안전하게 CPU를 중단하고 재시도하여 성공했다.
최종 보드는 정식 펌웨어와 낮은 밝기의 정적 RED 명령으로 실행한다.
저장 메모리 및 Chair ID는 시험 중 변경하지 않았다.

원본 폴더: 프로젝트 상위 tmp/flicker_timing_20260915.

- Timing_Analysis.log 및 각 조건의 .bin: RAM 계측 원본과 통계
- 각 조건의 _LIN.log: 실제 PC 송신 로그
- 각 조건의 _OSP.bin: LED 수신 구조체 스냅샷
- Main_Probe.c, OSPControl_Probe.c, SysInit_Probe.c, FlickerTimingProbe.h, Probe.map: 임시 계측 소스/주소
- Production_Restored_Download_Retry.log: 정식 펌웨어 복원 다운로드
- tools/CJFlickerHardwareTest.cs: 기존 PC 화면 송신 경로를 호출하는 시험 도구, 제품 빌드에는 포함되지 않음
