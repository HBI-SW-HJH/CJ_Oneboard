"""LIN-only: one-byte HardFault/recovery test, then 50 normal updates per chair.

No GUI, Boot download, product writes, firmware source edits, or SWD writes.
The fault BIN is deliberately different at exactly one byte; never a delivery file.
"""
import argparse
import ctypes as C
from datetime import datetime
import gzip
import hashlib
import json
from pathlib import Path
import sys
import time
import traceback
import zlib

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT.parent / 'old/CJ_Onboard_20261027/validation'))
import FullFourChairTest as transport
from RuntimeCrcLinReadOnly import ReadOnly

transport.ROOT = ROOT
PREFIX = 'FourChair50_20261117'
IMAGE = ROOT / 'CJ_OneBoard_App/build/Debug/Exe/CJ_App_Clock_20261117.bin'
SHA = '84a4ff1378d48131e8240366ec69fc8d1bd94aa7fdeafefae86fbeac54c0b4e2'
FAULT = ROOT / 'validation/CJ_App_FAULT_OneByte_20261117.bin'
require = transport.require


def decode(data):
    require(len(data) == 38 and data[:3] == bytes.fromhex('62f210'), 'F210 format')
    return dict(raw=data.hex(), mode=data[3], flag=int.from_bytes(data[12:16], 'big'), backup_flag=int.from_bytes(data[16:20], 'big'), fault_count=int.from_bytes(data[20:24], 'big'), backup_fault_count=int.from_bytes(data[24:28], 'big'), valid=data[28], restores=int.from_bytes(data[29:33], 'big'), backup_length=int.from_bytes(data[33:37], 'big'), bits=data[37])


