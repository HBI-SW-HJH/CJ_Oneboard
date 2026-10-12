# CJ Boot / App / PC 사용 안내

## 2026-10-01 최신 변경: 이전 APP 자동 복원

이 절과 [복원 검증 결과](validation/Rollback_20261001/RESULTS.md)가 최신 기준입니다. 아래 1~8절의 이전 448KB App 구성과 기존 시험 기록은 변경 이력입니다.

- Boot: `0x00000000~0x0000FFFF`, APP: `0x00010000~0x0003FFFF` (192KB 제한).
- `0x00040000~0x0004FFFF`는 미사용 여유 영역으로 남깁니다.
- 복원 APP: `0x00050000~0x0007FFFF` (192KB, Region 10~15).
- 정상 실행이 확인된 APP을 업데이트 전에 복원 영역으로 복사합니다. 32KB Region별로 진행하며 실제 쓰기는 256B Row 단위입니다.
- Flag=2이면 Boot에서 백업 CRC/벡터 확인 → APP으로 복사 → CRC 확인 → 누적 복원 횟수 +1, HardFault Count=0, Flag=0 저장 → Reset 순서로 복원합니다.
- 복원 중 Reset/전원 차단 시 Flag=2가 유지되어 다음 부팅에서 다시 복사합니다. 마지막 두 행 기록 중 한쪽만 완료되면 CRC와 index로 최신 기록을 선택하고 나머지 행을 맞춥니다.
- 복원본은 지우지 않습니다. 같은 복원본을 사용한 뒤 다시 HardFault 3회가 발생하면 무한 복원을 막기 위해 Boot에 남습니다.
- Flag=1 업데이트 중단 시에는 기존처럼 Boot에서 재다운로드를 기다립니다. 부분 다운로드 APP으로 정상 복원본을 덮어쓰지 않습니다.
- APP에는 기존 10초 정상 실행 판정에서 백업 가능 표시를 저장하는 처리와 F210 복원 정보 응답이 추가되었습니다. LED/색상/진단/PDS 제품정보 제어 소스는 변경하지 않았습니다.
- PC는 APP 정상 확인 후 업데이트를 진행하고, F210에 복원 횟수/백업 크기를 표시합니다. 0x34에는 백업 시간을 고려해 최대 25초 응답 대기를 적용하며 Stop은 계속 사용할 수 있습니다.
- 기존 12/29Byte 상태 응답도 PC에서 읽습니다. 새 복원 횟수 표시는 최신 APP/Boot의 38Byte 응답이 필요합니다.
- 최초 적용에는 최신 Boot와 APP이 모두 필요합니다. 기존 R/G/B/W 시험 BIN은 이번에 다시 만들지 않았습니다. 최신 APP은 `CJ_OneBoard_App_20260930/build/Debug/Exe/CJ_App.bin`입니다.
- 이번에는 빌드와 호스트 모의시험만 수행했습니다. 실제 보드 다운로드, Flash 실측 시간 및 물리적 전원 차단 검증은 수행하지 않았습니다.

## 1. 프로젝트 구분

| 프로젝트 | 역할 | 실행/업데이트 파일 |
|---|---|---|
| CJ_OneBoard_Boot_20260930 | App 검사, LIN UDS 검색, 펌웨어 수신·저장 | build/Debug/Exe/CJ_Boot.bin |
| CJ_OneBoard_App_20260930 | 기존 LED 동작 + 업데이트 요청 시 Boot 진입 | build/Debug/Exe/CJ_App.bin |
| PC_CJ_20260930 | UDS 제품 선택 후 Update | bin/Debug/LINMaster.exe |

IAR 프로젝트와 워크스페이스 이름은 `CJ_OneBoard_APP.ewp/.eww`, `CJ_OneBoard_Boot.ewp/.eww`입니다.
폴더 경로는 그대로 유지했습니다. 기존 이름으로 열려 있는 IAR 창은 닫고 새 `.eww`를 여십시오.
OUT/MAP도 새 프로젝트 이름으로 생성됩니다. PC 업데이트 BIN 이름은 `CJ_App.bin`, `CJ_Boot.bin`을 유지합니다.
각 프로젝트의 `applications/src/Reprogram.c`, `applications/inc/Reprogram.h`가 업데이트 담당 파일입니다.
PC는 `Forms/FormUDS_Update.cs`와 기존 UDS 화면의 Designer 버튼을 사용합니다.

