"""J-Link clock tests on chair 0B; no source, fuse, or Flash writes.

The App download is performed separately with its existing IAR loader.
Fault injection changes a CPU register at a hardware breakpoint, suppressing
XOSC/DPLL ENABLE. Original retry and timeout code then runs unchanged.
"""
import argparse
import ast
import ctypes as C
from datetime import datetime
import hashlib
import json
from pathlib import Path
import re
import sys
import time
import traceback
import uuid

ROOT = Path(__file__).resolve().parent.parent
APP = ROOT / 'CJ_OneBoard_App'
OLD = ROOT.parent / 'old/CJ_Onboard_20261027/validation'
MAP = (APP / 'build/Debug/List/CJ_OneBoard_APP.map').read_text()
sys.path.insert(0, str(OLD))
from RuntimeCrcReadOnly import Probe

# Reuse only pure parsing/packet declarations, not old board addresses or tests.
tree = ast.parse((OLD / 'StressLow_20261019/hardware.py').read_text())
selected = [n for n in tree.body if isinstance(n, (ast.FunctionDef, ast.ClassDef)) and n.name in ('symbol', 'cstruct', 'Msg', 'Rx', 'pid')]
exec(compile(ast.Module(body=selected, type_ignores=[]), 'readonly_helpers', 'exec'))
SRGB = cstruct('SRGBControl.h', 'SRGB_Control_t')
OSP = cstruct('OSPControl.h', 'OSP_Control_t')
assert C.sizeof(SRGB) == symbol('gSRGBControl')[1] == 356
assert C.sizeof(OSP) == symbol('gOSPControl')[1] == 1768


