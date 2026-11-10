"""Characterize Strobe effects with normal Logo recovery left enabled."""
import json
from pathlib import Path
import time
from StrobeOnlyLinTest import StrobeTest, CHAIRS, ROOT, require


def main():
    prefix = 'StrobeOnlyEffects_20261110'
    report = ROOT / 'validation' / (prefix + '_result.json')
    require(not report.exists(), 'Do not overwrite effect results')
    updates = json.loads((ROOT / 'validation/StrobeOnly_20261110_result.json').read_text(encoding='utf-8'))
    require(updates['completed'] and len(updates['updates']) == 9, 'Finish the requested updates first')
    test = StrobeTest(prefix)
    result = dict(completed=False, boards=[], optical=False, normal_logo_recovery=True, brightness_raw=26)
    active_chair = None
    code = 1
    try:
        test.connect()
        for chair in CHAIRS:
            active_chair = chair
            item = dict(chair=f'{chair:02X}', fade=[], flicker=[])
            result['boards'].append(item)
            test.verify_command(chair, 4)
            test.off(chair)
            time.sleep(0.1)
            for direction in ('in', 'out'):
                test.lamp(chair, 0x20, (26, 26, 26), 0x37 if direction == 'in' else 0x27, 30, 30)
                start = time.monotonic()
                samples = []
                while time.monotonic() - start < 3.5:
                    status, rgb = test.rgb(chair, 5, 0)
                    require(status & 0xF1 == 1, 'Strobe fault during fade')
                    require(rgb[0] == rgb[1] == rgb[2], 'Fade RGB channels disagree')
                    samples.append(dict(ms=round((time.monotonic() - start) * 1000, 2), rgb=rgb, status=status))
                    time.sleep(0.08)
                values = [sample['rgb'][0] for sample in samples]
                require(len(set(values)) >= 8, 'Insufficient fade progression')
                require(all((b >= a if direction == 'in' else b <= a) for a, b in zip(values, values[1:])), 'Fade progression reversed')
                target = (26, 26, 26) if direction == 'in' else (0, 0, 0)
                for node in range(8):
                    test.read_strobe(chair, node, target, 'Fade ' + direction + ' final')
                item['fade'].append(dict(direction=direction, duration_requested_ms=3000, samples=samples, monotonic=True, final_nodes=8, passed=True))
                test.log('FADE_PASS', chair=f'{chair:02X}', direction=direction, samples=len(samples), normal_logo_recovery=True)
            for hz in (10, 50):
                test.lamp(chair, 0x20, (26, 26, 26), 0x16, 125, hz)
                time.sleep(0.6)
                for window in range(3):
                    test.verify_command(chair, 2, 5, capture=1000)
                    time.sleep(1.05)
                    response = test.verification(chair, lambda data: data[:2] == bytes([0x82, 5]), 1)
                    timings = list(response[2:8])
                    require(all(value not in (0, 255) for value in timings), 'No complete flicker periods captured')
                    data = dict(hz=hz, duty_percent=50, window=window + 1, period_ms=timings[0:2], on_ms=timings[2:4], off_ms=timings[4:6], raw=response.hex(), measurement='MCU output-value transition timing; not an optical waveform')
                    item['flicker'].append(data)
                    test.log('FLICKER_MEASURED', chair=f'{chair:02X}', **data)
            test.off(chair)
            time.sleep(0.1)
            for node in range(8):
                test.read_strobe(chair, node, (0, 0, 0), 'Final OFF')
            test.verify_command(chair, 4)
            test.strobe_diag(chair)
            test.runtime(chair, updates['boards'][f'{chair:02X}']['product']['init'])
            active_chair = None
        result['completed'] = True
        code = 0
    except Exception as error:
        result['error'] = str(error)
        test.log('STOP_ON_FIRST_ERROR', error=str(error))
    finally:
        if active_chair is not None and test.connected and not test.competing:
            try:
                test.off(active_chair)
                test.verify_command(active_chair, 4)
            except Exception as error:
                result['cleanup_error'] = str(error)
        report.write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
        test.close()
        print('REPORT=' + str(report), flush=True)
    return code


if __name__ == '__main__':
    raise SystemExit(main())
