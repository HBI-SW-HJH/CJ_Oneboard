"""100 real LIN App downloads to the SWD-identified chair 0B.

Existing transport/protocol; no GUI, Boot download, fuse writes, or source edits.
On authorized resume, retry and record the known post-boot read timeout only.
J-Link is used only for readback after LIN downloads.
"""
import argparse
import ctypes as C
from datetime import datetime
import gzip
import hashlib
import json
from pathlib import Path
import struct
import sys
import time
import traceback
import zlib

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT.parent / 'old/CJ_Onboard_20261027/validation'))
import FullFourChairTest as transport
from RuntimeCrcLinReadOnly import ReadOnly
from RuntimeCrcReadOnly import Probe, record, pds_info
import ClockVerify_20261117 as clock

transport.ROOT = ROOT
PREFIX = 'ClockLin100_20261117'
IMAGE = ROOT / 'CJ_OneBoard_App/build/Debug/Exe/CJ_App_Clock_20261117.bin'
SHA = '84a4ff1378d48131e8240366ec69fc8d1bd94aa7fdeafefae86fbeac54c0b4e2'
CHAIR = 0x0B
require = transport.require


class Test(transport.Test):
    def __init__(self):
        self.frames = gzip.open(ROOT / 'validation' / (PREFIX + '_frames.jsonl.gz'), 'xt', encoding='utf-8')
        self.frame_count = 0
        self.initialized = False
        self.read_timeouts = []
        self.allow_read_retry = False
        super().__init__(PREFIX)

    def connect(self):
        ReadOnly.connect(self)

    def frame(self, frame_id, data=None, allow_empty=False):
        require(frame_id in (1, 0x0B, 0x29, 0x2D, 0x3C, 0x3D), 'Non-target frame')
        if frame_id == 1:
            require(data[0] == 2 and max(data[3:6]) <= 4, 'Only low-brightness chair 0B control')
        return super().frame(frame_id, data, allow_empty)

    def log(self, kind, **values):
        if kind == 'START':
            values.update(jlink=True, jlink_flash_write=False, lin_flash_write=True)
        if kind == 'FRAME':
            self.frame_count += 1
            self.frames.write(json.dumps(dict(time=datetime.now().isoformat(), **values)) + '\n')
        else:
            super().log(kind, **values)

    def runtime(self, identity, confirmed=True):
        deadline = time.monotonic() + 16
        retries = 0
        while True:
            try:
                nad, mode = self.select(CHAIR, identity)
                require(mode == 0 and nad == CHAIR - 9, 'Expected target APP')
                data = self.uds(nad, bytes.fromhex('22f210'))
            except RuntimeError as exc:
                message = str(exc)
                known_read = message.startswith('UDS timeout NAD=7F request=3101f302') or message.startswith('UDS timeout NAD=02 request=22f210') or message == 'LIN error ID=3D, flags=10'
                if not self.allow_read_retry or not known_read or retries >= 3 or time.monotonic() >= deadline:
                    raise
                retries += 1
                event = dict(time=datetime.now().isoformat(), retry=retries, error=message)
                self.read_timeouts.append(event)
                self.log('KNOWN_READ_TIMEOUT_RETRY', **event)
                time.sleep(0.25)
                continue
            require(len(data) == 38 and data[3] == 0 and data[12:28] == bytes(16), 'App flags/HardFault counts not zero')
            if not confirmed or (data[28] == 3 and data[37] & 4):
                return data
            require(time.monotonic() < deadline, 'App healthy confirmation timed out')
            time.sleep(0.25)

    def rgb_all(self, color):
        rgb = ([4, 0, 0], [0, 4, 0], [0, 0, 4])[color]
        self.lamp(CHAIR, 63, rgb)
        time.sleep(0.1)
        output = []
        for zone in range(6):
            for node in range(24 if zone == 4 else 8 if zone == 5 else 1):
                seq = len(output) + 1
                self.frame(0x29, [5, zone, node, 0, 1, seq, 0, 0])
                time.sleep(0.015)
                actual = self.frame(0x2D)
                self.frame(0x29, [10, zone, node, 0, 0, seq, 0, 0])
                time.sleep(0.005)
                expected = self.frame(0x2D)
                require(actual == expected and actual[7] == seq and actual[0] == 15, f'RGB readback mismatch {zone}/{node}: {actual.hex()}/{expected.hex()}')
                values = [int.from_bytes(actual[n:n + 2], 'big') for n in (1, 3, 5)]
                require(values[color] > 0 and all(values[n] == 0 for n in range(3) if n != color), 'Color not applied')
                output.append(dict(zone=zone, node=node, rgb=values))
        self.off(CHAIR)
        return output

    def close(self):
        if self.initialized and self.connected:
            self.api('InitializeHardware', [C.c_uint8, C.c_uint16, C.c_uint8, C.c_uint16])(self.client, self.hw, 0, 19200)
        super().close()
        self.frames.close()


