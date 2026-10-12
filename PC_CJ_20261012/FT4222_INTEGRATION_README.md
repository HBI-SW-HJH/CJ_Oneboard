# LINMaster + FT4222 LED 제어 통합 가이드

## 추가된 내용

### 새 파일
| 파일 | 역할 |
|------|------|
| `FT4222/FT4222Manager.cs` | FT4222H SPI 통신 + OSP 프로토콜 구현 (ex_fioOsp C코드 C# 포팅) |
| `FT4222/ftd2xx.dll` | FTDI USB 드라이버 DLL |
| `FT4222/LibFT4222-64.dll` | FT4222 전용 DLL |

### 수정된 파일
| 파일 | 변경 내용 |
|------|-----------|
| `Forms/FormMain.cs` | FT4222 멤버(`_ft4222`), 연결/해제 메서드, FormCIE에 FT4222 전달 |
| `Forms/FormMain.Designer.cs` | `grpFT4222` GroupBox + 연결 버튼/콤보/상태 레이블 추가 |
| `Forms/FormCIE.cs` | 생성자에 `FT4222Manager` 파라미터 추가, `DoTx()`에서 FT4222 LED 동기화 |
| `Forms/FormCIE.Designer.cs` | `lblFT4222Status` 레이블 추가 |
| `LINMaster.csproj` | 새 .cs 파일 컴파일 등록, DLL 출력 폴더 복사 설정 |

---

## 동작 흐름

```
[FormMain]
  grpFT4222 그룹박스
    ├─ cmbFT4222Port  : 연결된 FT4222 장치 목록
    ├─ btnFT4222Refresh : 목록 새로고침
    ├─ btnFT4222Connect : 연결 / 연결 해제 토글
    └─ lblFT4222Status  : "● FT4222 연결됨" / "● FT4222 연결 안됨"

  btnOpenCIE 클릭
    → FormCIE(_gw, _ft4222) 생성   ← FT4222 인스턴스 전달

[FormCIE]
  CIE 색도도 클릭 또는 이미지 클릭
    → ApplyColorToTx()  →  nudColorX/Y 업데이트
  
  btnTxOnce 또는 주기 TX 타이머
    → DoTx()
        ① LIN 프레임 전송 (_gw.SendPublisher)
        ② FT4222 연결 시  _ft4222.SetColorCIE(x, y, brightness)
           → OSP SET_PWM 명령 → SPI → AD3306 → AD3301 → LED
```

---

## OSP 초기화 시퀀스 (FT4222Manager.Connect 내부)

```
1. RESET    (0x00, broadcast addr=0)
2. INIT_BIDIR (0x02, addr=1) → 응답 6바이트로 디바이스 주소 확인
3. GO_ACTIVE (0x05, confirmed addr)
4. SET_SETUP (0x4D) — CRC ON, PWM 586Hz
5. (이후 색상 변경마다) SET_PWM (0x4F)
```

---

## 빌드 방법

1. Visual Studio 2019/2022에서 `LINMaster.csproj` 열기
2. **빌드 구성**: Debug / x86 (기존과 동일)
3. 빌드 후 출력 폴더에 `ftd2xx.dll`, `LibFT4222-64.dll` 자동 복사됨

### 드라이버 사전 설치 필요
- [FTDI CDM WHQL Driver](https://www.ftdichip.com/Drivers/D2XX.htm) 설치
- FT4222 USB 모듈을 PC에 꽂으면 장치 관리자에 인식됨

---

## 색상 변환 흐름

```
CIE xy + Brightness
  │
  ▼  SetColorCIE()
XYZ (Y = brightness)
  │  XYZ→sRGB 행렬
  ▼
선형 sRGB  →  감마 보정 sRGB  →  0~255 byte
  │
  ▼  SetColorRgb()
PWM = byte << 7   (0~255 → 0~0x7F80, 15bit PWM)
  │
  ▼  OspSetPwm()
OSP SET_PWM 프레임 (10바이트) → SPI → AD3306
```

---

## 핀 연결 (FT4222H → AD3306)

| FT4222H 핀 | AD3306 핀 | 신호 |
|-----------|----------|------|
| SCK       | CLK      | SPI 클럭 |
| MOSI      | SDI      | SPI 데이터 |
| SS0/CS    | CS_N     | Chip Select (Active Low) |
| GND       | GND      | 공통 접지 |
| 3.3V or 5V | VDD    | 전원 (회로도 확인) |
