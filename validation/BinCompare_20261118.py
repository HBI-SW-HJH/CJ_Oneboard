"""Real four-board LIN comparison, delivery BIN then current BIN on each board.
Only test artifacts are written locally. No GUI, Boot download, SWD, or source edits.
"""
import ctypes as C
from datetime import datetime
import gzip
import hashlib
import json
from pathlib import Path
import re
import statistics
import struct
import subprocess
import sys
import time
import traceback
import zlib

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parent.parent
DELIVERY = ROOT.parent / '납품/CJ_Git_20261111'
sys.path.insert(0, str(ROOT.parent / 'old/CJ_Onboard_20261027/validation'))
import FullFourChairTest as transport
from RuntimeCrcLinReadOnly import ReadOnly
transport.ROOT = ROOT
require = transport.require
COUNTS = (1, 1, 1, 1, 24, 8)
PREFIX = 'BinCompare_20261118'
OUT = ROOT / 'validation'
IMAGES = {
    'delivery': (DELIVERY / 'CJ_OneBoard_App/build/Debug/Exe/CJ_App.bin', '534cac48edeb1978f82171d25da766f70826c45ce93fbe185a045244b8630c1a'),
    'current': (ROOT / 'CJ_OneBoard_App/build/Debug/Exe/CJ_App.bin', '84a4ff1378d48131e8240366ec69fc8d1bd94aa7fdeafefae86fbeac54c0b4e2')}


def save(path, data):
    path.write_text(json.dumps(data, ensure_ascii=False, indent=2), encoding='utf-8')


def source_hashes(root):
    return {str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest() for directory in ('CJ_OneBoard_App', 'CJ_OneBoard_Boot') for p in (root / directory).rglob('*') if p.is_file() and p.suffix.lower() in ('.c', '.h', '.icf', '.ewp', '.mac', '.s') and not any(n in p.parts for n in ('build', 'settings', 'tests'))}


class Test(transport.Test):
    def __init__(self, tag):
        self.initialized = False
        self.frames = gzip.open(OUT / (tag + '_frames.jsonl.gz'), 'xt', encoding='utf-8')
        super().__init__(tag)
        self.cap = 26

    def connect(self):
        ReadOnly.connect(self)

    def log(self, kind, **data):
        if kind == 'FRAME':
            self.frames.write(json.dumps(dict(time=datetime.now().isoformat(), **data)) + '\n')
        else:
            super().log(kind, **data)

    def close(self):
        # Leave master mode configured for the next console client; never keep a schedule.
        super().close()
        self.frames.close()

    def runtime(self, board):
        nad, mode = self.select(board['chair'], board['init'])
        require(mode == 0 and nad == board['chair'] - 9, 'Expected running target App')
        data = self.uds(nad, bytes.fromhex('22f210'))
        require(len(data) == 38 and data[3] == 0, 'Runtime App response')
        require(data[12:28] == bytes(16), 'Programming/HardFault flags or counters nonzero')
        require(self.info(board['chair']).hex() == board['raw'], 'Product identity changed')
        return dict(raw=data.hex(), mode=data[3], restores=int.from_bytes(data[29:33], 'big'), valid=data[28], bits=data[37])

    def rgb_exact(self, chair, zone, node, expected=None):
        status, actual = self.rgb(chair, zone, node)
        if expected is None:
            _, expected = self.rgb(chair, zone, node, 10)
        row = dict(zone=zone, node=node + 1, actual=actual, expected=expected, status=status)
        require(status & 0xF1 == 1 and actual == expected, 'RGB/diagnostic mismatch: ' + json.dumps(row))
        return row