def main():
    global PREFIX
    parser = argparse.ArgumentParser()
    parser.add_argument('--resume', action='store_true')
    parser.add_argument('--resume-after-sleep', action='store_true')
    parser.add_argument('--resume-after-read-error', action='store_true')
    args = parser.parse_args()
    previous = None
    if args.resume_after_read_error:
        args.resume = True
        previous = json.loads((ROOT / 'validation/ClockLin100_20261117_resume3_result.json').read_text())
        require(len(previous['completed']) == 53 and previous['in_progress']['number'] == 54 and previous['in_progress']['phase'] == 'app_restart' and previous['in_progress']['flash_crc_accepted'], 'Unexpected read-error position')
        audit = json.loads((ROOT / 'validation/ClockLin100_20261117_incident54_readonly.json').read_text())
        require(audit.get('passed') and audit['app_match'] and audit['backup_match'] and len(audit['samples']) == 10, 'Read-only incident audit must pass')
        PREFIX = 'ClockLin100_20261117_resume4'
    elif args.resume_after_sleep:
        args.resume = True
        previous = json.loads((ROOT / 'validation/ClockLin100_20261117_resume_result.json').read_text())
        require(len(previous['completed']) == 31 and previous['in_progress']['number'] == 32 and previous['in_progress']['phase'] == 'transfer', 'Unexpected interrupted transfer position')
        PREFIX = 'ClockLin100_20261117_resume3'
    elif args.resume:
        previous = json.loads((ROOT / 'validation/ClockLin100_20261117_result.json').read_text())
        require(len(previous['completed']) == 12 and previous['in_progress']['number'] == 13 and previous['in_progress']['flash_crc_accepted'], 'Unexpected resume position')
        PREFIX = 'ClockLin100_20261117_resume'
    output = ROOT / 'validation' / (PREFIX + '_result.json')
    require(not output.exists(), 'Do not overwrite/repeat an existing campaign')
    clocks = json.loads((ROOT / 'validation/ClockStress100_20261117_result.json').read_text())
    require(clocks.get('passed') and len(clocks['cycles']) == 300 and not clocks.get('cleanup_error'), 'Clock campaign must pass before LIN download campaign')
    image = IMAGE.read_bytes()
    require(hashlib.sha256(image).hexdigest() == SHA, 'BIN changed')
    stack, entry = struct.unpack_from('<II', image)
    require(192 <= len(image) <= 192 * 1024 and 0x20000000 < stack <= 0x20010000 and stack % 8 == 0 and entry & 1 and 0x10000 <= entry < 0x10000 + len(image), 'Invalid App image')
    crc = zlib.crc32(image)
    result = dict(started=datetime.now().isoformat(), requested=100, chair='0B', image=dict(path=str(IMAGE), length=len(image), sha256=SHA, crc32=f'{crc:08X}'), completed=[], in_progress=None, passed=False)
    if previous is not None:
        result.update(completed=previous['completed'], baseline_hashes=previous['baseline_hashes'], initial_record=previous['initial_record'],
                      prior_started=previous['started'], prior_incident=dict(number=13, error=previous['error'], in_progress=previous['in_progress']),
                      continuation_authorized=True, resume_from=14, known_read_retry_limit=3)
    if args.resume_after_sleep:
        result.update(prior_incident=previous['prior_incident'], resume_from=32,
                      interrupted_attempts=[dict(number=32, cause='Windows sleep: 12:24:24 to 13:04:16; System event 42/1', error=previous['error'], in_progress=previous['in_progress'])])
    if args.resume_after_read_error:
        result.update(prior_incident=previous['prior_incident'], resume_from=55,
                      interrupted_attempts=previous['interrupted_attempts'],
                      postboot_read_incidents=[dict(number=54, error=previous['error'], in_progress=previous['in_progress'], readonly_audit='ClockLin100_20261117_incident54_readonly.json')])
    test, p = None, Probe()
    timer_set = C.WinDLL('winmm').timeBeginPeriod(1) == 0

    def save():
        output.write_text(json.dumps(result, indent=2), encoding='utf-8')

    def byte(address):
        return (p.word(address & ~3) >> ((address & 3) * 8)) & 255

    def snapshot():
        osp = clock.OSP.from_buffer_copy(p.read(*clock.symbol('gOSPControl')))
        srgb = clock.SRGB.from_buffer_copy(p.read(*clock.symbol('gSRGBControl')))
        state = dict(tick_ms=p.word(clock.symbol('oAppTimer')[0]), halted=bool(p.api('IsHalted', C.c_int, [])()),
                     gclk0=hex(p.word(0x40001C20)), gclk1=hex(p.word(0x40001C24)), dpll_ctrlb=hex(p.word(0x40001024)), dpll_status=byte(0x40001030),
                     wdt=byte(0x40002000), bootprot=byte(0x804000) & 15, locks=p.word(0x41004020) & 65535,
                     nodes=list(osp.nodeCount), osp_valid=[sum(row) for row in osp.dynamicValid],
                     osp_errors=list(osp.communicationErrorCount), osp_crc=list(osp.crcErrorCount), osp_manchester=list(osp.manchesterErrorCount),
                     srgb_errors=srgb.communicationErrorCount, srgb_valid=srgb.dynamicValid,
                     lin_errors=p.word(clock.symbol('g_dbg_lin_error_count')[0]), lin_drops=p.word(clock.symbol('g_dbg_lin_drop_count')[0]),
                     power_fault=byte(clock.symbol('gPowerData')[0] + 1), max_temperature=max(max(row) for row in osp.temperatureC))
        require(not state['halted'] and state['wdt'] & 2, 'CPU not running with WDT')
        require(state['gclk0'] == state['gclk1'] == '0x10307' and state['dpll_ctrlb'] == '0x31010' and state['dpll_status'] & 2, 'Clock configuration mismatch')
        require(state['bootprot'] == 4 and state['locks'] == 0, 'Flash protection mismatch')
        require(state['nodes'] == state['osp_valid'] == [24, 8] and state['srgb_valid'] == 1, 'LED communication invalid')
        require(state['osp_errors'] == state['osp_crc'] == state['osp_manchester'] == [0, 0] and state['srgb_errors'] == state['lin_drops'] == state['power_fault'] == 0, 'Runtime communication/power error')
        if state['lin_errors'] != 0:
            a, length = clock.symbol('g_dbg_lin_last_status')
            state['lin_last_status'] = (p.word(a & ~3) >> ((a & 3) * 8)) & 65535
            allowed = 1 if test is None and previous is not None else len(test.read_timeouts) if test else 0
            if previous is not None and len(result['completed']) == 12:
                allowed = max(allowed, 1)
            require(args.resume and state['lin_errors'] <= allowed and state['lin_last_status'] == 4, 'Unexpected LIN error')
            state['known_buffer_overflow_observed'] = True
        require(state['max_temperature'] < 80, 'Temperature cutoff')
        return state

    def verify_flash():
        main = p.read(0x10000, len(image))
        recovery = p.read(0x50000, len(image))
        require(main == image, 'App Flash byte comparison failed')
        require(recovery == image, 'Backup Flash byte comparison failed')
        data = p.read(0x400000, 0x2000)
        a, b = record(data[0x1E00:0x1F00]), record(data[0xF00:0x1000])
        require(a == b and a['valid'], 'Update records invalid/different')
        require(a['flag'] == a['faultCount'] == a['recoveryUsed'] == 0 and a['appConfirmed'] == 1, 'Unexpected recovery flags')
        require(a['length'] == a['recoveryLength'] == len(image) and a['imageCrc'] == a['recoveryCrc'] == crc, 'Stored image CRC/length mismatch')
        require(a['restoreCount'] == result['initial_record']['restoreCount'], 'Unexpected recovery')
        for name, address, length in [('boot', 0, 0x10000), ('reserved', 0x40000, 0x10000), ('userrow', 0x804000, 256), ('pds_main', 0x401000, 256), ('pds_backup', 0x400100, 256)]:
            require(hashlib.sha256(p.read(address, length)).hexdigest() == result['baseline_hashes'][name], name + ' changed')
        return dict(app_sha256=hashlib.sha256(main).hexdigest(), backup_sha256=hashlib.sha256(recovery).hexdigest(), record=a, boot_userrow_pds_unchanged=True)

    try:
        p.connect()
        p.api('SetSpeed', None, [C.c_uint32])(1000)
        require(p.read(0x10000, len(image)) == image, 'Connected App differs from tested BIN')
        if not args.resume_after_sleep:
            require(byte(clock.symbol('gConFig')[0] + 22) == CHAIR, 'Connected chair changed')
        else:
            require(pds_info(p.read(0x401000, 256))['chair'] == '0B', 'Connected chair changed')
            require(record(p.read(0x401E00, 256))['flag'] == 1, 'Expected interrupted-update Boot state')
        uid = b''.join(p.read(a, 4) for a in [0x80A00C, 0x80A040, 0x80A044, 0x80A048])
        identity = zlib.crc32(uid)
        result['init_number'] = f'{identity:08X}'
        hashes = {name: hashlib.sha256(p.read(address, length)).hexdigest() for name, address, length in [('boot', 0, 0x10000), ('reserved', 0x40000, 0x10000), ('userrow', 0x804000, 256), ('pds_main', 0x401000, 256), ('pds_backup', 0x400100, 256)]}
        if previous is None:
            result['baseline_hashes'] = hashes
            result['initial_record'] = record(p.read(0x401E00, 256))
        else:
            require(hashes == result['baseline_hashes'], 'Boot/UserRow/PDS changed before resume')
            require(result['init_number'] == previous['init_number'], 'Target identity changed')
        result['initial_hardware'] = None if args.resume_after_sleep else snapshot()
        test = Test()
        test.allow_read_retry = args.resume
        test.connect()
        if args.resume_after_sleep:
            nad, mode = test.select(CHAIR, identity)
            require(mode == 1, 'Expected Boot after PC sleep interruption')
            original = bytes.fromhex(previous['product']['raw'])
            result['initial_boot_status'] = test.uds(nad, bytes.fromhex('22f210')).hex()
        else:
            original = test.info(CHAIR)
        info = transport.product(original)
        require(info['init'] == identity and info['chair'] == CHAIR and info['serial'] == '26I16-0002', 'LIN identity differs from SWD board')
        result['product'] = info
        if not args.resume_after_sleep:
            test.runtime(identity)
            require(test.status(CHAIR) == [0] * 6, 'Initial diagnostic fault')
        if previous is not None and not args.resume_after_sleep:
            item = dict(previous['in_progress'])
            item.update(readbacks=test.rgb_all((item['number'] - 1) % 3), flash=verify_flash(), hardware=snapshot(),
                        phase='passed_with_prior_read_error', passed=True, clean_first_attempt=False,
                        prior_error=previous['error'], postverification_time=datetime.now().isoformat())
            result['completed'].append(item)
            test.log('PRIOR_UPDATE_REVERIFIED', number=item['number'], flash_crc_accepted=True, app_bytes_match=True, backup_bytes_match=True, readbacks=len(item['readbacks']), prior_timeout_preserved=True)
        save()
        for number in range(result.get('resume_from', 1), 101):
            require(not (ROOT / 'validation' / (PREFIX + '.STOP')).exists(), 'Operator stop')
            require(IMAGE.read_bytes() == image, 'BIN changed during test')
            starting_in_boot = args.resume_after_sleep and number == 32
            if not starting_in_boot:
                before = test.runtime(identity)
                require(transport.stable_product(test.info(CHAIR)) == transport.stable_product(original), 'Product changed')
                require(test.status(CHAIR) == [0] * 6, 'Pre-update diagnostic fault')
            test.read_timeouts = []
            item = dict(number=number, started=datetime.now().isoformat(), phase='enter_boot')
            result['in_progress'] = item
            save()
            test.log('UPDATE_BEGIN', number=number, total=100)
            started = time.monotonic()
            frames = test.frame_count
            nad, mode = test.select(CHAIR, identity)
            if not starting_in_boot:
                require(mode == 0 and test.uds(nad, [0x10, 2])[:2] == b'\x50\x02', 'App programming session rejected')
                time.sleep(0.7)
                nad, mode = test.select(CHAIR, identity)
            require(mode == 1, 'Boot entry not confirmed')
            require(test.uds(nad, [0x10, 2])[:2] == b'\x50\x02', 'Boot programming session rejected')
            ready = test.uds(nad, b'\x34\x00\x44' + (0x10000).to_bytes(4, 'big') + len(image).to_bytes(4, 'big'), 25)
            require(ready == bytes.fromhex('74200082'), 'Unexpected transfer block size')
            item['phase'] = 'transfer'
            save()
            seq = 1
            for offset in range(0, len(image), 128):
                block = image[offset:offset + 128]
                require(test.uds(nad, bytes([0x36, seq]) + block) == bytes([0x76, seq]), 'Transfer acknowledgement mismatch')
                seq = (seq + 1) & 255
                if (offset // 128 + 1) % 58 == 0:
                    item['bytes_acknowledged'] = offset + len(block)
                    save()
                    test.log('UPDATE_PROGRESS', number=number, percent=(offset + len(block)) * 100 // len(image))
            require(test.uds(nad, b'\x37' + crc.to_bytes(4, 'big'), 3) == b'\x77', 'Device Flash CRC rejected')
            item.update(phase='app_restart', flash_crc_accepted=True, bytes_acknowledged=len(image))
            save()
            require(test.uds(nad, [0x11, 1]) == b'\x51\x01', 'Reset acknowledgement mismatch')
            time.sleep(3)
            memory = test.runtime(identity)
            require(memory[37] & 7 == 7 and memory[37] & 8 == 0, 'Backup/App confirmation mismatch')
            require(transport.stable_product(test.info(CHAIR)) == transport.stable_product(original), 'Product identity not retained')
            item['readbacks'] = test.rgb_all((number - 1) % 3)
            require(test.status(CHAIR) == [0] * 6, 'Post-update diagnostic fault')
            item['flash'] = verify_flash()
            item['hardware'] = snapshot()
            item.update(phase='passed', passed=True, seconds=round(time.monotonic() - started, 3), frames=test.frame_count - frames, runtime=memory.hex(), read_timeout_retries=list(test.read_timeouts), clean_first_attempt=not test.read_timeouts)
            result['completed'].append(item)
            result['in_progress'] = None
            test.frames.flush()
            save()
            test.log('UPDATE_PASS', number=number, seconds=item['seconds'], readbacks=len(item['readbacks']), flash_crc_accepted=True, app_bytes_match=True, backup_bytes_match=True, read_timeouts=len(test.read_timeouts))
        result['passed'] = True
        result['final_hardware'] = snapshot()
    except Exception as exc:
        result['error'] = repr(exc)
        result['traceback'] = traceback.format_exc()
        print('STOP_ON_FIRST_ERROR ' + repr(exc), flush=True)
    finally:
        # Do not reset or overwrite evidence on a failure during programming.
        if test:
            test.close()
        p.close()
        if timer_set:
            C.WinDLL('winmm').timeEndPeriod(1)
        result['finished'] = datetime.now().isoformat()
        save()
        print(json.dumps(dict(passed=result['passed'], completed=len(result['completed']), error=result.get('error'), report=str(output))), flush=True)
    if not result['passed']:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