## 2. 메모리와 IAR Linker 설정

| 영역 | 시작 | 끝 | 크기 |
|---|---|---|---|
| Boot | 0x00000000 | 0x0000FFFF | 64 KB |
| App | 0x00010000 | 0x0007FFFF | 448 KB |
| RAM | 0x20000000 | 0x2000FFFF | 64 KB |
| 기존 PDS Backup / Main | 0x00400100 / 0x00401000 | 각 시작 + 0xFF | 각 256 B |
| 업데이트 Backup / Main | 0x00400F00 / 0x00401E00 | 각 시작 + 0xFF | 각 256 B |

Data Flash 행 번호는 0부터 셉니다. 업데이트는 지정한 15·30번 행만 사용합니다.

IAR **Project → Options → Linker → Config → Edit**에서 각 프로젝트의 `flash.icf` 설정을 확인·변경합니다.
Debug와 Release 모두 같은 프로젝트 내 `flash.icf`를 참조합니다.
App의 Interrupt vector 시작도 `0x00010000`입니다.
제조사 ICF의 배치 방식은 유지하고, IAR Edit에서 보이는 ROM·벡터 주소만 Boot/App에 맞춰 사용합니다.
별도 추가했던 `__app_start__`, `__app_end__` 링커 심볼은 제거했습니다.
업데이트 코드에서 사용하는 시작·끝 주소는 양쪽 `Reprogram.h`의 `REPROGRAM_APP_START=0x00010000`, `REPROGRAM_APP_END=0x0007FFFF`입니다.
끝 주소는 마지막 바이트를 포함합니다. 코드의 주소 상수만 변경해서는 실제 프로그램 배치가 바뀌지 않습니다.

2026-09-30 최신 Debug 빌드의 Boot BIN은 10,340 B, App BIN은 25,465 B입니다. Boot 64 KB 영역을 유지했습니다.
나중에 경계를 변경할 때는 양쪽 `Reprogram.h`와 IAR Linker Edit의 ROM·벡터 주소를 함께 맞춰야 합니다.
추가로 App 디버거의 벡터 주소, 각 `debugger/*.board` 다운로드 범위, `CJ_PIC32CM_BootUnlock.mac`의 BOOTPROT도 함께 맞춰야 합니다.
현재 64 KB Boot 보호 설정은 BOOTPROT=4입니다.

## 3. 사용하는 순서

1. 처음 한 번 IAR에서 Boot를 다운로드합니다.
2. PC에서 LIN 연결 → CJ UDS → Scan → 대상 제품을 선택합니다.
3. **Update**를 누르고 **App 프로젝트의 `CJ_App.bin`**을 선택합니다.
4. 전송 → Flash CRC 검사 → 두 업데이트 기록 저장 → MCU Reset → App 실행 순서로 진행합니다.

App이 없어도 Boot에서 제품 검색과 Update가 가능합니다.
4대가 연결되어 있어도 선택한 제품의 `Init_Number`가 일치하는 보드만 업데이트에 응답합니다.
의자 ID는 통신 주소로 사용하고, Init_Number로 대상 하나를 선택합니다.
진행 중에는 다른 LIN 도구에서 UDS 요청을 보내지 마십시오.

**초도 보드는 IAR에서 Boot → App 순서로 다운로드한 뒤 Reset하여 App을 실행할 수 있습니다.**
두 Flag가 동일하고 값이 1이 아니면 App 진입을 허용합니다. 빈 Data Flash의 `0xFFFFFFFF / 0xFFFFFFFF`도 허용합니다.
App이 없거나 기본 벡터(스택·Reset 시작주소)가 유효하지 않으면 Boot에서 대기합니다.
업데이트 중단으로 Flag=1이 남았거나 두 Flag가 다르면 IAR로 App만 다시 써도 Boot에서 대기하므로 PC Update로 복구합니다.

