"""Audit saved hardware-test evidence; no adapter, SWD, GUI or firmware writes."""
from collections import Counter
from datetime import datetime
import gzip
import hashlib
import json
from pathlib import Path
import zlib

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / 'validation'
result = json.loads((OUT / 'FourChair50_20261117_result.json').read_text())
manifest = json.loads((OUT / 'ClockStress100_20261117_manifest.json').read_text())
checks = {}


def healthy(state, restores, bits=7):
    return state['mode'] == 0 and all(state[k] == 0 for k in ['flag', 'backup_flag', 'fault_count', 'backup_fault_count']) and state['valid'] == 3 and state['bits'] == bits and state['restores'] == restores


checks['campaign_finished'] = result['passed'] and result['in_progress'] is None and not result.get('error')
pairs = [(i['chair'], i['cycle']) for i in result['completed']]
checks['exactly_50_each_unique'] = len(pairs) == 200 and set(pairs) == {(c, n) for c in range(10, 14) for n in range(1, 51)}
restores = {i['chair']: i['after']['restores'] for i in result['recoveries']}
checks['four_fault_recoveries'] = len(result['recoveries']) == 4 and set(restores) == set(range(10, 14))
checks['fault_recovery_evidence'] = all(i['passed'] and i['flash_crc_accepted'] and i['reset_acknowledged'] and healthy(i['after'], i['before']['restores'] + 1, 15) and len(i['readbacks']) == 36 for i in result['recoveries'])
checks['normal_update_evidence'] = all(i['passed'] and i['flash_crc_accepted'] and i['reset_acknowledged'] and i['bytes_acknowledged'] == result['image']['length'] and healthy(i['after'], restores[i['chair']]) and len(i['readbacks']) == 36 for i in result['completed'])
checks['final_four_healthy'] = len(result['final']) == 4 and all(healthy(i['runtime'], restores[i['chair']]) for i in result['final'])

events = []
for segment in result['segments']:
    events.extend(json.loads(line) for line in (OUT / (segment + '.jsonl')).read_text(encoding='utf-8').splitlines() if line.strip())
passes = [e for e in events if e['kind'] == 'UPDATE_PASS']
begins = [e for e in events if e['kind'] == 'UPDATE_BEGIN']
checks['event_passes_match_results'] = len(passes) == 200 and {(int(e['chair'], 16), e['cycle']) for e in passes} == set(pairs)
checks['logged_diagnostics_clear'] = all(not any(e['faults'].values()) for e in events if e['kind'] == 'DIAGNOSTIC')
checks['logged_recoveries_match_results'] = len([e for e in events if e['kind'] == 'RECOVERY_PASS']) == 4

image = Path(result['image']['path']).read_bytes()
fault = Path(result['fault_image']['path']).read_bytes()
checks['normal_bin_unchanged'] = hashlib.sha256(image).hexdigest() == result['image']['sha256'] and f'{zlib.crc32(image):08X}' == result['image']['crc32']
differences = [i for i, (a, b) in enumerate(zip(image, fault)) if a != b]
checks['fault_bin_exactly_one_byte'] = len(image) == len(fault) and differences == [0x5AE9] and image[0x5AE9] == 0xB5 and fault[0x5AE9] == 0xDE and hashlib.sha256(fault).hexdigest() == result['fault_image']['sha256']
source_changes = []
for relative, expected in manifest['source_hashes'].items():
    path = ROOT / relative
    actual = hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else None
    if actual != expected:
        source_changes.append(dict(path=relative, expected=expected, actual=actual))
checks['source_and_configuration_unchanged'] = not source_changes

