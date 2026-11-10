"""LIN-only Strobe characterization to 30 Hz; no firmware edits or downloads."""
import argparse
import json
import math
from pathlib import Path
import time
from StrobeOnlyLinTest import StrobeTest, ROOT, CHAIRS, require


def save(path, result):
    path.write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--prefix', default='StrobeOnly30Hz_20261110')
    parser.add_argument('--chair', type=lambda value: int(value, 16), choices=CHAIRS)
    args = parser.parse_args()
    prefix = args.prefix
    require(Path(prefix).name == prefix, 'Invalid report prefix')
    path = ROOT / 'validation' / (prefix + '_result.json')
    require(not path.exists(), 'Do not overwrite earlier results')
    baseline = json.loads((ROOT / 'validation/StrobeOnly_20261110_result.json').read_text(encoding='utf-8'))
    test = StrobeTest(prefix)
    result = dict(completed=False, stage='connect', boards=[], frequencies_hz=[5, 10, 15, 20, 25, 30], duties_percent=[10, 50, 90], normal_logo_recovery=True, brightness_raw=26, optical=False, firmware_modified=False)
    active_chair = None
    code = 1
    try:
        test.connect()
        for chair in ((args.chair,) if args.chair is not None else CHAIRS):
            active_chair = chair
            identity = baseline['boards'][f'{chair:02X}']['product']['init']
            test.runtime(chair, identity)
            test.strobe_diag(chair)
            test.verify_command(chair, 4)
            test.off(chair)
            board = dict(chair=f'{chair:02X}', flicker=[], fade=[])
            result['boards'].append(board)
            cases = [(30, 50)] + [(hz, duty) for hz in (5, 10, 15, 20, 25, 30) for duty in (10, 50, 90) if (hz, duty) != (30, 50)]
            for hz, duty in cases:
                result['stage'] = f'{chair:02X} flicker {hz} Hz {duty}%'
                raw_duty = duty * 250 // 100
                on_nominal = (raw_duty * 1000 + hz * 125) // (hz * 250)
                test.lamp(chair, 0x20, (26, 26, 26), 0x16, raw_duty, hz)
                time.sleep(0.1)
                windows = []
                for window in range(3):
                    test.verify_command(chair, 2, 5, capture=1000)
                    time.sleep(1.05)
                    response = test.verification(chair, lambda data: data[:2] == bytes([0x82, 5]), 1)
                    values = list(response[2:8])
                    require(all(value > 0 for value in values), 'Incomplete flicker transition measurement')
                    windows.append(dict(window=window + 1, period_ms=values[:2], on_ms=values[2:4], off_ms=values[4:6], raw=response.hex(), saturated_255ms=255 in values))
                item = dict(hz=hz, duty_percent=duty, nominal_period_ms=1000 / hz, nominal_on_ms=on_nominal, period_ms=[min(x['period_ms'][0] for x in windows), max(x['period_ms'][1] for x in windows)], on_ms=[min(x['on_ms'][0] for x in windows), max(x['on_ms'][1] for x in windows)], off_ms=[min(x['off_ms'][0] for x in windows), max(x['off_ms'][1] for x in windows)], windows=windows)
                item['within_one_ms_band'] = item['period_ms'][0] >= math.floor(1000 / hz) - 1 and item['period_ms'][1] <= math.ceil(1000 / hz) + 1 and item['on_ms'][0] >= on_nominal - 1 and item['on_ms'][1] <= on_nominal + 1 and item['off_ms'][0] >= math.floor(1000 / hz) - on_nominal - 1 and item['off_ms'][1] <= math.ceil(1000 / hz) - on_nominal + 1
                board['flicker'].append(item)
                test.strobe_diag(chair)
                test.log('FLICKER_CASE', chair=f'{chair:02X}', **{key: value for key, value in item.items() if key != 'windows'})
            test.off(chair)
            time.sleep(0.1)
            for duration_ms in (500, 1000, 3000):
                for direction in ('in', 'out'):
                    result['stage'] = f'{chair:02X} fade {direction} {duration_ms} ms'
                    # STOP resets the normal 2-second read/recovery counters; it does not disable them.
                    test.verify_command(chair, 4)
                    time.sleep(1.8)
                    test.lamp(chair, 0x20, (26, 26, 26), 0x37 if direction == 'in' else 0x27, duration_ms // 100, duration_ms // 100)
                    started = time.monotonic()
                    samples = []
                    while time.monotonic() - started < duration_ms / 1000 + 0.3:
                        status, rgb = test.rgb(chair, 5, 0)
                        require(status & 0xF1 == 1, 'Strobe diagnosis/read error during fade')
                        require(rgb[0] == rgb[1] == rgb[2], 'Fade RGB mismatch')
                        samples.append(dict(ms=round((time.monotonic() - started) * 1000, 3), rgb=rgb, status=status))
                    values = [x['rgb'][0] for x in samples]
                    require(len(set(values)) >= 5, 'Missing fade progression')
                    require(all((b >= a if direction == 'in' else b <= a) for a, b in zip(values, values[1:])), 'Fade direction reversed')
                    target = (26, 26, 26) if direction == 'in' else (0, 0, 0)
                    target_value = target[0] * 32767 // 255
                    require(values[-1] == target_value, 'Fade final value mismatch')
                    reached = next(x['ms'] for x in samples if x['rgb'][0] == target_value)
                    for node in range(8):
                        test.read_strobe(chair, node, target, f'Fade {direction} {duration_ms} ms final')
                    item = dict(direction=direction, requested_ms=duration_ms, first_observed_final_ms=reached, samples=samples, monotonic=True, all_8_nodes_final_pass=True)
                    board['fade'].append(item)
                    test.strobe_diag(chair)
                    test.log('FADE_CASE', chair=f'{chair:02X}', direction=direction, requested_ms=duration_ms, first_observed_final_ms=reached, samples=len(samples), final_nodes=8, passed=True)
            test.off(chair)
            time.sleep(0.1)
            for node in range(8):
                test.read_strobe(chair, node, (0, 0, 0), 'Final OFF')
            test.verify_command(chair, 4)
            test.runtime(chair, identity)
            test.strobe_diag(chair)
            active_chair = None
        result['completed'] = True
        result['stage'] = 'measurement completed; evaluate timing separately'
        code = 0
    except Exception as error:
        result['error'] = str(error)
        test.log('STOP_ON_FIRST_ERROR', stage=result['stage'], error=str(error))
    finally:
        if active_chair is not None and test.connected and not test.competing:
            try:
                test.off(active_chair)
                test.verify_command(active_chair, 4)
            except Exception as error:
                result['cleanup_error'] = str(error)
        save(path, result)
        test.close()
        print('REPORT=' + str(path), flush=True)
    return code


if __name__ == '__main__':
    raise SystemExit(main())