## 4. 실패했을 때

```text
업데이트 시작 → 두 기록 Flag=1 저장·확인 → App Flash 기록
                                            ↓
                           전체 CRC·벡터 검사 성공
                                            ↓
                           두 기록 Flag=0 저장·확인 → Reset → App

전송 중단 / CRC 실패 / 두 Flag 불일치 → Boot 대기 → PC Scan → Update 재시작
```

두 기록에는 Flag뿐 아니라 주소·길이·이미지 CRC·기록 CRC가 들어갑니다.
**부팅은 두 Flag가 동일하고 1이 아니며 App 기본 벡터가 유효하면 허용합니다.**
UDS 업데이트 완료 시 전체 App CRC와 두 기록 저장 검사는 유지하지만, 매 부팅 시 저장 기록·App CRC를 재검사하지는 않습니다.
실패한 지점부터 이어받는 방식은 아닙니다. 새 Update는 BIN 처음부터 다시 전송합니다.
이전 App을 보관하는 A/B 복구 방식도 아닙니다.
Boot와 기존 제품정보 PDS는 App 업데이트 범위에 포함되지 않습니다.

## 5. 추가 UDS 형식

LIN 0x3C 요청 / 0x3D 응답, Classic checksum, 기존 LIN 다중 프레임 전송을 사용합니다.
아래는 NAD·PCI를 제외한 UDS 데이터입니다.

| 순서 | 요청 | 의미 |
|---|---|---|
| 대상 선택 | 31 01 F3 02 + Init_Number 4 B | 지정한 보드만 선택. F302는 CJ 사용자 지정 Routine |
| 영역 확인 | 22 F2 10 | Boot/App 상태, App 시작주소·용량. F210은 CJ 사용자 지정 DID |
| 세션 진입 | 10 02 | Programming session. App이면 Flag 저장 후 Reset |
| 다운로드 시작 | 34 00 44 + 주소 4 B + 크기 4 B | App 범위 확인, Flag=1 저장 |
| 데이터 전송 | 36 + 블록번호 1 B + 데이터 최대 128 B | 블록별 응답 확인 후 다음 블록 전송 |
| 다운로드 종료 | 37 + CRC32 4 B | 전체 BIN 검증. CRC 파라미터 형식은 CJ 사용자 지정 |
| 재시작 | 11 01 | 정상 완료 응답 송신 후 MCU Reset |

UDS 안의 주소·길이·Init_Number·CRC는 상위 바이트부터 보냅니다.
CRC는 CRC32/IEEE입니다. 응답이 없으면 PC는 최대 3회 시도합니다.
Boot는 마지막 블록의 번호·길이·CRC가 같은 재전송을 중복 저장하지 않습니다.
5초간 요청이 없으면 선택/전송 세션은 해제되지만 Flash의 업데이트 Flag는 유지됩니다.
Init_Number는 대상 식별용이며 보안 인증이 아닙니다. 암호화·서명·SecurityAccess는 이번 구현에 포함되지 않습니다.

## 6. 실제 확인 결과

### 6-1. 현재 수정본: 연속 업데이트 10회

2026-09-30, 최신 Boot/App Debug 빌드(각 오류 0 / 경고 0)를 IAR 다운로드 방식으로 설치했습니다.
실제 PEAK LIN과 PC 프로그램의 업데이트 함수를 사용했습니다. 화면 클릭 모의시험이 아니라 보드에 실제로 전송·저장한 결과입니다.

