// Host timing test of the current C bodies. GPIO/SPI/Read are stubbed.
// This checks software logic, not physical LIN/LED timing on the board.
'use strict';
const fs = require('node:fs');
const path = require('node:path');
const assert = require('node:assert/strict');
const root = path.resolve(__dirname, '..');
const read = file => fs.readFileSync(path.join(root, file), 'utf8');
const sources = {
    SRGB: read('applications/src/SRGBControl.c'),
    OSP: read('applications/src/OSPControl.c'),
    LIN: read('applications/src/lin_task.c'),
    main: read('main.c')
};

function body(source, name) {
    const match = new RegExp('\\b' + name + '\\s*\\([^;{}]*\\)\\s*\\{').exec(source);
    assert(match, 'Missing C definition: ' + name);
    const start = match.index + match[0].length;
    let depth = 1, end = start;
    for (; depth; end++) {
        assert(end < source.length);
        if (source[end] === '{') depth++;
        if (source[end] === '}') depth--;
    }
    return source.slice(start, end - 1);
}

function compile(c, parameters) {
    let js = c.replace(/\/\*[\s\S]*?\*\//g, '').replace(/\/\/[^\n]*/g, '');
    js = js.replace(/uint8_t\s+payload\[6U\];/g, 'let payload = Array(6).fill(0);');
    js = js.replace(/\(uint8_t\)gOSPControl\.(pwmRed|pwmGreen|pwmBlue)\[(index|port)\]/g, '(gOSPControl.$1[$2] & 0xFF)');
    js = js.replace(/->/g, '.').replace(/\bNULL\b/g, 'null');
    js = js.replace(/\(void\)/g, '');
    js = js.replace(/\((?:u?int(?:8|16|32|64)_t|SRGB_Zone_t|OSP_Port_t)\)/g, '');
    js = js.replace(/LedControlCommand_t\s+command;/g, 'let command = {};');
    js = js.replace(/\b(?:u?int(?:8|16|32|64)_t)\s+\*?\s*/g, 'let ');
    js = js.replace(/&(?=g\w+Control|command\b|color\b|previous\b|state\b)/g, '');
    js = js.replace(/sizeof\((?:command|color|state\.output)\)/g, '1');
    js = js.replace(/\b(0x[0-9A-Fa-f]+)U(?:L)?\b/g, '$1');
    js = js.replace(/\b(\d+)(?:U(?:L)?|LL)\b/g, '$1');
    // C integer division and unsigned timestamp subtraction.
    js = js.replace(/\(delta \* elapsed\) \/ duration/g, 'Math.trunc((delta * elapsed) / duration)');
    js = js.replace(/\(\(\(command\.effect1 \* 1000\) \+ \(command\.effect2 \* 125\)\) \/ \(command\.effect2 \* 250\)\)/g,
        'Math.floor(((command.effect1 * 1000) + (command.effect2 * 125)) / (command.effect2 * 250))');
    js = js.replace(/\(\(\(\(command\.effect1 \* 10\) \+ 125\) \/ 250\) \* 100\)/g,
        '(Math.floor(((command.effect1 * 10) + 125) / 250) * 100)');
    js = js.replace(/\(command\.color1 \* 2250\) \/ denominator/g, 'Math.trunc((command.color1 * 2250) / denominator)');
    js = js.replace(/\(command\.color2 \* 1000\) \/ denominator/g, 'Math.trunc((command.color2 * 1000) / denominator)');
    js = js.replace(/\(cieX \* yValue\) \/ cieY/g, 'Math.trunc((cieX * yValue) / cieY)');
    js = js.replace(/\(cieZ \* yValue\) \/ cieY/g, 'Math.trunc((cieZ * yValue) / cieY)');
    js = js.replace(/\(\(32406 \* xValue\) - \(15372 \* yValue\) - \(4986 \* zValue\)\) \/ 10000/g, 'Math.trunc(((32406 * xValue) - (15372 * yValue) - (4986 * zValue)) / 10000)');
    js = js.replace(/\(\(-9689 \* xValue\) \+ \(18758 \* yValue\) \+ \(415 \* zValue\)\) \/ 10000/g, 'Math.trunc(((-9689 * xValue) + (18758 * yValue) + (415 * zValue)) / 10000)');
    js = js.replace(/\(\(557 \* xValue\) - \(2040 \* yValue\) \+ \(10570 \* zValue\)\) \/ 10000/g, 'Math.trunc(((557 * xValue) - (2040 * yValue) + (10570 * zValue)) / 10000)');
    js = js.replace(/\((red|green|blue) \* command\.color3 \* 65535\) \/ \(maximum \* 255\)/g, 'Math.trunc(($1 * command.color3 * 65535) / (maximum * 255))');
    js = js.replace(/oAppTimer\.timeMs - g(SRGB|OSP)Control\.(startTimeMs|phaseTimeMs)\[(zone|port)\]/g, '((oAppTimer.timeMs - g$1Control.$2[$3]) >>> 0)');
    js = js.replace(/nowMs - g(SRGB|OSP)Control\.phaseTimeMs\[(zone|port)\]/g, '((nowMs - g$1Control.phaseTimeMs[$2]) >>> 0)');
    js = js.replace(/\(\((red|green|blue) \* OSP_PWM_MAX\) \/ 65535\)/g, 'Math.floor(($1 * OSP_PWM_MAX) / 65535)');
    const run = new Function('env', ...parameters, 'with (env) { ' + js + ' }');
    return (env, ...args) => run(env, ...args);
}

const compiled = {};
for (const driver of ['SRGB', 'OSP']) {
    compiled[driver + '_Interpolate'] = compile(body(sources[driver], driver + '_Interpolate'), ['start', 'target', 'elapsed', 'duration']);
    compiled[driver + 'Control_Set'] = compile(body(sources[driver], driver + 'Control_Set'), ['command']);
    compiled[driver + 'Control_Process'] = compile(body(sources[driver], driver + 'Control_Process'), []);
    assert(!/lastTick|tick1ms|updateNormal|\/ 10U/.test(body(sources[driver], driver + 'Control_Process')));
}
compiled.OSP_WriteColor = compile(body(sources.OSP, 'OSP_WriteColor'), ['port', 'red', 'green', 'blue']);
compiled.LINT_HandleLampControl = compile(body(sources.LIN, 'LINT_HandleLampControl'), ['frame']);
let loop = body(sources.main, 'while').replace(/#if[^\n]*\n[\s\S]*?#endif/g, '');
compiled.loop = compile(loop, []);
assert(!/updateNormal/.test(sources.main + sources.SRGB + sources.OSP));

function control(count) {
    const result = {};
    for (const field of ['startTimeMs','phaseTimeMs','startRed','startGreen','startBlue','targetRed','targetGreen','targetBlue','fadedRed','fadedGreen','fadedBlue','outputRed','outputGreen','outputBlue','durationMs','onThreshold','flickerPhase','pwmRed','pwmGreen','pwmBlue','mode','effect1','effect2','active','fadeEnabled','dirty']) {
        result[field] = Array(count).fill(0);
    }
    result.fadeInMs = Array(count).fill(500);
    result.fadeOutMs = Array(count).fill(500);
    return result;
}
function setup() {
    const env = {
        LIN_FRAME_DATA_LEN:8, SRGB_ZONE_COUNT:4, OSP_PORT_COUNT:2,
        gConFig:{Chair_Number:0x0A},
        SRGB_ZONE_FRT_LH:0, SRGB_ZONE_FRT_RH:1, SRGB_ZONE_HEADREST:2, SRGB_ZONE_BACK:3,
        OSP_PWM_MAX:32767, OSP_CURRENT_BIT:128, OSP_LOGO_CURRENT_LIMIT_MA:41,
        OSP_ADDR_BROADCAST:0, OSP_CMD_SET_PWM:79,
        OSP_PORT_LOGO:0, OSP_PORT_STROBE:1, WDT_CLEAR_MS:100,
        oAppTimer:{timeMs:0, tick1ms:0, tick10ms:0, tick100ms:30, tick100ms_1:60, tick100ms_2:90, tick2s:0, tickWdtMs:0},
        gVerification:{ActiveFlag:0},
        gSRGBControl:{...control(4), imageDirty:0, txImage:Array.from({length:2}, ()=>Array(16).fill(0))},
        gOSPControl:control(2),
        counts:{SRGB:0, OSP:0, update:[0,0], read:[0,0,0], power:0, srgbWrites:0}, packets:[], srgbImage:null
    };
    env.states = () => [[env.gSRGBControl,4],[env.gOSPControl,2]].flatMap(([part,count]) => Array.from({length:count}, (_,index) => ({
        startTimeMs:part.startTimeMs[index], phaseTimeMs:part.phaseTimeMs[index],
        durationMs:part.durationMs[index], fadeOutMs:part.fadeOutMs[index],
        onThreshold:part.onThreshold[index], flickerPhase:part.flickerPhase[index],
        mode:part.mode[index], active:part.active[index], effect1:part.effect1[index], effect2:part.effect2[index],
        targetRed:part.targetRed[index], targetGreen:part.targetGreen[index], targetBlue:part.targetBlue[index],
        faded:{red:part.fadedRed[index],green:part.fadedGreen[index],blue:part.fadedBlue[index]},
        output:{red:part.outputRed[index],green:part.outputGreen[index],blue:part.outputBlue[index]}
    })));
    for (const [name, run] of Object.entries(compiled)) env[name] = (...args) => run(env, ...args);
    for (const driver of ['SRGB', 'OSP']) {
        const run = env[driver + 'Control_Process'];
        env[driver + 'Control_Process'] = () => {
            env.counts[driver]++;
            if (env.oAppTimer.tick10ms >= 10) env.counts.update[driver==='SRGB' ? 0 : 1]++;
            return run();
        };
    }
    env.memset = object => {
        for (const key of Object.keys(object)) delete object[key];
    };
    env.LINT_Process = () => {};
    env.Reprogram_Process = () => {};
    env.Verification_Process = () => {};
    env.SRGB_WriteImage = () => { env.counts.srgbWrites++; env.srgbImage=env.gSRGBControl.txImage.map(row=>row.slice()); };
    env.OSP_Transfer = (port, address, command, payload, length, response, responseLength, timeoutUs) => {
        assert.equal(address,0); assert.equal(command,79); assert.equal(length,6);
        assert.equal(response,null); assert.equal(responseLength,0); assert.equal(timeoutUs,0);
        assert(payload.every(byte=>Number.isInteger(byte) && byte>=0 && byte<=255));
        env.packets.push({port, payload:payload.slice()});
        return 0;
    };
    env.memcmp = (left, right) => JSON.stringify(left) === JSON.stringify(right) ? 0 : 1;
    env.Power_Read = () => { env.counts.power++; };
    env.PDS_Process = () => {};
    env.SRGBControl_Read = () => { env.counts.read[0]++; };
    env.OSPControl_Read = port => { env.counts.read[port+1]++; };
    env.OSPControl_CheckLogoOpen = () => {};
    env.OSPControl_CheckStrobeOpen = () => {};
    env.step = ms => { for (const key of Object.keys(env.oAppTimer)) env.oAppTimer[key] = (env.oAppTimer[key]+ms) >>> 0; };
    env.sendCommand = (d6, hz, control=0x16) => env.LINT_HandleLampControl({data:[0x0F,0x3F,control,255,0,0,d6,hz]});
    return env;
}

function cieReference(mode, value1, value2, brightness) {
    let x=value1, y=value2;
    if (mode===2) {
        const denominator=(value1*6)-(value2*16)+3000;
        if (denominator<=0) return null;
        x=Math.trunc((value1*2250)/denominator);
        y=Math.trunc((value2*1000)/denominator);
    }
    if (value1>250 || value2>250 || x<0 || x>250 || y<=0 || y>250) return null;
    if (brightness===0) return [0,0,0];
    const z=Math.max(0,250-x-y);
    const yy=brightness;
    const xx=Math.trunc((x*yy)/y);
    const zz=Math.trunc((z*yy)/y);
    let red=Math.trunc(((32406*xx)-(15372*yy)-(4986*zz))/10000);
    let green=Math.trunc(((-9689*xx)+(18758*yy)+(415*zz))/10000);
    let blue=Math.trunc(((557*xx)-(2040*yy)+(10570*zz))/10000);
    red=Math.max(0,red); green=Math.max(0,green); blue=Math.max(0,blue);
    const maximum=Math.max(red,green,blue);
    if (maximum===0) return [0,0,0];
    return [red,green,blue].map(value=>Math.trunc((value*brightness*65535)/(maximum*255)));
}

let tests = 0;
function check(name, test) { test(); tests++; console.log('PASS ' + name); }
check('Main gates both Process calls at 1ms, resets first; normal/Fade updates at 10ms', () => {
    const env = setup(); env.sendCommand(0,0,0x17);
    for (let tick=1; tick<=1000; tick++) {
        env.step(1); env.loop();
        const count = env.counts.SRGB;
        for (let repeat=0; repeat<20; repeat++) env.loop();
        assert.equal(env.counts.SRGB, count);
        assert.equal(env.oAppTimer.tick1ms,0);
        assert(env.states().every(s=>s.output.red === (tick<10 ? 0 : 65535)), JSON.stringify({tick, states:env.states()}));
    }
    assert.equal(env.counts.SRGB,1000); assert.equal(env.counts.OSP,1000);
    assert.deepEqual(env.counts.update,[100,100]);
    assert.equal(env.counts.srgbWrites,1);
    assert.equal(env.counts.power,100); assert.deepEqual(env.counts.read,[10,10,10]);
});
check('Normal RGB command changes at the Main 10ms update, not every 1ms', () => {
    const env=setup(); env.sendCommand(0,0,0x17);
    for (let tick=1; tick<=15; tick++) { env.step(1); env.loop(); }
    env.LINT_HandleLampControl({data:[0x0F,0x3F,0x17,0,255,0,0,0]});
    for (let tick=16; tick<=20; tick++) {
        env.step(1); env.loop();
        assert(env.states().every(s=>s.output.red===(tick<20 ? 65535 : 0) && s.output.green===(tick<20 ? 0 : 65535)));
    }
});
check('Unsupported Mode and Color frames are ignored', () => {
    const env=setup();
    env.LINT_HandleLampControl({data:[0x0F,0x3F,0x17,255,0,0,0,0]});
    const before=JSON.stringify([env.gSRGBControl,env.gOSPControl]);
    env.LINT_HandleLampControl({data:[0x0F,0x3F,0x15,0,255,0,0,0]});
    assert.equal(JSON.stringify([env.gSRGBControl,env.gOSPControl]),before);
    env.LINT_HandleLampControl({data:[0x0F,0x3F,0xD7,0,0,255,0,0]});
    assert.equal(JSON.stringify([env.gSRGBControl,env.gOSPControl]),before);
});
check('LIN fields, Chair/LED masks, RGB and CIE limits are applied before LED control', () => {
    for (let chair=0; chair<4; chair++) {
        const env=setup(); env.gConFig.Chair_Number=0x0A+chair;
        env.LINT_HandleLampControl({data:[1<<chair,0x3F,0x17,255,128,0,255,255]});
        assert(env.states().every(state=>state.targetRed===65535 && state.targetGreen===32896 && state.targetBlue===0));
        assert(env.states().every(state=>state.effect1===250 && state.effect2===250));
        const before=JSON.stringify([env.gSRGBControl,env.gOSPControl]);
        env.LINT_HandleLampControl({data:[1<<((chair+1)%4),0x3F,0x17,0,255,0,0,0]});
        assert.equal(JSON.stringify([env.gSRGBControl,env.gOSPControl]),before);
        env.LINT_HandleLampControl({data:[1<<chair,0xC0,0x17,0,255,0,0,0]});
        assert.equal(JSON.stringify([env.gSRGBControl,env.gOSPControl]),before);
    }
    for (const [control,value1,value2,brightness,mode] of [[0x57,80,90,255,1],[0x97,50,120,200,2]]) {
        const env=setup(); const expected=cieReference(mode,value1,value2,brightness);
        env.LINT_HandleLampControl({data:[0x0F,0x3F,control,value1,value2,brightness,0,0]});
        assert(env.states().every(state=>state.targetRed===expected[0] && state.targetGreen===expected[1] && state.targetBlue===expected[2]));
        const before=JSON.stringify([env.gSRGBControl,env.gOSPControl]);
        env.LINT_HandleLampControl({data:[0x0F,0x3F,control,251,value2,brightness,0,0]});
        assert.equal(JSON.stringify([env.gSRGBControl,env.gOSPControl]),before);
    }
    const env=setup();
    env.LINT_HandleLampControl({data:[0x0F,0x3F,0x57,80,90,255,0,0]});
    const before=JSON.stringify([env.gSRGBControl,env.gOSPControl]);
    env.LINT_HandleLampControl({data:[0x0F,0x3F,0x57,80,0,255,0,0]});
    assert.equal(JSON.stringify([env.gSRGBControl,env.gOSPControl]),before);
    env.gConFig.Chair_Number=0x09;
    env.LINT_HandleLampControl({data:[0x0F,0x3F,0x17,255,0,0,0,0]});
    assert.equal(JSON.stringify([env.gSRGBControl,env.gOSPControl]),before);
});
check('CIE zero brightness or zero coordinate pair turns all outputs off and valid commands recover', () => {
    for (const [control,value1,value2,brightness] of [[0x57,80,90,255],[0x97,50,120,200]]) {
        const env=setup();
        const light = () => {
            env.LINT_HandleLampControl({data:[0x0F,0x3F,control,value1,value2,brightness,0,0]});
            assert(env.states().every(state=>state.active===1));
            assert(env.states().every(state=>state.targetRed!==0 || state.targetGreen!==0 || state.targetBlue!==0));
        };
        const assertOff = () => {
            assert(env.states().every(state=>state.active===0));
            assert(env.states().every(state=>state.targetRed===0 && state.targetGreen===0 && state.targetBlue===0));
        };

        light();
        env.LINT_HandleLampControl({data:[0x0F,0x3F,control,0,0,0,0,0]});
        assertOff();

        light();
        env.LINT_HandleLampControl({data:[0x0F,0x3F,control-1,0,0,brightness,0,100]});
        assertOff();
        assert(env.states().every(state=>state.mode===6));

        light();
        env.LINT_HandleLampControl({data:[0x0F,0x3F,control,value1,value2,0,0,0]});
        assertOff();

        light();
    }
});
check('Mixed Normal/Flicker commits one SRGB image with both parts; OSP PWM payload matches', () => {
    const env=setup();
    env.LINT_HandleLampControl({data:[0x0F,0x01,0x17,0,255,0,0,0]});
    env.LINT_HandleLampControl({data:[0x0F,0x32,0x16,255,0,0,25,100]});
    for (let tick=1; tick<=10; tick++) { env.step(1); env.loop(); }
    assert.equal(env.counts.srgbWrites,0);
    assert.equal(env.packets.length,2);
    assert.deepEqual(env.packets.at(-1).payload,[128,0,128,0,128,0]);
    env.step(1); env.loop();
    assert.equal(env.counts.srgbWrites,1);
    assert.deepEqual(env.srgbImage[1].slice(0,12),[0,0,0,0,65535,65535,0,0,0,0,0,0]);
    for (let tick=12; tick<=20; tick++) { env.step(1); env.loop(); }
    assert.equal(env.counts.srgbWrites,2);
    assert.deepEqual(env.srgbImage[1].slice(0,12),[0,0,65535,65535,65535,65535,0,0,0,0,0,0]);
    assert.deepEqual(env.packets.at(-1).payload,[255,255,128,0,128,0]);
    assert.deepEqual(env.packets.slice(-2).map(packet=>packet.payload),[[232,244,128,0,128,0],[255,255,128,0,128,0]]);
});
check('LIN Duration 0..250 and Hz 1..100: all six LED parts, all 10% steps', () => {
    for (let hz=1; hz<=100; hz++) for (let duty=0; duty<=100; duty+=10) {
        const env = setup(); const effect1=duty*2.5;
        const expectedOnThreshold=(effect1>=250) ? 1000 : Math.floor(((effect1*1000)+(hz*125))/(hz*250))*hz;
        env.sendCommand(effect1,hz);
        assert(env.states().every(s=>s.onThreshold===expectedOnThreshold && s.effect2===hz));
        for (let tick=1; tick<=1100; tick++) {
            env.step(1); env.loop();
            if (tick>=100) {
                const on = ((tick*hz)%1000)<expectedOnThreshold;
                assert(env.states().every(s=>s.output.red === (on ? 65535 : 0)));
            }
        }
    }
});
check('100Hz: 10%=1ms ON/9ms OFF; 50%=5/5; 90%=9/1', () => {
    for (const duty of [10,50,90]) {
        const env=setup(); env.sendCommand(duty*2.5,100);
        let on=0;
        for (let tick=1; tick<=29; tick++) {
            env.step(1); env.loop();
            if (tick>=20 && env.states()[0].output.red!==0) on++;
        }
        assert.equal(on,duty/10);
    }
});
check('Zero Hz is OFF; raw 255 caps to 250, Hz above 100 caps to 100', () => {
    const env=setup(); env.sendCommand(255,255);
    assert(env.states().every(s=>s.effect1===250 && s.effect2===100 && s.onThreshold===1000));
    env.sendCommand(250,0);
    for (let tick=0; tick<20; tick++) { env.step(1); env.loop(); }
    assert(env.states().every(s=>s.output.red===0));
});
check('Fade+Flicker: initial 500ms; remembered Fade In 300ms/Out 800ms', () => {
    const mixed=setup(); mixed.sendCommand(125,10,0x36);
    for (let tick=1; tick<=250; tick++) {
        mixed.step(1); mixed.loop();
        if (tick===200) assert(mixed.states().every(s=>s.output.red===24903));
    }
    assert(mixed.states().every(s=>s.faded.red===32767 && s.output.red===0));

    const env=setup(); env.sendCommand(250,10,0x36);
    for (let tick=1; tick<=250; tick++) { env.step(1); env.loop(); }
    assert(env.states().every(s=>s.durationMs===500 && s.faded.red===32767 && s.output.red===31456));
    env.step(1); env.loop();
    assert(env.states().every(s=>s.output.red===32767));
    env.sendCommand(3,8,0x37); env.sendCommand(125,10,0x36);
    assert(env.states().every(s=>s.durationMs===300 && s.fadeOutMs===800));
    env.sendCommand(125,10,0x26);
    assert(env.states().every(s=>s.durationMs===800));
    for (let tick=1; tick<=810; tick++) { env.step(1); env.loop(); }
    assert(env.states().every(s=>s.faded.red===0 && s.output.red===0));
});
check('Read delay does not turn elapsed milliseconds into Process-call counts', () => {
    const env=setup(); env.sendCommand(25,100);
    for (let tick=1; tick<=20; tick++) { env.step(1); env.loop(); }
    const start=env.states()[0].startTimeMs;
    env.step(37); env.loop();
    assert(env.states().every(s=>s.flickerPhase===700 && s.output.red===0 && s.startTimeMs===start));
    env.sendCommand(25,100);
    assert(env.states().every(s=>s.startTimeMs===start), 'Cyclic command must not restart');
    env.sendCommand(25,100,0x1E);
    assert(env.states().every(s=>s.startTimeMs===57), 'Sync must restart');
});
check('Timestamp wrap keeps Flicker phase and Fade elapsed time valid', () => {
    const env=setup(); env.oAppTimer.timeMs=0xFFFFFFFB; env.sendCommand(125,100,0x36);
    for (let tick=1; tick<=20; tick++) { env.step(1); env.loop(); }
    assert(env.states().every(s=>s.flickerPhase===0 && s.faded.red===2621));
});
console.log('RESULT ' + tests + ' PASS, 0 FAIL. Host logic only; blocking Read still delays physical output edges.');