def download(test, board, image, progress):
    chair = board['chair']
    test.off(chair)
    test.verify_command(chair, 4)
    nad, mode = test.select(chair, board['init'])
    require(mode == 0, 'Download must start from verified App')
    progress.update(phase='enter_boot', started=datetime.now().isoformat())
    persist()
    require(test.uds(nad, [0x10, 2])[:2] == bytes.fromhex('5002'), 'App programming session')
    time.sleep(0.7)
    nad, mode = test.select(chair, board['init'])
    require(mode == 1 and test.uds(nad, [0x10, 2])[:2] == bytes.fromhex('5002'), 'Boot programming session')
    ready = test.uds(nad, bytes.fromhex('340044') + (0x10000).to_bytes(4, 'big') + len(image).to_bytes(4, 'big'), 25)
    require(ready == bytes.fromhex('74200082'), 'Download 128-byte block negotiation')
    progress['phase'] = 'transfer'
    persist()
    seq = 1
    for pos in range(0, len(image), 128):
        block = image[pos:pos + 128]
        require(test.uds(nad, bytes([0x36, seq]) + block) == bytes([0x76, seq]), 'Transfer acknowledgement')
        seq = (seq + 1) & 255
        progress['bytes_acknowledged'] = pos + len(block)
        if (pos // 128 + 1) % 58 == 0:
            test.log('UPDATE_PROGRESS', chair=f'{chair:02X}', image=progress['image'], bytes=progress['bytes_acknowledged'], length=len(image))
            persist()
    require(test.uds(nad, b'\x37' + zlib.crc32(image).to_bytes(4, 'big'), 3) == b'\x77', 'Device rejected whole-image CRC')
    progress.update(phase='restart', crc_accepted=True)
    persist()
    require(test.uds(nad, [0x11, 1]) == bytes.fromhex('5101'), 'Reset acknowledgement')
    time.sleep(3)
    progress['runtime'] = test.runtime(board)
    progress.update(phase='complete', completed=datetime.now().isoformat(), passed=True)
    test.log('UPDATE_PASS', chair=f'{chair:02X}', image=progress['image'], crc=f'{zlib.crc32(image):08X}')
    persist()


def product_write(test, board):
    original = bytes.fromhex(board['raw'])
    temporary = bytearray(original)
    temporary[9] = ord('X') if original[9] != ord('X') else ord('Y')
    try:
        require(test.uds(0x7F, bytes.fromhex('2ef205') + temporary) == bytes.fromhex('6ef205'), 'Write ACK')
        actual = test.uds(0x7F, bytes.fromhex('3103f301') + original[25:29])[4:]
        require(actual == temporary and test.info(board['chair']) == temporary, 'Written serial readback')
        return dict(field='Serial final character', temporary=actual.hex(), passed=True)
    finally:
        if not test.competing:
            require(test.uds(0x7F, bytes.fromhex('2ef205') + original) == bytes.fromhex('6ef205'), 'Restore ACK')
            require(test.info(board['chair']) == original, 'Product restoration failed')
            test.log('PRODUCT_RESTORED', chair=f"{board['chair']:02X}")


def controls(test, chair):
    rows = []
    test.verify_command(chair, 4)
    test.off(chair)
    colors = [(13, 0, 0), (0, 13, 0), (0, 0, 13), (13, 13, 13), (5, 9, 13)]
    for zone in range(6):
        for color in colors:
            test.lamp(chair, 1 << zone, color)
            time.sleep(0.07)
            expected = [v * 257 if zone < 4 else v * 32767 // 255 for v in color]
            for node in range(COUNTS[zone]):
                rows.append(dict(command=list(color), **test.rgb_exact(chair, zone, node, expected)))
            for other in range(6):
                if other != zone:
                    test.rgb_exact(chair, other, 0, [0, 0, 0])
        test.off(chair)
        time.sleep(0.04)
        test.log('CONTROL_ZONE_PASS', chair=f'{chair:02X}', zone=zone, colors=len(colors), nodes=COUNTS[zone])
    cie = []
    for mode, values in [(1, (80, 80, 13)), (1, (0, 0, 13)), (1, (80, 80, 0)), (2, (50, 100, 13))]:
        test.lamp(chair, 63, values, (mode << 6) | 0x17)
        time.sleep(0.08)
        for zone in range(6):
            row = test.rgb_exact(chair, zone, 0)
            if values[2] == 0 or values[:2] == (0, 0):
                require(row['actual'] == [0, 0, 0], 'Zero-coordinate/brightness OFF')
            cie.append(dict(mode=mode, command=values, **row))
    test.off(chair)
    return dict(passed=True, rgb_rows=rows, cie_rows=cie)


def effects(test, chair):
    result = dict(fade=[], flicker=[])
    test.verify_command(chair, 4)
    test.off(chair)
    time.sleep(0.05)
    for direction, control in [('in', 0x37), ('out', 0x27)]:
        test.lamp(chair, 63, (13, 13, 13), control, 30, 30)
        started = time.monotonic()
        samples = [[] for _ in range(6)]
        while time.monotonic() - started < 3.3:
            for zone in range(6):
                status, rgb = test.rgb(chair, zone, 0)
                require(status & 0xF1 == 1, 'Fade diagnostic status')
                require(rgb[0] == rgb[1] == rgb[2], 'White fade channels disagree')
                samples[zone].append(dict(ms=round((time.monotonic()-started)*1000, 3), rgb=rgb))
            time.sleep(0.03)
        for zone in range(6):
            values = [row['rgb'][0] for row in samples[zone]]
            require(len(set(values)) >= 8, 'Insufficient fade progression')
            require(all((b >= a if direction == 'in' else b <= a) for a, b in zip(values, values[1:])), 'Fade direction reversal')
            value = (13 * 257 if zone < 4 else 13 * 32767 // 255) if direction == 'in' else 0
            for node in range(COUNTS[zone]):
                test.rgb_exact(chair, zone, node, [value] * 3)
        result['fade'].append(dict(direction=direction, requested_ms=3000, samples=samples, passed=True))
        test.log('FADE_PASS', chair=f'{chair:02X}', direction=direction)
    for zone in range(6):
        for hz in (10, 20, 30):
            for duty in (10, 50, 90):
                test.off(chair)
                test.lamp(chair, 1 << zone, (13, 13, 13), 0x16, duty * 250 // 100, hz)
                time.sleep(0.04)
                test.verify_command(chair, 2, zone, capture=600)
                time.sleep(0.62)
                raw = test.verification(chair, lambda data: data[:2] == bytes([0x82, zone]), 1)
                period = 1000.0 / hz
                target = [period, period, period*duty/100, period*duty/100, period*(100-duty)/100, period*(100-duty)/100]
                actual = list(raw[2:8])
                passed = all(0 < a < 255 and abs(a-b) <= 2 for a,b in zip(actual,target))
                item = dict(zone=zone, hz=hz, duty=duty, actual=actual, expected=target, passed=passed)
                result['flicker'].append(item)
                require(passed, 'Flicker outside 2ms tolerance: '+json.dumps(item))
        test.log('FLICKER_ZONE_PASS', chair=f'{chair:02X}', zone=zone, hz=[10,20,30], duties=[10,50,90])
    test.off(chair)
    test.verify_command(chair, 4)
    result['passed'] = True
    return result


def xcp(test, board, image_name):
    chair=board['chair']
    root=DELIVERY if image_name=='delivery' else ROOT
    mapping=(root/'CJ_OneBoard_App/build/Debug/List/CJ_OneBoard_APP.map').read_text(encoding='utf-8',errors='replace')
    addresses={name:int(re.search(r'^'+name+r'\s+0x([\da-fA-F\']+)',mapping,re.M)[1].replace("'",''),16) for name in ('gOSPControl','gSRGBControl','gPowerData')}
    require(addresses=={'gOSPControl':0x2000003C,'gSRGBControl':0x200017FC,'gPowerData':0x2000209A},'Review XCP layout')
    test.off(chair)
    require(test.xcp_exchange(chair,[255]+[0]*7)==bytes.fromhex('ff01000808010100'),'XCP CONNECT')
    try:
        expected=bytes([24,8])
        for length in (1,2):require(test.xcp_read(chair,addresses['gOSPControl']+1752,length)==expected[:length],'XCP node-count value and read length')
        test.lamp(chair,63,(5,9,13))
        time.sleep(0.08)
        for offset,value in ((76,5),(80,9),(84,13)):
            expected=struct.pack('<HH',value*257,value*257)
            for length in (1,2,4):require(test.xcp_read(chair,addresses['gOSPControl']+offset,length)==expected[:length],'XCP OSP output RAM value')
        for offset,value in ((108,5),(112,9),(116,13)):
            require(test.xcp_read(chair,addresses['gOSPControl']+offset,4)==struct.pack('<HH',value*32767//255,value*32767//255),'XCP OSP PWM RAM value')
        for offset,value in ((108,5),(116,9),(124,13)):
            require(test.xcp_read(chair,addresses['gSRGBControl']+offset,4)==struct.pack('<HH',value*257,value*257),'XCP SRGB output RAM value')
        voltage=int.from_bytes(test.xcp_read(chair,addresses['gPowerData'],2),'little')
        require(voltage==1,'XCP digital power-good flags')
        test.off(chair)
        for address,length in ((0x10000,1),(0x40000000,1),(0x20010000,1),(0x20000020,1)):
            require(test.xcp_exchange(chair,bytes([244,length,0,0])+address.to_bytes(4,'little'))[:2]==b'\xfe\x24','XCP RAM boundary rejection')
        for length in (0,5,7,8):
            require(test.xcp_exchange(chair,bytes([244,length,0,0])+addresses['gOSPControl'].to_bytes(4,'little'))[:2]==b'\xfe\x22','XCP invalid size rejection')
        require(test.xcp_read(chair,addresses['gOSPControl']+1704,3)==bytes([1,1,1]),'XCP temperature valid flags')
        samples=[];start=time.perf_counter();deadline=start
        for index in range(1000):
            time.sleep(max(0,deadline-time.perf_counter()))
            begin=time.perf_counter()
            values=list(struct.unpack('<hhh',test.xcp_read(chair,addresses['gOSPControl']+120,4)+test.xcp_read(chair,addresses['gOSPControl']+124,2)))
            require(all(-40<=v<=150 for v in values),'XCP temperature data invalid')
            samples.append(dict(seconds=begin-start,values=values,read_ms=(time.perf_counter()-begin)*1000))
            deadline=max(deadline+0.02,time.perf_counter())
        intervals=[(b['seconds']-a['seconds'])*1000 for a,b in zip(samples,samples[1:])]
        result=dict(passed=True,samples=samples,requested_ms=20,mean_ms=statistics.mean(intervals),min_ms=min(intervals),max_ms=max(intervals),addresses=addresses,max_payload=4,transactions_per_sample=2,power_raw=voltage)
        test.log('XCP_PASS',chair=f'{chair:02X}',image=image_name,samples=len(samples),mean_ms=result['mean_ms'],max_ms=result['max_ms'])
        return result
    finally:test.xcp_exchange(chair,[254]+[0]*7)


RESULT={}
def persist():save(OUT/(PREFIX+'_result.json'),RESULT)


def main():
    global RESULT
    resume='--resume' in sys.argv
    exclude_0a='--exclude-0a' in sys.argv
    require(resume or not (OUT/(PREFIX+'_result.json')).exists(),'Existing campaign; do not overwrite')
    images={}
    for name,(path,digest) in IMAGES.items():
        image=path.read_bytes();require(hashlib.sha256(image).hexdigest()==digest,'BIN changed: '+name)
        stack,entry=struct.unpack_from('<II',image)
        require(0x20000000<stack<=0x20010000 and stack%8==0 and entry&1 and 0x10000<=entry<0x10000+len(image),'App vector validation')
        images[name]=image
    baseline=json.loads((OUT/'BinCompare_20261118_selected_131826.json').read_text(encoding='utf-8'))
    require(baseline['passed'],'Initial selected-board probe failed')
    auto=json.loads((OUT/(PREFIX+'_auto_manifest.json')).read_text(encoding='utf-8'))
    hashes={name:source_hashes(root) for name,root in [('delivery',DELIVERY),('current',ROOT)]}
    if resume:
        RESULT=json.loads((OUT/(PREFIX+'_result.json')).read_text(encoding='utf-8'))
        setup_error=RESULT.get('error') in ('XCP read error: fe22ffffffffffff','XCP OSP output RAM value')
        if RESULT.get('error','').startswith('PC automatic verification failed: '):
            setup_error='Unexpected baud rate' in Path(RESULT['error'].split(': ',1)[1]).read_text(encoding='utf-8')
        user_exclusion=exclude_0a and RESULT.get('error')=='Final diagnostic fault' and RESULT['cases'][-1]['chair']=='0A'
        require(setup_error or user_exclusion,'Only resume confirmed host setup errors or the user-excluded 0A fault')
        require(RESULT['source_hashes']==hashes,'Source files changed before resume')
        if user_exclusion:
            RESULT.setdefault('excluded_faults',[]).append(dict(time=datetime.now().isoformat(),chair='0A',error=RESULT.pop('error'),traceback=RESULT.pop('traceback'),reason='User confirmed abnormal Logo LED on 0A and requested continuation on other IDs. Preserve the 0A results and do not retest or download it.'))
        else:
            RESULT.setdefault('test_setup_corrections',[]).append(dict(time=datetime.now().isoformat(),error=RESULT.pop('error'),traceback=RESULT.pop('traceback'),reason='Corrected host assertions to source/map: three structures, maximum 4 bytes, OSP output is 16-bit and PWM is 15-bit. Firmware unchanged.'))
    else:
        RESULT=dict(started=datetime.now().isoformat(),completed=False,baseline=baseline,images={name:dict(path=str(IMAGES[name][0]),sha256=IMAGES[name][1],length=len(data),crc32=f'{zlib.crc32(data):08X}') for name,data in images.items()},cases=[],updates=[],gui=False,optical_measurement=False,brightness_max_raw=26,flicker_tolerance_ms=2,source_hashes=hashes)
    save(OUT/(PREFIX+'_baseline.json'),baseline)
    chairs=[11,12,13] if exclude_0a else [10,11,12,13]
    RESULT['planned_chairs']=[f'{chair:02X}' for chair in chairs]
    RESULT['excluded_chairs']=['0A'] if exclude_0a else []
    persist()
    test=None;active=None
    timer=C.WinDLL('winmm').timeBeginPeriod(1)==0
    try:
        for chair in chairs:
            board=baseline['boards'][f'{chair:02X}']['product']
            for name in ('delivery','current'):
                case=next((c for c in RESULT['cases'] if c['chair']==f'{chair:02X}' and c['image']==name),None)
                if case and case.get('passed'):continue
                tag=f'{PREFIX}_{chair:02X}_{name}'+('_resume'+str(len(RESULT.get('test_setup_corrections',[]))) if case else '')
                if not case:
                    case=dict(chair=f'{chair:02X}',image=name,phase='preflight',started=datetime.now().isoformat())
                    RESULT['cases'].append(case)
                persist()
                test=Test(tag);test.connect();active=chair
                for peer in chairs:test.off(peer)
                test.runtime(board)
                require(not any(test.status(chair)),'Initial diagnostic fault')
                update=next((u for u in RESULT['updates'] if u['chair']==f'{chair:02X}' and u['image']==name),None)
                if not update:
                    update=dict(chair=f'{chair:02X}',image=name,passed=False)
                    RESULT['updates'].append(update);persist()
                    download(test,board,images[name],update)
                require(update.get('passed'),'Do not silently retry failed download')
                if 'controls' not in case:
                    case['phase']='controls';persist()
                    case['controls']=controls(test,chair);persist()
                if 'effects' not in case:
                    case['phase']='effects';persist()
                    case['effects']=effects(test,chair);persist()
                if 'uds_write' not in case:
                    case['phase']='uds_write';persist()
                    case['uds_write']=product_write(test,board);persist()
                if 'xcp' not in case:
                    case['phase']='xcp';persist()
                    case['xcp']=xcp(test,board,name);persist()
                require(not any(test.status(chair)),'Diagnostic fault after functional tests')
                case['runtime_before_auto']=test.runtime(board)
                test.off(chair);test.close();test=None
                case['phase']='pc_automatic';persist()
                print(json.dumps(dict(kind='AUTO_BEGIN',chair=f'{chair:02X}',image=name)),flush=True)
                auto_tag=tag+'_auto'
                with (OUT/(auto_tag+'_console.txt')).open('x',encoding='utf-8') as log:
                    proc=subprocess.run([auto['executable'],str(ROOT),auto_tag,f'{chair:02X}',str(OUT/(PREFIX+'_baseline.json'))],stdout=log,stderr=subprocess.STDOUT)
                report_path=OUT/(auto_tag+'_report.json')
                case['automatic_report']=str(report_path)
                if report_path.exists():
                    report=json.loads(report_path.read_text(encoding='utf-8-sig'))
                    case['automatic']=dict(passed=report['passed'],rows=len(report['rows']),detail_rows=len(report['detailRows']))
                persist()
                require(proc.returncode==0 and case.get('automatic',{}).get('passed'),'PC automatic verification failed: '+str(OUT/(auto_tag+'_console.txt')))
                print(json.dumps(dict(kind='AUTO_PASS',chair=f'{chair:02X}',image=name,**case['automatic'])),flush=True)
                test=Test(tag+'_final');test.connect()
                test.off(chair);test.verify_command(chair,4)
                case['final_diagnostics']=test.status(chair)
                require(not any(case['final_diagnostics']),'Final diagnostic fault')
                case['final_runtime']=test.runtime(board)
                case.update(phase='complete',passed=True,completed=datetime.now().isoformat());persist()
                test.close();test=None;active=None
        RESULT['completed']=True
        RESULT['finished']=datetime.now().isoformat()
        return 0
    except Exception as ex:
        RESULT['error']=str(ex)
        RESULT['traceback']=traceback.format_exc()
        print(json.dumps(dict(kind='STOP_ON_FAILURE',error=str(ex))),flush=True)
        return 1
    finally:
        if test is not None:
            if test.connected and not test.competing and active is not None:
                try:test.off(active);test.verify_command(active,4)
                except Exception as ex:RESULT['cleanup_error']=str(ex)
            test.close()
        RESULT['sources_unchanged']={name:source_hashes(root)==hashes[name] for name,root in [('delivery',DELIVERY),('current',ROOT)]}
        persist()
        if timer:C.WinDLL('winmm').timeEndPeriod(1)
        print('REPORT='+str(OUT/(PREFIX+'_result.json')),flush=True)


if __name__=='__main__':raise SystemExit(main())