- 대상: Chair 0x0D / Init_Number 62152715. 4대 연결 상태에서 이 보드만 선택했습니다.
- 초기 Flag 두 행을 모두 0xFF로 지운 뒤 IAR Boot → App 설치 및 Reset: App 진입 성공.
- 동일한 최신 CJ_App.bin을 연속 10회 업데이트: **성공 10 / 실패 0**.
- 매 회 199개 블록, 총 25,465 B 전송과 응답 확인. App 복귀와 제품 정보 유지도 10회 모두 확인.
- 업데이트 시간: 평균 **108.33초**, 최소 103.2초 / 최대 122.5초. 최초 Scan 시간 제외.
- 마지막 Flash 직접 읽기: Boot/App 파일 일치, Boot 영역 유지, 제품 PDS 두 행 유지.
- 두 업데이트 기록 동일, Flag=0, App CRC32=908C7C80, 기록 CRC32=B4FE7B25.
- 최종 Main Flash LOCK=0000, User Row BOOTPROT=4. User Row는 시험 전 값과 동일.
- 업데이트 후 기존 LED 자동검사: **114 Pass / 0 Fail**, 상세 696개. 검사와 성적서 저장 총 39.133초.
- LED 검사는 기존 전기적/소프트웨어 자동검사이며 광학 측정이 아닙니다.
- 2회차 종료 처리 중 FF 수신 한 번을 PC가 무시하고 다음 응답 요청에서 정상 77 응답을 받았습니다. 데이터 블록 재전송·UDS 거절·업데이트 재시도는 없었습니다.

회차별 결과, 통신 로그, 설치 전후 메모리와 비교 결과:
[최신 10회 시험 기록](validation/Update10_20260930_161143/UPDATE_10_RESULTS.md).
이번 연속시험 중 실제 전원 차단 시험은 하지 않았습니다.

### 6-2. 이전 구조에서 수행한 시험

아래 실측은 코드 분리·프로젝트 이름 변경 전 버전에서 수행했습니다.
프로젝트 분리 직후에는 App/Boot Debug·Release 빌드와 소스·링커 확인까지 수행했습니다.
아래는 과거 이력이며 현재 수정본의 시험 결과는 6-1절입니다.

- 대상: Chair 0x0D / Init_Number 62152715. 다른 3대는 다운로드하지 않았습니다.
- IAR Boot/App Debug·Release: 오류 0, 경고 0. PC Debug 빌드 성공.
- 정상 App → Boot → PC Update → App: 성공. 최신 25,689 B BIN 기준 약 107.2초.
- 256 B만 보낸 뒤 전송 중단 → 하드웨어 Reset → Boot 검색 → 재다운로드: 성공.
- 최종 Flash를 읽어 BIN 전체 일치, 두 Flag=0 기록 및 CRC 일치 확인.
- 기존 PDS 두 행은 변경 전과 완전히 동일. Data Flash는 15·30번 행만 변경.
- 업데이트 후 기존 LED 자동검사: **114 Pass / 0 Fail**, 성적서 생성 포함 **29.692초**.
- 실제 전원 차단 중 Flash 기록 시험은 수행하지 않았습니다. 전송 중단과 Reset 복구를 시험했습니다.

상세 통신 로그는 PC의 `bin/Debug/UpdateLogs`, 검사 결과는 `bin/Debug/Reports`에 있습니다.
실패한 하드웨어 디버깅 시도는 5/50회이며 모두 초기 IAR 다운로드 매크로 문제였습니다.
Boot에서 제거한 App 전용 폴더는 `validation/Boot_removed`에 이동해 복구 가능하게 보관했습니다.
작업 전 전체 프로젝트 백업은 `D:/jhhan/program/CJ_OneBoard/tmp/CJ_Boot_before_20260930.zip`입니다.

## 7. Boot / App 전용 코드 정리

- `REPROGRAM_BOOT_BUILD`와 해당 조건부 컴파일 분기를 모두 제거했습니다.
- App: 대상 선택, 메모리 정보 응답, 두 업데이트 Flag 저장, Reset만 유지합니다. Boot 전용 BIN 수신·App Flash 쓰기·App CRC 검사·App 점프 코드는 제거했습니다.
- App: 미사용 수신 버퍼와 세션·블록 상태, 불필요한 초기 Flash 읽기를 제거했습니다. 기록 CRC는 기존 CRC32 함수를 사용합니다.
- Boot: 실제 BIN 수신·저장·검증·App 진입 기능을 유지합니다. App의 업데이트 진입용 Flag 저장 분기와 LED 전용 타이머·미사용 입력 읽기를 제거했습니다.
- 저장 기록의 256 B 구조는 Boot와 App이 같은 Flash 데이터를 읽어야 하므로 동일하게 유지합니다.
- 기존 제품 PDS, LED 제어, 자동검사, XCP 소스는 변경하지 않았습니다. PC 프로그램도 이번 정리에서는 변경하지 않았습니다.
- 이전 이름의 OUT/MAP 등 산출물은 `validation/Previous_project_outputs`로 이동했습니다. 복구할 수 있습니다.

