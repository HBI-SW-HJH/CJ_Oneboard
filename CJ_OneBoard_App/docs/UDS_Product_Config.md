# CJ 제품 정보 저장 및 UDS 통신

## 저장 구조

- `ConFig_t`: Version 2B + Serial 11B(종료문자 포함) + Part 9B(종료문자 포함) + Chair 1B + Init 4B = 27B.
- 기본 Part Number는 `ZX1-0025`이며 표시·통신 길이는 8자다.
- Data Flash의 기존 System Main/Backup 256B Row와 CRC/index 관리 방식은 유지한다.
- `PDS_CONFIG_FORMAT = 2`. 형식 1의 9자 Part 레코드는 Serial·Chair·Init을 보존하고 새 Part로 변환하여 저장한다.
- 형식 1의 Init이 없는 24B 레코드는 기존 UID 기반 Init 생성 방식을 사용한다.
- App의 Version은 기존처럼 실행 중인 펌웨어의 `CONFIG_VERSION`을 따른다. Boot는 저장된 Version을 보고한다.
- 정상 변환된 레코드는 재부팅할 때 다시 쓰지 않는다.

## 구조체 확장 규칙

새 필드는 `ConFig_t` 맨 뒤에 추가한다. 저장된 길이 뒤의 필드는 초기 기본값을 유지한 뒤 저장한다.
기존 필드 크기·순서·의미 변경 시 형식 번호를 올리고 별도 변환 코드를 둔다.
포인터와 가변 길이 데이터는 저장하지 않는다.

## 제품정보 데이터: 29바이트

| 위치(0부터) | 내용 | 길이 |
|---|---|---:|
| 0..9 | Serial ASCII | 10 |
| 10 | `*` | 1 |
| 11..18 | Part ASCII: `ZX1-0025` | 8 |
| 19 | `*` | 1 |
| 20..21 | Version, Big-endian packed nibble | 2 |
| 22 | `*` | 1 |
| 23 | Chair 0x0A..0x0D | 1 |
| 24 | `*` | 1 |
| 25..28 | Init Number, Big-endian | 4 |

- F202 Read: `22 F2 02` → `62 F2 02 + 제품정보`, 총 32B(App/Boot).
- F202 Write: `2E F2 02 + 제품정보`, 총 32B(App). Version·Init은 변경할 수 없다.
- F205 Target Write: `2E F2 05 + 제품정보`, 총 32B(App). Init이 일치하는 보드만 저장한다.
- F300 Scan / F301 Target Read 응답: `71 03 F3 00/01 + 제품정보`, 총 33B(App/Boot).
- F300 요청 5B, F301 요청 8B, F302 대상 선택 및 F210 상태 형식은 유지한다.
- PC의 Scan, Read, Write 후 재확인, Update 후 제품정보 비교, QR/성적서는 동일 ProductInfo 해석을 사용한다.
- LIN 요청/응답 ID 0x3C/0x3D, Classic checksum 및 FF/CF 분할 방식은 유지한다.
- 구형 9자 통신 형식과 호환되지 않으므로 Boot/App/PC를 함께 교체해야 한다. 첫 적용 시 구형 App이나 구형 자동복원 이미지를 실행하지 않는다.

## Logo 전류 제한

`OSPControl.h`의 `OSP_LOGO_CURRENT_LIMIT_MA = 41U` 한 값으로 변경한다(0..150).
Day mode의 채널당 50mA를 기준으로 RGB 평균 합계 전류를 계산한다.
`OSP_WriteColor()`에서 Logo PWM 합계가 상한을 넘으면 RGB를 동일 비율로 낮춘 뒤 송신한다.
Strobe PWM은 그대로이며 최대 합계는 명목 150mA다.
송신·진단 기대 PWM은 제한 후 값으로 일치한다. 실제 전류 측정이나 과온 차단을 추가하는 변경은 아니다.
