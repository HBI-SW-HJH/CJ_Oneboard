"""Console-only LIN test; no GUI, J-Link, Boot download or product-source edits."""
import argparse
import ctypes as C
import hashlib
import json
from pathlib import Path
import struct
import sys
import time
import zlib

HELPERS = Path(r'D:\jhhan\program\CJ_OneBoard\Program\CJ_Onboard_20261027\validation')
sys.path.insert(0, str(HELPERS))
import FullFourChairTest as transport
from RuntimeCrcLinReadOnly import ReadOnly

ROOT = Path(__file__).resolve().parent.parent
transport.ROOT = ROOT
CHAIRS = (0x0A, 0x0B, 0x0C)
IMAGE = ROOT / 'CJ_OneBoard_App/build/Debug/Exe/CJ_App_mass_20261110.bin'
EXPECTED_SHA = '583cf89c7bc3ffb5966b397ed0d95d624c38f1ff6b268051cceb31563671ade8'
require = transport.require


class StrobeTest(transport.Test):
    def __init__(self, prefix):
        super().__init__(prefix, full_brightness=True)
        self.initialized = False
        self.timer_set = C.WinDLL('winmm').timeBeginPeriod(1) == 0
        self.detail_count = 0

    def connect(self):
        ReadOnly.connect(self)

    def off(self, chair):
        self.lamp(chair, 0x20, (0, 0, 0), control=0x07)

    def close(self):
        if self.initialized and self.connected:
            self.log('ADAPTER_DISABLED', result=self.api('InitializeHardware', [C.c_uint8, C.c_uint16, C.c_uint8, C.c_uint16])(self.client, self.hw, 0, 19200))
        if self.timer_set:
            C.WinDLL('winmm').timeEndPeriod(1)
        super().close()

    def runtime(self, chair, identity):
        nad, mode = self.select(chair, identity)
        require(mode == 0 and nad == chair - 9, f'{chair:02X}: expected APP')
        data = self.uds(nad, bytes.fromhex('22f210'))
        require(len(data) == 38 and data[3] == 0, 'APP runtime response')
        require(data[12:28] == bytes(16), 'Update/HardFault flags are not zero')
        return data

    def confirmed(self, chair, identity):
        deadline = time.monotonic() + 16
        while True:
            data = self.runtime(chair, identity)
            if data[28] == 3 and data[37] & 4:
                return data
            require(time.monotonic() < deadline, 'APP confirmation timeout')
            time.sleep(0.5)

    def strobe_diag(self, chair):
        faults = self.status(chair)
        require(faults[5] == 0, f'{chair:02X}: Strobe diagnostic fault 0x{faults[5]:02X}')
        return faults

    def read_strobe(self, chair, node, rgb, label):
        expected = [value * 32767 // 255 for value in rgb]
        status, actual = self.rgb(chair, 5, node)
        passed = status & 0xF1 == 1 and actual == expected
        self.detail_count += 1
        self.log('DETAIL', chair=f'{chair:02X}', node=node + 1, test=label, expected=expected, actual=actual, status=status, passed=passed)
        require(passed, f'{chair:02X} Strobe LED{node + 1}: {label}, status={status:02X}, expected={expected}, actual={actual}')

    def colors(self, chair, full):
        # Do not send VERIFICATION_START: normal 2-second Logo recovery stays active.
        self.verify_command(chair, 4)
        palette = [('R', (26, 0, 0)), ('G', (0, 26, 0)), ('B', (0, 0, 26)), ('W', (26, 26, 26))] if full else [('W', (26, 26, 26))]
        for name, rgb in palette:
            self.lamp(chair, 0x20, rgb)
            time.sleep(0.08)
            end = time.monotonic() + 2.3
            while time.monotonic() < end:
                self.read_strobe(chair, 0, rgb, name + ' hold with Logo recovery active')
                time.sleep(0.1)
            for node in range(8):
                self.read_strobe(chair, node, rgb, name)
            self.strobe_diag(chair)
            self.log('STROBE_COLOR_PASS', chair=f'{chair:02X}', color=name, nodes=8, normal_logo_recovery=True)
        self.off(chair)
        time.sleep(0.1)
        for node in range(8):
            self.read_strobe(chair, node, (0, 0, 0), 'OFF')
        self.verify_command(chair, 4)


def save(path, result):
    path.write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')


def image_info():
    data = IMAGE.read_bytes()
    stack, entry = struct.unpack_from('<II', data)
    sha = hashlib.sha256(data).hexdigest()
    require(sha == EXPECTED_SHA, 'Requested firmware file changed')
    require(192 <= len(data) <= 192 * 1024 and 0x20000000 < stack <= 0x20010000 and stack % 8 == 0 and entry & 1 and 0x10000 <= entry < 0x10000 + len(data), 'Invalid APP image/vector')
    return data, dict(path=str(IMAGE), length=len(data), sha256=sha, crc32=f'{zlib.crc32(data):08X}', stack=f'{stack:08X}', entry=f'{entry:08X}')


def preflight(test, result):
    _, result['image'] = image_info()
    result['boards'] = {}
    for chair in CHAIRS:
        info = transport.product(test.info(chair))
        require(info['chair'] == chair and info['init'] not in (0, 0xFFFFFFFF), 'Unexpected board identity')
        memory = test.confirmed(chair, info['init'])
        faults = test.strobe_diag(chair)
        result['boards'][f'{chair:02X}'] = dict(product=info, runtime=memory.hex(), faults=faults)
        test.log('BOARD', chair=f'{chair:02X}', product=info, runtime=memory.hex(), faults=faults)
    require(len({b['product']['init'] for b in result['boards'].values()}) == 3, 'Duplicate board identities')
    for chair in CHAIRS:
        result['stage'] = f'preflight colors {chair:02X}'
        test.colors(chair, True)
    result['preflight_passed'] = True


def update_one(test, result, journal, round_number, chair):
    image, info = image_info()
    key = f'{chair:02X}'
    baseline = result['boards'][key]['product']
    original = bytes.fromhex(baseline['raw'])
    peers = {peer: test.info(peer) for peer in CHAIRS if peer != chair}
    require(transport.stable_product(test.info(chair)) == transport.stable_product(original), 'Product identity changed')
    before = test.confirmed(chair, baseline['init'])
    test.strobe_diag(chair)
    address, capacity = int.from_bytes(before[4:8], 'big'), int.from_bytes(before[8:12], 'big')
    require(address == 0x10000 and len(image) <= capacity, 'Target APP memory mismatch')
    restore_count = int.from_bytes(before[29:33], 'big')
    item = dict(number=len(result['updates']) + 1, round=round_number, chair=key, image=info, started=time.strftime('%Y-%m-%dT%H:%M:%S'))
    result['in_progress'] = item
    result['stage'] = f'update {item["number"]}/9 chair {key}'
    save(journal, result)
    test.log('UPDATE_BEGIN', **item)
    started = time.monotonic()
    nad, mode = test.select(chair, baseline['init'])
    require(mode == 0, 'Expected APP before update')
    require(test.uds(nad, [0x10, 2])[:2] == b'\x50\x02', 'APP programming session')
    time.sleep(0.7)
    nad, mode = test.select(chair, baseline['init'])
    require(mode == 1, 'BOOT entry not confirmed')
    require(test.uds(nad, [0x10, 2])[:2] == b'\x50\x02', 'BOOT programming session')
    ready = test.uds(nad, b'\x34\x00\x44' + address.to_bytes(4, 'big') + len(image).to_bytes(4, 'big'), 25)
    require(ready == bytes.fromhex('74200082'), 'Download block size mismatch')
    sequence, progress = 1, 0
    for offset in range(0, len(image), 128):
        block = image[offset:offset + 128]
        require(test.uds(nad, bytes([0x36, sequence]) + block) == bytes([0x76, sequence]), 'Transfer acknowledgement mismatch')
        sequence = (sequence + 1) & 255
        percent = (offset + len(block)) * 100 // len(image)
        if percent >= progress + 10 or percent == 100:
            progress = percent
            test.log('UPDATE_PROGRESS', number=item['number'], chair=key, percent=percent)
    require(test.uds(nad, b'\x37' + zlib.crc32(image).to_bytes(4, 'big'), 3) == b'\x77', 'Device Flash CRC verification failed')
    memory = test.uds(nad, bytes.fromhex('22f210'))
    require(len(memory) == 38 and memory[28] == 3 and memory[12:20] == bytes(8), 'Downloaded update records invalid')
    test.log('UPDATE_FLASH_CRC_PASS', number=item['number'], chair=key, crc32=info['crc32'])
    require(test.uds(nad, [0x11, 1]) == b'\x51\x01', 'Reset acknowledgement')
    time.sleep(2)
    test.runtime(chair, baseline['init'])
    require(transport.stable_product(test.info(chair)) == transport.stable_product(original), 'Product identity not retained')
    test.colors(chair, False)
    memory = test.confirmed(chair, baseline['init'])
    require(memory[37] & 7 == 7 and memory[37] & 8 == 0, 'Backup/confirmation status abnormal')
    require(int.from_bytes(memory[29:33], 'big') == restore_count, 'Unexpected recovery during update')
    faults = test.strobe_diag(chair)
    for peer, expected in peers.items():
        require(test.info(peer) == expected, 'Non-target identity changed')
        test.strobe_diag(peer)
    item.update(passed=True, seconds=round(time.monotonic() - started, 3), runtime=memory.hex(), product=transport.product(test.info(chair)), faults=faults, blocks=(len(image) + 127) // 128, flash_crc_accepted=True)
    result['updates'].append(item)
    result['in_progress'] = None
    save(journal, result)
    test.log('UPDATE_PASS', **item)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('phase', choices=['preflight', 'updates'])
    parser.add_argument('--prefix', required=True)
    args = parser.parse_args()
    require(Path(args.prefix).name == args.prefix, 'Invalid log prefix')
    journal = ROOT / 'validation' / (args.prefix + '_result.json')
    if args.phase == 'preflight':
        require(not journal.exists(), 'Existing journal must not be overwritten')
        result = dict(completed=False, preflight_passed=False, stage='preflight', updates=[], in_progress=None, gui=False, jlink=False, boot_downloaded=False, optical_measurement=False, absent_zones_ignored=['LH', 'RH', 'Head', 'Back', 'Logo'])
    else:
        result = json.loads(journal.read_text(encoding='utf-8'))
        require(result['preflight_passed'] and not result['updates'] and result['in_progress'] is None, 'Unexpected journal state; do not repeat downloads')
    test = StrobeTest(args.prefix)
    code = 1
    try:
        test.connect()
        if args.phase == 'preflight':
            preflight(test, result)
        else:
            for round_number in range(1, 4):
                for chair in CHAIRS:
                    update_one(test, result, journal, round_number, chair)
            result['completed'] = True
            result['stage'] = 'all 9 updates passed'
        code = 0
    except Exception as error:
        result['error'] = str(error)
        test.log('STOP_ON_FIRST_ERROR', stage=result['stage'], error=str(error), completed_updates=len(result['updates']))
    finally:
        result['readback_checks'] = result.get('readback_checks', 0) + test.detail_count
        save(journal, result)
        test.close()
        print('REPORT=' + str(journal), flush=True)
    return code


if __name__ == '__main__':
    raise SystemExit(main())