transfer_failures = []
for segment, cycle, since in [('FourChair50_20261117_20261118_090228', 41, '2026-11-18T10:06:56.643730'), ('FourChair50_20261117_20261118_101033', 50, '2026-11-18T11:03:26')]:
    with gzip.open(OUT / (segment + '_frames.jsonl.gz'), 'rt', encoding='utf-8') as stream:
        frames = [e for e in (json.loads(line) for line in stream) if e['time'] >= since]
    first = next(n for n, e in enumerate(frames) if e['id'] == 60 and e['data'].startswith('7d10823603'))
    tail = frames[first:]
    requests = [e for e in tail if e['id'] == 60]
    responses = [e for e in tail if e['id'] == 61]
    intervals = [(b['hw_us'] - a['hw_us']) / 1000 for a, b in zip(requests, requests[1:])]
    row = dict(chair='0A', cycle=cycle, acknowledged_blocks=[n for n in [1, 2] if any(e['id'] == 61 and e['errors'] == 0 and e['data'].startswith(f'7d0276{n:02x}') for e in frames[:first])], failed_block=3, block_frames=len(requests), publisher_echo_errors=sum(e['errors'] != 0 for e in requests), response_headers=len(responses), response_errors=dict(Counter(f"{e['errors']:02X}" for e in responses)), request_frame_interval_ms_min=min(intervals), request_frame_interval_ms_max=max(intervals), later_complete_retransmission_passed=any(i['chair'] == 10 and i['cycle'] == cycle and i['passed'] and i.get('restarted_interrupted_transfer') for i in result['completed']), frames=frames)
    transfer_failures.append(row)

retries = [dict(chair=f"{i['chair']:02X}", cycle=i['cycle'], events=i['read_retries']) for i in result['completed'] if i['read_retries']]
board_rows = []
for board in result['boards']:
    c = board['chair']
    recovery = next(i for i in result['recoveries'] if i['chair'] == c)
    board_rows.append(dict(chair=f'{c:02X}', serial=board['serial'], init_number=f"{board['init']:08X}", completed=sum(i['chair'] == c for i in result['completed']), fault_test_passed=recovery['passed'], restore_count_before=recovery['before']['restores'], restore_count_after=restores[c], post_boot_read_retries=sum(len(i['read_retries']) for i in result['completed'] if i['chair'] == c)))
audit = dict(created=datetime.now().isoformat(), requested_checks_complete=all(checks.values()), clean_without_incidents=False, checks=checks, boards=board_rows, normal_downloads_completed=len(pairs), normal_attempts_started=sum(e['kind_of_image'] == 'normal' for e in begins), recovery_test_downloads=sum(e['kind_of_image'] == 'one_byte_fault' for e in begins), rgb_readback_comparisons=sum(len(i['readbacks']) for i in result['completed'] + result['recoveries']), source_files_checked=len(manifest['source_hashes']), source_changes=source_changes, image=result['image'], fault_image=result['fault_image'], post_boot_read_retry_cases=retries, interruptions=result['interruptions'], transfer_failures=transfer_failures, final=result['final'], limitations=['LIN API hardware tests; no GUI automation or optical/scope measurement', 'Four-board recovery confirmed using device status/restore count and LED-driver RGB readback; not a direct SWD flash comparison on all boards', 'Two missing block3 responses on 0A; exact cause not established; both entire retransmissions succeeded'])
audit_path = OUT / 'FourChair50_20261118_completion_audit.json'
audit_path.write_text(json.dumps(audit, indent=2, ensure_ascii=False), encoding='utf-8')

lines = [
    'CJ 4대 LIN 업데이트 / 백업 복원 시험 결과',
    f"작업 프로젝트: {ROOT}",
    f"시험 시작: {result['started']}",
    f"시험 종료: {result['finished']} (중간 PC 절전 및 시험 중단 시간 포함)",
    '',
    '결과: 각 50회, 정상 업데이트 완료 총 200회. 1바이트 변경 후 자동 백업 복원도 4대 모두 성공.',
    '중단·재시도를 포함한 완료 결과이며, 모든 시도가 오류 없이 끝났다는 의미는 아님.',
    '',
    '의자 | Serial | 정상 완료 | 1바이트 오류 복원 | 복원 횟수 변화 | 부팅 후 조회 재시도',
]
for b in board_rows:
    lines.append(f"0x{b['chair']} | {b['serial']} | {b['completed']}/50 | 성공 | {b['restore_count_before']} -> {b['restore_count_after']} | {b['post_boot_read_retries']}회")
