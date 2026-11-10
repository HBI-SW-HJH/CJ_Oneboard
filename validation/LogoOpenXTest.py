"""Console LIN download and Strobe checks, without GUI or product source changes."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import time
import zlib

import StrobeOnlyLinTest as s

s.IMAGE = s.ROOT / 'CJ_OneBoard_App/build/Debug/Exe/CJ_App_Test_20261110_LogoOpenX.bin'
s.EXPECTED_SHA = '85a14a03832a3fdee0b6a4660bd2803c61833ea42421b15766b03fa42367c791'


def capture(test, chair, hz, duty, windows=3):
    raw_duty = duty * 250 // 100
    test.lamp(chair, 0x20, (26, 26, 26), 0x16, raw_duty, hz)
    time.sleep(0.1)
    samples = []
    for window in range(windows):
        test.verify_command(chair, 2, 5, capture=1000)
        time.sleep(1.05)
        response = test.verification(chair, lambda data: data[:2] == bytes([0x82, 5]), 1)
        values = list(response[2:8])
        s.require(all(0 < value < 255 for value in values), 'Incomplete/saturated flicker measurement')
        samples.append(dict(period_ms=values[:2], on_ms=values[2:4], off_ms=values[4:], raw=response.hex()))
    item = dict(hz=hz, duty_percent=duty, windows=samples)
    for key in ('period_ms', 'on_ms', 'off_ms'):
        item[key] = [min(row[key][0] for row in samples), max(row[key][1] for row in samples)]
    nominal_on = (raw_duty * 1000 + hz * 125) // (hz * 250)
    item['within_one_ms_band'] = (item['period_ms'][0] >= math.floor(1000 / hz) - 1 and item['period_ms'][1] <= math.ceil(1000 / hz) + 1 and item['on_ms'][0] >= nominal_on - 1 and item['on_ms'][1] <= nominal_on + 1 and item['off_ms'][0] >= math.floor(1000 / hz) - nominal_on - 1 and item['off_ms'][1] <= math.ceil(1000 / hz) - nominal_on + 1)
    test.strobe_diag(chair)
    test.log('FLICKER_CASE', chair=f'{chair:02X}', **item)
    return item


def colors(test, chair):
    for name, rgb in [('R', (26, 0, 0)), ('G', (0, 26, 0)), ('B', (0, 0, 26)), ('W', (26, 26, 26)), ('OFF', (0, 0, 0))]:
        test.lamp(chair, 0x20, rgb, 0x07 if name == 'OFF' else 0x17)
        time.sleep(0.1)
        for node in range(8):
            test.read_strobe(chair, node, rgb, name)
        test.strobe_diag(chair)
        test.log('COLOR_PASS', chair=f'{chair:02X}', color=name, nodes=8)


def update(test, result, journal, chair):
    image, info = s.image_info()
    key = f'{chair:02X}'
    baseline = result['boards'][key]['product']
    original = bytes.fromhex(baseline['raw'])
    peers = {peer: test.info(peer) for peer in s.CHAIRS if peer != chair}
    before = test.confirmed(chair, baseline['init'])
    s.require(s.transport.stable_product(test.info(chair)) == s.transport.stable_product(original), 'Product changed')
    address = int.from_bytes(before[4:8], 'big')
    s.require(address == 0x10000 and len(image) <= int.from_bytes(before[8:12], 'big'), 'APP range mismatch')
    item = dict(chair=key, image=info, passed=False, started=time.strftime('%Y-%m-%dT%H:%M:%S'))
    result['in_progress'] = item
    s.save(journal, result)
    test.log('UPDATE_BEGIN', **item)
    started = time.monotonic()
    nad, mode = test.select(chair, baseline['init'])
    s.require(mode == 0 and test.uds(nad, [0x10, 2])[:2] == b'\x50\x02', 'APP programming session')
    time.sleep(0.7)
    nad, mode = test.select(chair, baseline['init'])
    s.require(mode == 1 and test.uds(nad, [0x10, 2])[:2] == b'\x50\x02', 'BOOT programming session')
    s.require(test.uds(nad, b'\x34\x00\x44' + address.to_bytes(4, 'big') + len(image).to_bytes(4, 'big'), 25) == bytes.fromhex('74200082'), 'Download block size')
    sequence, progress = 1, 0
    for offset in range(0, len(image), 128):
        block = image[offset:offset + 128]
        s.require(test.uds(nad, bytes([0x36, sequence]) + block) == bytes([0x76, sequence]), 'Transfer acknowledgement')
        sequence = (sequence + 1) & 255
        percent = (offset + len(block)) * 100 // len(image)
        if percent >= progress + 10 or percent == 100:
            progress = percent
            test.log('UPDATE_PROGRESS', chair=key, percent=percent)
    s.require(test.uds(nad, b'\x37' + zlib.crc32(image).to_bytes(4, 'big'), 3) == b'\x77', 'Device Flash CRC failed')
    item['flash_crc_accepted'] = True
    test.log('FLASH_CRC_PASS', chair=key, crc=info['crc32'])
    s.require(test.uds(nad, [0x11, 1]) == b'\x51\x01', 'Reset acknowledgement')
    time.sleep(2)
    memory = test.confirmed(chair, baseline['init'])
    s.require(s.transport.stable_product(test.info(chair)) == s.transport.stable_product(original), 'Identity not retained')
    s.require(memory[37] & 7 == 7 and memory[37] & 8 == 0, 'Confirmation/backup status')
    s.require(memory[29:33] == before[29:33], 'Unexpected restoration')
    test.strobe_diag(chair)
    for peer, expected in peers.items():
        s.require(test.info(peer) == expected, 'Non-target board identity changed')
    item.update(passed=True, seconds=round(time.monotonic() - started, 3), runtime=memory.hex(), blocks=(len(image) + 127) // 128)
    result['updates'].append(item)
    result['in_progress'] = None
    s.save(journal, result)
    test.log('UPDATE_PASS', **item)


def fade(test, chair, duration, direction):
    test.verify_command(chair, 4)
    time.sleep(1.8)
    test.lamp(chair, 0x20, (26, 26, 26), 0x37 if direction == 'in' else 0x27, duration // 100, duration // 100)
    started = time.monotonic()
    samples = []
    while time.monotonic() - started < duration / 1000 + 0.3:
        status, rgb = test.rgb(chair, 5, 0)
        s.require(status & 0xF1 == 1 and rgb[0] == rgb[1] == rgb[2], 'Fade read/status mismatch')
        samples.append(dict(ms=round((time.monotonic() - started) * 1000, 3), rgb=rgb, status=status))
    values = [row['rgb'][0] for row in samples]
    s.require(len(set(values)) >= 5, 'Missing fade progression')
    s.require(all((b >= a if direction == 'in' else b <= a) for a, b in zip(values, values[1:])), 'Fade reversed')
    target = (26, 26, 26) if direction == 'in' else (0, 0, 0)
    s.require(values[-1] == target[0] * 32767 // 255, 'Fade final value')
    for node in range(8):
        test.read_strobe(chair, node, target, f'Fade {direction} {duration} ms final')
    test.strobe_diag(chair)
    item = dict(direction=direction, requested_ms=duration, samples=samples, monotonic=True, all_8_nodes_final_pass=True)
    test.log('FADE_PASS', chair=f'{chair:02X}', direction=direction, requested_ms=duration, samples=len(samples))
    return item


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--prefix', required=True)
    parser.add_argument('--baseline', required=True)
    args = parser.parse_args()
    s.require(Path(args.prefix).name == args.prefix, 'Invalid prefix')
    journal = s.ROOT / 'validation' / (args.prefix + '_result.json')
    s.require(not journal.exists(), 'Do not overwrite results/repeat updates')
    baseline = json.loads(Path(args.baseline).read_text(encoding='utf-8'))
    s.require(baseline['completed'], 'Preflight incomplete')
    _, info = s.image_info()
    result = dict(completed=False, boards=baseline['boards'], image=info, updates=[], in_progress=None, stage='connect', gui=False, jlink=False, optical=False, brightness_raw=26, normal_diagnostics_active=True, logo_periodic_recovery_disabled=True)
    test = s.StrobeTest(args.prefix)
    active = None
    try:
        test.connect()
        for chair in s.CHAIRS:
            active = chair
            key = f'{chair:02X}'
            result['stage'] = f'{key} baseline'
            test.verify_command(chair, 4)
            result['boards'][key]['before_30hz_50'] = capture(test, chair, 30, 50)
            test.off(chair)
            result['stage'] = f'{key} download'
            update(test, result, journal, chair)
            result['stage'] = f'{key} post-update 30Hz'
            test.verify_command(chair, 4)
            sample = capture(test, chair, 30, 50, 6)
            result['boards'][key]['after_30hz_50'] = sample
            s.save(journal, result)
            s.require(sample['within_one_ms_band'], 'Post-update 30Hz timing outside 1ms band; stop further updates')
            colors(test, chair)
            test.off(chair)
        for chair in s.CHAIRS:
            active = chair
            key = f'{chair:02X}'
            board = result['boards'][key]
            board['flicker'], board['fade'] = [], []
            test.verify_command(chair, 4)
            for hz in (5, 10, 15, 20, 25, 30):
                for duty in (10, 50, 90):
                    result['stage'] = f'{key} flicker {hz}Hz {duty}%'
                    sample = capture(test, chair, hz, duty)
                    board['flicker'].append(sample)
                    s.save(journal, result)
                    s.require(sample['within_one_ms_band'], 'Flicker timing outside 1ms band')
            test.off(chair)
            time.sleep(0.1)
            for duration in (500, 1000, 3000):
                for direction in ('in', 'out'):
                    result['stage'] = f'{key} fade {direction} {duration}ms'
                    board['fade'].append(fade(test, chair, duration, direction))
                    s.save(journal, result)
            test.off(chair)
            time.sleep(0.1)
            for node in range(8):
                test.read_strobe(chair, node, (0, 0, 0), 'Final OFF')
            test.verify_command(chair, 4)
            board['final_runtime'] = test.confirmed(chair, board['product']['init']).hex()
            board['final_faults'] = test.strobe_diag(chair)
            active = None
        result['stage'] = 'complete'
        result['completed'] = True
    except Exception as error:
        result['error'] = str(error)
        test.log('STOP_ON_FIRST_ERROR', stage=result['stage'], error=str(error))
    finally:
        if active is not None and test.connected and not test.competing:
            try:
                test.off(active)
                test.verify_command(active, 4)
            except Exception as error:
                result['cleanup_error'] = str(error)
        result['readback_checks'] = test.detail_count
        s.save(journal, result)
        test.close()
        print('REPORT=' + str(journal), flush=True)
    return 0 if result['completed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