## 8. 주소·보호 코드 정리

- Boot의 `SYS_Init()` 첫 줄에서 `Reprogram_Init()`을 실행합니다. App 진입 판단은 Clock·타이머·LIN 초기화보다 먼저 수행합니다.
- Boot `Reprogram_WriteProtect_Lock()`는 전체 `0x00000000~0x0007FFFF`(512 KB)를 잠급니다. `Reprogram_WriteProtect_Unlock()`는 App 전체 `0x00010000~0x0007FFFF`(448 KB)를 해제합니다. 두 함수 모두 인자는 없으며 구역 반복은 함수 내부에서 처리합니다. Boot 64 KB는 Unlock 대상에 포함하지 않습니다.
- `Reprogram_Init()`은 `Reprogram_WriteProtect_Lock()`을 한 번 호출합니다. 이미 모두 잠겨 있으면 잠금 명령은 생략합니다. 보호에 실패하면 App으로 진입하지 않고 Boot에 남습니다.
- 이 C 함수들의 LR/UR 명령은 실행 중 보호용이며 User Row를 지우거나 쓰지 않습니다. 전원 차단 후에도 유지되는 설정은 양쪽 `debugger/CJ_PIC32CM_BootUnlock.mac`에서 관리합니다.
- IAR 다운로드 전 BOOTPROT=`0xF`로 해제하고, 다운로드 후 `0x4`(Boot 64 KB 보호)를 저장합니다. BOOTPROT 재설정은 다음 Reset부터 적용됩니다. REGION_LOCKS=`0x0000`으로 Main Flash 16개 영역의 비휘발성 보호 설정을 유지합니다.
- LIN 업데이트는 0x34에서 두 Flag=1 저장에 성공한 뒤 App 전체를 한 번 해제하고, 0x37에서 이미지 확인 후 전체를 잠근 다음 Flag=0을 저장합니다. 256 B 행 쓰기 함수에서는 Lock/Unlock을 호출하지 않습니다.
- 요청에 부정 응답을 보내거나 대상을 다시 선택하거나 세션을 다시 시작하면 전송을 중단하고 전체를 잠급니다. 5초간 업데이트 요청이 없을 때도 전체를 잠급니다. 중단 후에는 0x34부터 다시 다운로드합니다. 잠금에 실패하면 정상 완료로 응답하지 않으며, 전송하지 않는 동안 남은 해제 상태를 다시 잠그도록 합니다.
- Boot 영역은 업데이트 대상으로 허용하지 않습니다. Data Flash의 PDS·업데이트 Flag는 Main Flash 보호 대상이 아닙니다.
- LED·PDS·LIN/UDS 패킷·PC 프로그램은 이번 정리에서 변경하지 않았습니다.
- 최신 Debug BIN/OUT으로 IAR 다운로드와 LIN 연속 업데이트 10회를 완료했습니다. 결과는 6-1절에 기록했습니다.
- 수정 전 파일은 `validation/Before_address_cleanup_20260930_153556`에 보관했습니다.
- Boot/App IAR 프로젝트에서 `Reprogram.h`는 `applications/inc`, `Reprogram.c`는 `applications/src`에 등록했습니다. 이전 `Firmware Update` 그룹은 제거했습니다. 열린 IAR 프로젝트는 다시 열어 변경된 그룹 구성을 반영하십시오.
- 함수 분리 전 파일과 IAR 프로젝트는 `validation/Before_lock_split_20260930_154740`에 보관했습니다.