lines.extend([
    '',
    f"정상 BIN: {result['image']['path']}",
    f"크기: {len(image)} bytes / CRC32: {result['image']['crc32']}",
    f"SHA256: {result['image']['sha256']}",
    '',
    '검증 방법',
    '- 각 다운로드: 전 블록 응답, 장치 CRC 수락, 리셋 응답, App 확인 플래그, 제품 정보/의자 ID, 6구역 진단 확인.',
    '- 각 회차: R/G/B를 순서대로 낮은 값 4/255로 제어하고, SRGB 4구역 + Logo 24개 + Strobe 8개의 실제 RGB 읽기와 예상값을 비교.',
    f"- 정상 200회 7,200건 + 복원 시험 4회 144건 = 총 {audit['rgb_readback_comparisons']:,}건 RGB 비교 통과.",
    '- 마지막 상태: 4대 모두 App 실행, flag=0, HardFault count=0, 진단 6구역 정상. 모든 보드 OFF 명령 전송 후 어댑터 연결 해제.',
    '',
    '1바이트 오류 주입',
    '- 별도 시험 BIN의 offset 0x5AE9 (Flash 0x15AE9) 한 바이트만 B5 -> DE로 변경.',
    '- Reprogram_Process 진입의 PUSH 명령을 UDF로 바꿔 HardFault를 유발. 전송 CRC는 변경된 BIN 기준으로 계산하여 장치에서 실행되도록 함.',
    '- 4대 모두 자동 복원 후 App 실행 및 RGB/진단 확인. 복원 확인 전에 정상 BIN으로 구제 다운로드하지 않음.',
    '- 이 시험은 해당 위치의 실행 오류와 HardFault 복원 경로를 검증함. 임의의 모든 데이터 손상이나 정상 부팅 시 전체 CRC 검사까지 보증하는 시험은 아님.',
    '',
    '중간에 발생한 항목',
    '1. 0x0A 41회차 / 50회차: 3번 데이터 블록(0x36 03) 응답 누락이 각각 1회 발생.',
    '   1·2번 블록은 응답했고, 이후 응답 헤더를 반복 요청했으나 1초 내 응답이 없었음.',
    '   상태 재조회에서 Boot flag=1, HardFault count=0, 복원 횟수 증가 없음. Boot LIN 응답은 정상.',
    '   두 회차 모두 처음부터 재전송 후 CRC/App/RGB/진단 통과. 같은 위치에서 재발했으므로 원인이 단순 LIN 노이즈라고 확정하지 않음.',
    '2. 재부팅 후 대상 선택/F210 상태 조회 지연 또는 수신 오류 9건: 각 1회 재조회로 정상 통과.',
    '   0A: 7, 9, 31회차 / 0B: 3, 19회차 / 0C: 6, 16, 30, 33회차 / 0D: 없음.',
    '3. 0x0B 16회차 진입 전 무응답: SWD에서 HALT 확인. 이후 당사 Reset/Run/다운로드 없이 LIN 정상 복귀를 확인하고 시험 재개. 원인 미확정.',
    '4. 0x0A 30회차 전송 도중 PC 절전: Windows 이벤트와 중단 기록 확인. 다음 날 Boot 대기 상태에서 전체 재전송 성공.',
    f"- 정상 전송 시작 {audit['normal_attempts_started']}회 중 완료 200회. 중단된 3회(PC 절전 1회, 블록 응답 누락 2회)는 성공 횟수에 포함하지 않음.",
    f"- 별도 오류 주입 다운로드 {audit['recovery_test_downloads']}회. 총 다운로드 시작 {len(begins)}회.",
    '',
    '변경 및 검증 범위',
    f"- 시험 시작 기준 소스/설정 파일 {audit['source_files_checked']}개 SHA256 대조: 변경 {len(source_changes)}개.",
    '- 정상 BIN 및 별도 1바이트 오류 BIN 해시 일치. Boot/App 소스, IAR ewp/icf 변경 없음.',
    '- 화면 제어/GUI 실행 없이 LIN API로 실물 시험. 광학 측정/파형 측정/4대 전체 SWD Flash 비교는 이 시험 범위에 포함하지 않음.',
    '',
    f"상세 검증 결과: {audit_path}",
    f"전체 회차 원본 결과: {OUT / 'FourChair50_20261117_result.json'}",
])
report_path = OUT / 'FourChair50_20261118_final_summary.txt'
report_path.write_text('\n'.join(lines) + '\n', encoding='utf-8-sig')
print(json.dumps(dict(checks=checks, report=str(report_path), audit=str(audit_path), starts=len(begins), normal_starts=audit['normal_attempts_started'], rgb=audit['rgb_readback_comparisons'], source_changes=source_changes, transfer_failures=[{k:v for k,v in row.items() if k!='frames'} for row in transfer_failures]), ensure_ascii=False))
if not all(checks.values()):
    raise SystemExit('Evidence audit failed; inspect checks before reporting completion')