class Lin:
    def __init__(self):
        self.lib = C.WinDLL(r'C:\Windows\System32\PLinApi.dll')
        self.client = C.c_uint8()
        hardware = (C.c_uint16 * 16)()
        count = C.c_uint16()
        self.call('GetAvailableHardware', [C.POINTER(C.c_uint16), C.c_uint16, C.POINTER(C.c_uint16)], hardware, C.sizeof(hardware), C.byref(count))
        assert count.value == 1, 'Expected one LIN adapter'
        self.hw = hardware[0]
        clients = (C.c_uint8 * 256)()
        self.call('GetHardwareParam', [C.c_uint16, C.c_uint16, C.c_void_p, C.c_uint16], self.hw, 4, clients, 256)
        settings = {}
        for parameter in (6, 7, 21):
            value = C.c_uint32()
            self.call('GetHardwareParam', [C.c_uint16, C.c_uint16, C.c_void_p, C.c_uint16], self.hw, parameter, C.byref(value), 4)
            settings[parameter] = value.value
        assert settings[21] == 0, 'Active LIN schedule'
        assert (settings[6] == 19200 and settings[7] == 2) or (settings[7] == 0 and clients[0] == 0), ('Adapter busy', settings)
        self.initialized = settings[7] == 0
        self.call('RegisterClient', [C.c_char_p, C.c_void_p, C.POINTER(C.c_uint8)], ('ClockB_' + uuid.uuid4().hex[:8]).encode(), None, C.byref(self.client))
        self.call('ConnectClient', [C.c_uint8, C.c_uint16], self.client, self.hw)
        self.call('SetClientFilter', [C.c_uint8, C.c_uint16, C.c_uint64], self.client, self.hw, sum(1 << n for n in [1, 0x0B, 0x29, 0x2D]))
        if self.initialized:
            self.call('InitializeHardware', [C.c_uint8, C.c_uint16, C.c_uint8, C.c_uint16], self.client, self.hw, 2, 19200)
        self.sequence = 0

    def call(self, name, types, *args):
        fn = getattr(self.lib, 'LIN_' + name)
        fn.argtypes, fn.restype = types, C.c_int
        result = fn(*args)
        if result:
            raise RuntimeError(('PLIN', name, result))

    def frame(self, frame, data=None):
        assert frame in (1, 0x0B, 0x29, 0x2D)
        if frame == 1:
            assert data[0] == 2 and max(data[3:6]) <= 12
        msg = Msg(pid(frame), 8, 2 if data is None else 1, 2, (C.c_uint8 * 8)(*(data or [0] * 8)), 0)
        if data is not None:
            self.call('CalculateChecksum', [C.POINTER(Msg)], C.byref(msg))
        self.call('Write', [C.c_uint8, C.c_uint16, C.POINTER(Msg)], self.client, self.hw, C.byref(msg))
        read = self.lib.LIN_Read
        read.argtypes, read.restype = [C.c_uint8, C.POINTER(Rx)], C.c_int
        deadline = time.monotonic() + 0.5
        while time.monotonic() < deadline:
            rx = Rx()
            status = read(self.client, C.byref(rx))
            if status == 3:
                time.sleep(0.001)
                continue
            assert status == 0, ('LIN read', status)
            if rx.kind == 0 and rx.pid == msg.pid:
                assert rx.errors == 0, ('LIN errors', frame, rx.errors)
                return bytes(rx.data)
        raise TimeoutError(('LIN response', frame))

    def off(self):
        self.frame(1, [2, 63, 7, 0, 0, 0, 0, 0])

    def status(self):
        value = self.frame(0x0B).hex()
        assert value == '0020406080a00000', ('Diagnosis', value)
        return value

    def read_rgb(self, zone, node):
        self.sequence = self.sequence % 255 + 1
        self.frame(0x29, [5, zone, node, 0, 1, self.sequence, 0, 0])
        time.sleep(0.015)
        actual = self.frame(0x2D)
        assert actual[7] == self.sequence, ('Stale RGB', actual.hex(), self.sequence)
        self.frame(0x29, [10, zone, node, 0, 0, self.sequence, 0, 0])
        time.sleep(0.005)
        expected = self.frame(0x2D)
        assert actual == expected and actual[0] == 15, ('RGB mismatch/fault', zone, node, actual.hex(), expected.hex())
        return [int.from_bytes(actual[n:n + 2], 'big') for n in (1, 3, 5)]

    def close(self):
        if self.initialized:
            self.call('InitializeHardware', [C.c_uint8, C.c_uint16, C.c_uint8, C.c_uint16], self.client, self.hw, 0, 19200)
        self.call('DisconnectClient', [C.c_uint8, C.c_uint16], self.client, self.hw)
        self.call('RemoveClient', [C.c_uint8], self.client)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cycles', type=int, default=3)
    parser.add_argument('--prefix', default='ClockVerify_20261117')
    args = parser.parse_args()
    assert 1 <= args.cycles <= 100 and Path(args.prefix).name == args.prefix
    report = {'started': datetime.now().isoformat(), 'chair': '0B', 'requested_per_mode': args.cycles, 'cycles': [], 'failures': [], 'physical_low_temperature_test': False}
    output = ROOT / 'validation' / (args.prefix + '_result.json')
    assert not output.exists(), 'Existing result must not be overwritten'
    timer_set = C.WinDLL('winmm').timeBeginPeriod(1) == 0
    p, lin, bp = Probe(), None, None
    go = reset = clear_bp = None

    def save():
        output.write_text(json.dumps(report, indent=2), encoding='utf-8')

    def byte(address):
        return (p.word(address & ~3) >> ((address & 3) * 8)) & 255

    def health(internal):
        srgb = SRGB.from_buffer_copy(p.read(*symbol('gSRGBControl')))
        osp = OSP.from_buffer_copy(p.read(*symbol('gOSPControl')))
        state = dict(time_ms=p.word(symbol('oAppTimer')[0]), gclk0=hex(p.word(0x40001C20)), gclk1=hex(p.word(0x40001C24)),
                     dpll_ctrlb=hex(p.word(0x40001024)), dpll_ctrla=byte(0x4000101C), dpll_status=byte(0x40001030),
                     xosc_ctrl=p.word(0x40001010) & 65535, osc_status=p.word(0x4000100C),
                     osc48m_ctrl=byte(0x40001014), osc48m_div=byte(0x40001015), cpudiv=byte(0x40000804),
                     wdt=byte(0x40002000), halted=bool(halted()), reset_cause=byte(0x40000C00),
                     bootprot=byte(0x804000) & 15, region_locks=p.word(0x41004020) & 65535,
                     nodes=list(osp.nodeCount), osp_errors=list(osp.communicationErrorCount),
                     osp_crc=list(osp.crcErrorCount), osp_manchester=list(osp.manchesterErrorCount),
                     srgb_errors=srgb.communicationErrorCount, srgb_valid=srgb.dynamicValid,
                     osp_valid=[sum(row) for row in osp.dynamicValid],
                     max_temperature=max(max(row) for row in osp.temperatureC),
                     power_fault=byte(symbol('gPowerData')[0] + 1),
                     lin_errors=p.word(symbol('g_dbg_lin_error_count')[0]), lin_drops=p.word(symbol('g_dbg_lin_drop_count')[0]))
        source = '0x10306' if internal else '0x10307'
        assert state['gclk0'] == state['gclk1'] == source, state
        assert state['osc48m_ctrl'] == 2 and state['osc48m_div'] == 0 and state['cpudiv'] == 1, state
        assert state['wdt'] & 2 and not state['halted'], state
        if not internal:
            assert state['dpll_ctrlb'] == '0x31010' and state['dpll_status'] & 2, state
        assert state['bootprot'] == 4 and state['region_locks'] == 0, state
        assert state['nodes'] == state['osp_valid'] == [24, 8] and state['srgb_valid'] == 1, state
        assert state['osp_errors'] == state['osp_crc'] == state['osp_manchester'] == [0, 0], state
        assert state['srgb_errors'] == state['power_fault'] == state['lin_errors'] == state['lin_drops'] == 0, state
        assert state['max_temperature'] < 80, state
        return state

    def wait_started():
        deadline = time.monotonic() + 8
        while time.monotonic() < deadline:
            if not halted() and byte(0x40002000) & 2:
                time.sleep(3)
                return
            time.sleep(0.02)
        raise TimeoutError('App startup')

    def boot(mode):
        nonlocal bp
        lin.off()
        time.sleep(0.03)
        assert reset() >= 0 and halted()
        hits = []
        if mode != 'normal':
            address, register, value, count, opcode = (0x1437E, 0, 0x7B06, 3, b'\x18\x80') if mode == 'xosc_timeout' else (0x143D6, 4, 2, 1, b'\x04\x70')
            assert byte(address) == opcode[0] and byte(address + 1) == opcode[1]
            assert not byte(0x40002000) & 2, 'WDT active during startup injection'
            bp = set_bp(address, 0xFFFFFF00)
            assert bp > 0
            for attempt in range(count):
                go()
                deadline = time.monotonic() + 5
                while not halted() and time.monotonic() < deadline:
                    time.sleep(0.002)
                assert halted() and read_reg(15) & ~1 == address, 'Breakpoint not reached'
                assert read_reg(register) == value, 'Unexpected source register'
                assert p.word(0x40001C20) == 0x10306
                assert write_reg(register, value & ~2) == 0
                assert step() == 0 and read_reg(15) & ~1 == address + 2
                actual = p.word(0x40001010) & 65535 if mode == 'xosc_timeout' else byte(0x4000101C)
                assert actual & 2 == 0
                hits.append(dict(attempt=attempt + 1, instruction=hex(address), original=value, injected=value & ~2, actual_control=actual))
            assert clear_bp(bp) == 0
            bp = None
        go()
        wait_started()
        return hits

    try:
        p.connect()
        p.api('SetSpeed', None, [C.c_uint32])(1000)
        expected = (APP / 'build/Debug/Exe/CJ_App.bin').read_bytes()
        assert p.read(0x10000, len(expected)) == expected, 'App mismatch; invalid symbols'
        assert byte(symbol('gConFig')[0] + 22) == 0x0B
        halted = p.api('IsHalted', C.c_int, [])
        reset = p.api('Reset', C.c_int, [])
        go = p.api('Go', None, [])
        step = p.api('Step', C.c_int, [])
        read_reg = p.api('ReadReg', C.c_uint32, [C.c_int])
        write_reg = p.api('WriteReg', C.c_int, [C.c_int, C.c_uint32])
        set_bp = p.api('SetBPEx', C.c_int, [C.c_uint32, C.c_uint32])
        clear_bp = p.api('ClrBPEx', C.c_int, [C.c_int])
        reg_name = p.api('GetRegisterName', C.c_char_p, [C.c_int])
        assert reg_name(0) == b'R0' and reg_name(4) == b'R4' and b'PC' in reg_name(15)
        report['initial'] = health(False)
        lin = Lin()
        report['initial_lin'] = lin.status()
        for mode in ('normal', 'xosc_timeout', 'dpll_timeout'):
            for cycle in range(args.cycles):
                assert not (ROOT / 'validation' / (args.prefix + '.STOP')).exists(), 'Operator stop'
                result = dict(mode=mode, cycle=cycle + 1, injection=boot(mode))
                result['before'] = health(mode != 'normal')
                result['diagnosis_before'] = lin.status()
                rgb = ([4, 0, 0], [0, 4, 0], [0, 0, 4])[cycle % 3]
                lin.frame(1, [2, 63, 0x17, *rgb, 0, 0])
                time.sleep(0.1)
                result['rgb'] = rgb
                result['readbacks'] = []
                for zone in range(6):
                    for node in range(24 if zone == 4 else 8 if zone == 5 else 1):
                        value = lin.read_rgb(zone, node)
                        assert value[cycle % 3] > 0 and all(value[n] == 0 for n in range(3) if n != cycle % 3), ('Color not applied', zone, node, value)
                        result['readbacks'].append(dict(zone=zone, node=node, rgb=value))
                lin.off()
                time.sleep(0.15)
                result['after'] = health(mode != 'normal')
                assert result['after']['time_ms'] > result['before']['time_ms'], 'Unexpected reset'
                result['diagnosis_after'] = lin.status()
                report['cycles'].append(result)
                save()
                print(json.dumps(dict(mode=mode, cycle=cycle + 1, passed=True, readbacks=len(result['readbacks']), clock=result['after']['gclk1'])), flush=True)
        report['passed'] = True
    except Exception as exc:
        report['failures'].append(dict(error=repr(exc), traceback=traceback.format_exc()))
        print('FAIL ' + repr(exc), flush=True)
    finally:
        try:
            if bp is not None:
                assert clear_bp(bp) == 0
                bp = None
            if reset is not None:
                assert reset() >= 0
                go()
                wait_started()
                if lin:
                    lin.off()
                    time.sleep(0.1)
                    report['final_lin'] = lin.status()
                report['final_external'] = health(False)
                before = json.loads((ROOT / 'validation/ClockVerify_20261117_before.json').read_text())
                report['preserved'] = {}
                for name, address, length in [('boot', 0, 0x10000), ('backup', 0x50000, 0x30000), ('reserved', 0x40000, 0x10000), ('data', 0x400000, 0x2000), ('userrow', 0x804000, 256)]:
                    digest = hashlib.sha256(p.read(address, length)).hexdigest()
                    report['preserved'][name] = digest == before[name + '_sha256']
                    assert report['preserved'][name], name + ' changed'
                report['final_app_matches'] = p.read(0x10000, len(expected)) == expected
                assert report['final_app_matches']
        except Exception as exc:
            report['cleanup_error'] = repr(exc)
            print('CLEANUP FAIL ' + repr(exc), flush=True)
        if lin:
            lin.close()
        p.close()
        if timer_set:
            C.WinDLL('winmm').timeEndPeriod(1)
        report['finished'] = datetime.now().isoformat()
        save()
        print(json.dumps(dict(passed=report.get('passed', False), cycles=len(report['cycles']), failures=report['failures'], cleanup_error=report.get('cleanup_error'), preserved=report.get('preserved'))), flush=True)
    if report['failures'] or report.get('cleanup_error'):
        raise SystemExit(1)


if __name__ == '__main__':
    main()