class Test(transport.Test):
    def __init__(self, segment):
        self.initialized = False
        self.frames = gzip.open(ROOT / 'validation' / (segment + '_frames.jsonl.gz'), 'xt', encoding='utf-8')
        self.frame_count = 0
        self.read_retries = []
        self.target = None
        super().__init__(segment)

    def connect(self):
        ReadOnly.connect(self)

    def log(self, kind, **values):
        if kind == 'START':
            values.update(lin_flash_write=True, jlink=False)
        if kind == 'FRAME':
            self.frame_count += 1
            self.frames.write(json.dumps(dict(time=datetime.now().isoformat(), **values)) + '\n')
        else:
            super().log(kind, **values)

    def frame(self, frame_id, data=None, allow_empty=False):
        if frame_id == 1:
            require(self.target is not None and data[0] == 1 << (self.target - 10) and max(data[3:6]) <= 4, 'Target/brightness guard')
        return super().frame(frame_id, data, allow_empty)

    def runtime(self, board, confirmed=True, recovery=False):
        deadline = time.monotonic() + (60 if recovery else 18)
        retries = 0
        while True:
            try:
                nad, mode = self.select(board['chair'], board['init'])
                if mode == 1 and recovery:
                    state = decode(self.uds(nad, bytes.fromhex('22f210')))
                    self.log('RECOVERY_BOOT_STATE', chair=f"{board['chair']:02X}", state=state)
                    require(time.monotonic() < deadline, 'Recovery remained in Boot')
                    time.sleep(0.25)
                    continue
                require(mode == 0 and nad == board['chair'] - 9, 'Expected selected App')
                state = decode(self.uds(nad, bytes.fromhex('22f210')))
            except RuntimeError as exc:
                message = str(exc)
                known = message.startswith('UDS timeout NAD=7F request=3101f302') or (message.startswith(f"UDS timeout NAD={board['chair'] - 9:02X} request=22f210")) or message == 'LIN error ID=3D, flags=10'
                if not known or time.monotonic() >= deadline or (not recovery and retries >= 3):
                    raise
                retries += 1
                event = dict(time=datetime.now().isoformat(), chair=f"{board['chair']:02X}", retry=retries, recovery_wait=recovery, error=message)
                self.read_retries.append(event)
                self.log('READ_RETRY', **event)
                time.sleep(0.25)
                continue
            require(state['flag'] == state['backup_flag'] == state['fault_count'] == state['backup_fault_count'] == 0, 'App recovery flags/fault counters')
            if not confirmed or state['valid'] == 3 and state['bits'] & 4:
                return state
            require(time.monotonic() < deadline, 'App confirmation timeout')
            time.sleep(0.25)

    def rgb_all(self, chair, color):
        rgb = ([4, 0, 0], [0, 4, 0], [0, 0, 4])[color]
        self.lamp(chair, 63, rgb)
        time.sleep(0.1)
        rows = []
        for zone in range(6):
            for node in range(24 if zone == 4 else 8 if zone == 5 else 1):
                seq = len(rows) + 1
                self.frame(0x28 + chair - 10, [5, zone, node, 0, 1, seq, 0, 0])
                time.sleep(0.015)
                actual = self.frame(0x2C + chair - 10)
                self.frame(0x28 + chair - 10, [10, zone, node, 0, 0, seq, 0, 0])
                time.sleep(0.005)
                expected = self.frame(0x2C + chair - 10)
                require(actual == expected and actual[0] == 15 and actual[7] == seq, f'RGB mismatch {chair:02X}/{zone}/{node}')
                values = [int.from_bytes(actual[n:n + 2], 'big') for n in (1, 3, 5)]
                require(values[color] > 0 and all(values[n] == 0 for n in range(3) if n != color), 'RGB color mismatch')
                rows.append(dict(zone=zone, node=node, rgb=values))
        self.off(chair)
        return rows

    def close(self):
        if self.initialized and self.connected:
            self.api('InitializeHardware', [C.c_uint8, C.c_uint16, C.c_uint8, C.c_uint16])(self.client, self.hw, 0, 19200)
        super().close()
        self.frames.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--resume', action='store_true')
    parser.add_argument('--resume-interrupted', action='store_true')
    args = parser.parse_args()
    output = ROOT / 'validation' / (PREFIX + '_result.json')
    image = IMAGE.read_bytes()
    require(hashlib.sha256(image).hexdigest() == SHA, 'Normal BIN changed')
    # Thumb UDF T1 = 0xDE00 | imm8. Replace PUSH 0xB538 with UDF 0xDE38.
    # ARMv6-M Architecture Reference Manual A6.7 UDF:
    # https://documentation-service.arm.com/static/5f8ff05ef86e16515cdbf826
    offset = 0x5AE9
    require(image[offset - 1:offset + 1] == bytes.fromhex('38b5'), 'Fault injection instruction mismatch')
    damaged = bytearray(image)
    damaged[offset] = 0xDE
    require([i for i, (a, b) in enumerate(zip(image, damaged)) if a != b] == [offset], 'Must change exactly one byte')
    if FAULT.exists():
        require(FAULT.read_bytes() == damaged, 'Existing fault BIN differs')
    else:
        FAULT.write_bytes(damaged)
    fault_info = dict(path=str(FAULT), length=len(damaged), offset=hex(offset), address=hex(0x10000 + offset), old='B5', new='DE', instruction='PUSH 0xB538 -> UDF 0xDE38 at Reprogram_Process entry', sha256=hashlib.sha256(damaged).hexdigest(), crc32=f'{zlib.crc32(damaged):08X}', modified_byte_count=1)
    require(args.resume == output.exists() and (not args.resume_interrupted or args.resume), 'Use explicit resume for an existing campaign')
    interrupted = None
    if args.resume:
        result = json.loads(output.read_text())
        require(not result['passed'], 'Campaign already completed')
        if args.resume_interrupted:
            interrupted = result['in_progress']
            require(interrupted is not None and interrupted['type'] == 'normal' and interrupted['chair'] == 10 and interrupted['phase'] == 'transfer' and (interrupted['cycle'], len(result['completed'])) in [(30, 116), (41, 160), (50, 196)], 'Unexpected interrupted attempt')
            if interrupted['cycle'] == 30:
                note = 'PC sleep event at 2026-11-17 18:10:15 during A30 transfer; PC restarted on Nov18. Read-only LIN recheck found A in Boot flag1, B/C/D healthy App. Restart A30 transfer from byte zero; incomplete attempt is not counted as passed.'
            else:
                require(result.get('error') == "RuntimeError('UDS timeout NAD=7D request=360381b001000dd0')", 'Unexpected transfer error')
                note = f"A{interrupted['cycle']} third data block received no response within 1 second; blocks1/2 acknowledged. Read-only LIN recheck found A responsive in Boot flag1, fault count0, restore count unchanged; B/C/D healthy. Exact cause not established. User requested retransmission for this failure. Restart from byte zero; failed attempt is not counted as passed."
        else:
            require(result['in_progress'] is None, 'Incomplete attempt requires explicit inspection before resume')
            note = 'Resume after separately recorded inspection; completed attempts are retained.'
        result.setdefault('interruptions', []).append(dict(recorded=datetime.now().isoformat(), completed=len(result['completed']), error=result.pop('error', None), traceback=result.pop('traceback', None), note=note, interrupted_attempt=interrupted))
        result.pop('finished', None)
        result.setdefault('resume_times', []).append(datetime.now().isoformat())
    else:
        preflight = json.loads((ROOT / 'validation/FourChair50_20261117_preflight.json').read_text())
        require(len(preflight['boards']) == 4 and all('error' not in b and b['diagnosis'] == [0] * 6 for b in preflight['boards']), 'Initial four-chair check failed')
        result = dict(started=datetime.now().isoformat(), passed=False, requested_per_chair=50, boards=[b['product'] for b in preflight['boards']], image=dict(path=str(IMAGE), length=len(image), sha256=SHA, crc32=f'{zlib.crc32(image):08X}'), fault_image=fault_info, recoveries=[], completed=[], in_progress=None, initial=preflight)
    segment = PREFIX + '_' + datetime.now().strftime('%Y%m%d_%H%M%S')
    result.setdefault('segments', []).append(segment)
    t = None
    timer_set = C.WinDLL('winmm').timeBeginPeriod(1) == 0

    def save():
        temp = output.with_suffix('.tmp')
        temp.write_text(json.dumps(result, indent=2), encoding='utf-8')
        temp.replace(output)

    def check_board(board, recovered=None):
        state = t.runtime(board)
        require(transport.stable_product(t.info(board['chair'])) == transport.stable_product(bytes.fromhex(board['raw'])), 'Product identity changed')
        require(t.status(board['chair']) == [0] * 6, 'Diagnostic fault')
        if recovered is not None:
            require(state['restores'] == recovered, 'Unexpected restore count')
        return state

    def download(board, data, item, starting_in_boot=False):
        chair = board['chair']
        t.target = chair
        t.read_retries = []
        if not starting_in_boot:
            t.off(chair)
        item.update(started=datetime.now().isoformat(), phase='enter_boot', chair=chair)
        result['in_progress'] = item
        save()
        t.log('UPDATE_BEGIN', chair=f'{chair:02X}', kind_of_image=item['type'], cycle=item.get('cycle'), normal_completed=len(result['completed']))
        nad, mode = t.select(chair, board['init'])
        if not starting_in_boot:
            require(mode == 0 and t.uds(nad, [0x10, 2])[:2] == bytes.fromhex('5002'), 'App programming entry')
            time.sleep(0.7)
            nad, mode = t.select(chair, board['init'])
        require(mode == 1 and t.uds(nad, [0x10, 2])[:2] == bytes.fromhex('5002'), 'Boot programming entry')
        ready = t.uds(nad, bytes.fromhex('340044') + (0x10000).to_bytes(4, 'big') + len(data).to_bytes(4, 'big'), 25)
        require(ready == bytes.fromhex('74200082'), 'Download block size')
        item['phase'] = 'transfer'
        save()
        seq = 1
        for pos in range(0, len(data), 128):
            block = data[pos:pos + 128]
            require(t.uds(nad, bytes([0x36, seq]) + block) == bytes([0x76, seq]), 'Transfer acknowledgement')
            seq = (seq + 1) & 255
            if (pos // 128 + 1) % 58 == 0:
                item['bytes_acknowledged'] = pos + len(block)
                save()
                t.log('UPDATE_PROGRESS', chair=f'{chair:02X}', cycle=item.get('cycle'), percent=(pos + len(block)) * 100 // len(data))
        require(t.uds(nad, b'\x37' + zlib.crc32(data).to_bytes(4, 'big'), 3) == b'\x77', 'Device image CRC rejected')
        item.update(phase='app_restart', flash_crc_accepted=True, bytes_acknowledged=len(data))
        save()
        require(t.uds(nad, [0x11, 1]) == bytes.fromhex('5101'), 'Reset acknowledgement')
        item['reset_acknowledged'] = True
        save()
        time.sleep(3)

    try:
        t = Test(segment)
        t.connect()
        for board in result['boards']:
            t.target = board['chair']
            if interrupted is not None and board['chair'] == interrupted['chair']:
                nad, mode = t.select(board['chair'], board['init'])
                state = decode(t.uds(nad, bytes.fromhex('22f210')))
                require(nad == 0x7D and mode == state['mode'] == 1 and state['flag'] == state['backup_flag'] == 1 and state['fault_count'] == state['backup_fault_count'] == 0 and state['valid'] == 3 and state['bits'] == 3 and state['restores'] == interrupted['before']['restores'], 'Interrupted Boot state changed')
                result['interruptions'][-1]['live_boot_state'] = state
                t.log('RESUME_INTERRUPTED_BOOT', chair=f"{board['chair']:02X}", state=state, cycle=interrupted['cycle'])
                continue
            check_board(board)
            t.off(board['chair'])
        for board in result['boards']:
            if any(i['chair'] == board['chair'] for i in result['recoveries']):
                continue
            before = check_board(board)
            require(before['valid'] == 3 and before['bits'] & 7 == 7, 'Healthy backup-capable App required')
            item = dict(type='one_byte_fault', before=before)
            download(board, damaged, item)
            after = t.runtime(board, recovery=True)
            require(after['restores'] == before['restores'] + 1 and after['bits'] & 15 == 15, 'Automatic backup restore not confirmed')
            require(transport.stable_product(t.info(board['chair'])) == transport.stable_product(bytes.fromhex(board['raw'])), 'Restored product changed')
            item['readbacks'] = t.rgb_all(board['chair'], 0)
            require(t.status(board['chair']) == [0] * 6, 'Restored App diagnostic fault')
            item.update(after=after, passed=True, phase='restored', read_retries=list(t.read_retries), finished=datetime.now().isoformat())
            result['recoveries'].append(item)
            result['in_progress'] = None
            t.frames.flush()
            save()
            t.log('RECOVERY_PASS', chair=f"{board['chair']:02X}", before_restores=before['restores'], after_restores=after['restores'], readbacks=len(item['readbacks']))
        for cycle in range(1, 51):
            for board in result['boards']:
                if any(i['chair'] == board['chair'] and i['cycle'] == cycle for i in result['completed']):
                    continue
                require(not (ROOT / 'validation' / (PREFIX + '.STOP')).exists(), 'Operator stop')
                require(IMAGE.read_bytes() == image, 'BIN changed during campaign')
                restored = next(i['after']['restores'] for i in result['recoveries'] if i['chair'] == board['chair'])
                starting_in_boot = interrupted is not None and board['chair'] == interrupted['chair'] and cycle == interrupted['cycle']
                before = interrupted['before'] if starting_in_boot else check_board(board, restored)
                item = dict(type='normal', cycle=cycle, before=before)
                if starting_in_boot:
                    item['restarted_interrupted_transfer'] = True
                download(board, image, item, starting_in_boot)
                after = t.runtime(board)
                require(after['restores'] == restored and after['bits'] == 7, 'Unexpected recovery after normal image')
                require(transport.stable_product(t.info(board['chair'])) == transport.stable_product(bytes.fromhex(board['raw'])), 'Product changed')
                item['readbacks'] = t.rgb_all(board['chair'], (cycle - 1) % 3)
                require(t.status(board['chair']) == [0] * 6, 'Post-update diagnostic fault')
                item.update(after=after, passed=True, phase='passed', read_retries=list(t.read_retries), finished=datetime.now().isoformat())
                result['completed'].append(item)
                result['in_progress'] = None
                t.frames.flush()
                save()
                t.log('UPDATE_PASS', chair=f"{board['chair']:02X}", cycle=cycle, completed=len(result['completed']), readbacks=len(item['readbacks']), read_retries=len(item['read_retries']))
        result['final'] = []
        for board in result['boards']:
            t.target = board['chair']
            result['final'].append(dict(chair=board['chair'], runtime=check_board(board)))
            t.off(board['chair'])
        result['passed'] = True
    except Exception as exc:
        result['error'] = repr(exc)
        result['traceback'] = traceback.format_exc()
        print('STOP_ON_ERROR ' + repr(exc), flush=True)
    finally:
        if t:
            t.close()
        if timer_set:
            C.WinDLL('winmm').timeEndPeriod(1)
        result['finished'] = datetime.now().isoformat()
        save()
        print(json.dumps(dict(passed=result['passed'], recoveries=len(result['recoveries']), completed=len(result['completed']), report=str(output), error=result.get('error'))), flush=True)
    if not result['passed']:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
