// 50Hz Flicker timing model with the current staggered 100ms diagnostic reads.
'use strict';

const fs = require('node:fs');
const path = require('node:path');
const assert = require('node:assert/strict');

const root = path.resolve(__dirname, '..');
const main = fs.readFileSync(path.join(root, 'main.c'), 'utf8');
const osp = fs.readFileSync(path.join(root, 'applications/src/OSPControl.c'), 'utf8');
const lin = fs.readFileSync(path.join(root, 'applications/src/lin_task.c'), 'utf8');
const durationMs = 10000;

assert.match(main, /tick100ms_1 >= 100U/);
assert.match(main, /tick100ms_2 >= 100U/);
assert.match(osp, /for \(item = 0U; item < 3U; item\+\+\)/);
assert.match(lin, /command\.effect2 > 100U/);

function ospReadTimeUs(address)
{
    // Three reads: request TX/gap + configured response wait + decode margin.
    return (3 * ((address * 16) + 75)) + 200;
}

function buildBlocks()
{
    const blocks = [];
    let logo = 1;
    let strobe = 1;

    for (let timeMs = 10; timeMs < durationMs; timeMs += 100)
    {
        blocks.push({startUs:timeMs * 1000, endUs:(timeMs * 1000) + ospReadTimeUs(strobe)});
        strobe = (strobe >= 8) ? 1 : strobe + 1;
    }
    for (let timeMs = 40; timeMs < durationMs; timeMs += 100)
    {
        blocks.push({startUs:timeMs * 1000, endUs:(timeMs * 1000) + ospReadTimeUs(logo)});
        logo = (logo >= 24) ? 1 : logo + 1;
    }
    for (let timeMs = 70; timeMs < durationMs; timeMs += 100)
    {
        blocks.push({startUs:timeMs * 1000, endUs:(timeMs * 1000) + 500});
    }
    return blocks.sort((left, right) => left.startUs - right.startUs);
}

function processEvents()
{
    const blocks = buildBlocks();
    const events = [];

    for (let timeMs = 1; timeMs <= durationMs; timeMs++)
    {
        const timeUs = timeMs * 1000;
        const block = blocks.find(item => (timeUs > item.startUs) && (timeUs < item.endUs));
        if (block === undefined)
        {
            events.push({timeMs, timeUs});
        }
    }
    for (const block of blocks)
    {
        const timeMs = Math.floor(block.endUs / 1000);
        if (timeMs > Math.floor(block.startUs / 1000))
        {
            events.push({timeMs, timeUs:block.endUs});
        }
    }
    return events.sort((left, right) => left.timeUs - right.timeUs);
}

function measure(hz, percent)
{
    const threshold = percent * 10;
    const transitions = [{timeUs:0, on:true}];
    let phaseTimeMs = 0;
    let phase = 0;
    let on = true;

    for (const event of processEvents())
    {
        phase = (phase + ((event.timeMs - phaseTimeMs) * hz)) % 1000;
        phaseTimeMs = event.timeMs;
        const next = phase < threshold;
        if (next !== on)
        {
            on = next;
            transitions.push({timeUs:event.timeUs, on});
        }
    }

    const rises = transitions.filter(item => item.on && item.timeUs >= 1000000);
    const periods = [];
    const onTimes = [];
    for (let index = 0; index < (rises.length - 1); index++)
    {
        const start = rises[index].timeUs;
        const end = rises[index + 1].timeUs;
        const falling = transitions.find(item => !item.on && item.timeUs > start && item.timeUs < end);
        periods.push(end - start);
        onTimes.push(falling.timeUs - start);
    }
    const averagePeriod = periods.reduce((sum, value) => sum + value, 0) / periods.length;
    const averageOn = onTimes.reduce((sum, value) => sum + value, 0) / onTimes.length;
    return {
        hz:1000000 / averagePeriod,
        duration:(averageOn * 100) / averagePeriod,
        minPeriod:Math.min(...periods),
        maxPeriod:Math.max(...periods),
        minOn:Math.min(...onTimes),
        maxOn:Math.max(...onTimes)
    };
}

for (let percent = 10; percent <= 90; percent += 10)
{
    const result = measure(50, percent);
    assert(Math.abs(result.hz - 50) < 0.01);
    assert(Math.abs(result.duration - percent) < 0.01);
    assert.equal(result.minPeriod, 20000);
    assert.equal(result.maxPeriod, 20000);
    assert.equal(result.minOn, percent * 200);
    assert.equal(result.maxOn, percent * 200);
    console.log(`PASS 50Hz ${percent}%: ON ${result.minOn / 1000}ms, OFF ${(20000 - result.maxOn) / 1000}ms`);
}

console.log('RESULT 50Hz, Duration 10..90% in 10% steps: PASS with one OSP node read every 100ms.');

const limit = measure(100, 10);
console.log(`INFO 100Hz 10% with Read: ON ${limit.minOn / 1000}..${limit.maxOn / 1000}ms, period ${limit.minPeriod / 1000}..${limit.maxPeriod / 1000}ms`);
