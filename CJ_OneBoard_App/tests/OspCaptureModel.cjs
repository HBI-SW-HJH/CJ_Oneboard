// Host model of the single falling-period decoder, not a physical LED test.
'use strict';
const assert = require('node:assert/strict');
const fs = require('node:fs');

function crc(bytes) {
    let value = 0;
    for (const byte of bytes) {
        for (let bit = 7; bit >= 0; bit--) {
            value = ((value << 1) ^ ((((byte >> bit) ^ (value >> 7)) & 1) ? 0x2f : 0)) & 255;
        }
    }
    return value;
}
function levelsOf(raw) {
    return raw.flatMap(byte => Array.from({length: 8}, (_, bit) =>
        ((byte & (128 >> bit)) !== 0) ? [0, 1] : [1, 0]).flat());
}
function capture(raw, halfTicks = 10) {
    const levels = levelsOf(raw).concat([1]);
    let previousLevel = 1, previousFall = null;
    const result = [];
    levels.forEach((level, position) => {
        if (previousLevel === 1 && level === 0) {
            result.push(previousFall === null ? 480 :
                Math.round((position - previousFall) * halfTicks) - 1);
            previousFall = position;
        }
        previousLevel = level;
    });
    return result;
}
function recover(periods) {
    if (!periods.length) return [];
    let start = 0;
    for (let index = periods.length - 1; index > 0; index--) {
        if (periods[index] < 15 || periods[index] >= 45) { start = index; break; }
    }
    let levels = [0, 1], bit = 1;
    for (let index = start + 1; index < periods.length; index++) {
        if (levels.length + 4 > 512) return [];
        const units = periods[index] >= 15 && periods[index] < 25 ? 2 :
            periods[index] >= 25 && periods[index] < 35 ? 3 :
            periods[index] >= 35 && periods[index] < 45 ? 4 : 0;
        let count = 1;
        if (units === 3) {
            bit ^= 1;
            count = bit ? 2 : 1;
        } else if (units === 4 && bit === 0) {
            levels.push(0, 1);
        } else if (units !== 2) {
            levels = [];
            bit = 1;
        }
        while (count-- > 0) levels.push(bit ? 0 : 1, bit ? 1 : 0);
    }
    if (bit === 0 && levels.length + 2 <= 512) levels.push(0, 1);
    return levels;
}
function decode(levels, length, address, command) {
    const psi = length === 12 ? 7 : length - 4;
    const header = [0xa0 | ((address >> 6) & 15),
        ((address & 63) << 2) | (psi >> 1), ((psi & 1) << 7) | command];
    const normal = levelsOf(header), inverted = normal.map(bit => bit ^ 1);
    for (let offset = 0; offset + length * 16 <= levels.length; offset++) {
        const invert = levels[offset], expected = invert ? inverted : normal;
        if (!expected.every((bit, index) => bit === levels[offset + index])) continue;
        const raw = header.concat(Array(length - 3).fill(0));
        let valid = true;
        for (let bit = 24; bit < length * 8; bit++) {
            const first = levels[offset + bit * 2] ^ invert;
            const second = levels[offset + bit * 2 + 1] ^ invert;
            if (first === second) { valid = false; break; }
            if (first === 0) raw[bit >> 3] |= 128 >> (bit & 7);
        }
        if (valid && crc(raw.slice(0, -1)) === raw.at(-1)) return raw;
    }
    return null;
}

for (let data = 0; data < 65536; data++) {
    const raw = [0xa0, 0x05, 0x4c, data >> 8, data & 255];
    raw.push(crc(raw));
    assert.deepEqual(decode(recover(capture(raw)), 6, 1, 0x4c), raw);
}
const reply = [0xa0, 0x04, 0xcc, 0x24, 0x19];
assert.equal(crc([0xa0, 0x04, 0x02]), 0xa9);
for (const clock of [9.2, 10, 10.8]) {
    assert.deepEqual(decode(recover(capture(reply, clock)), 5, 1, 0x4c), reply);
}
assert.equal(decode(recover(capture([0xa0, 0x04, 0x4c, 0xfa])), 5, 1, 0x4c), null);
assert.equal(decode(recover(capture(reply)), 5, 2, 0x4c), null);
assert.equal(decode(recover(capture(reply)), 5, 1, 0x48), null);
assert.equal(decode(recover(capture([0xa0, 0x04, 0xcc, 0x24, 0x18])), 5, 1, 0x4c), null);
assert.deepEqual(decode(recover(capture([0xa0, 0x04, 0x4c, 0xfa])
    .concat([100], capture(reply).slice(1))), 5, 1, 0x4c), reply);
const tempstat = [0xa0, 0x61, 0x42, 142, 0x80];
tempstat.push(crc(tempstat));
assert.deepEqual(decode(recover(capture(tempstat)), 6, 24, 0x42), tempstat);
assert.deepEqual(decode(levelsOf(tempstat).map(bit => bit ^ 1), 6, 24, 0x42), tempstat);
console.log('PASS: 65,536 payloads; TEMPSTAT; inverted response; clock tolerance; CRC; address/command; TX echo rejection; reply after idle.');
if (process.argv[2]) {
    const bytes = fs.readFileSync(process.argv[2]);
    const periods = [];
    for (let index = 0; index < bytes.length; index += 2) {
        const period = bytes.readUInt16LE(index);
        if (!period) break;
        periods.push(period);
    }
    const expected = [0xa0, 0x03, 0x4f, 0x85, 0x04, 0x80, 0, 0x80, 0];
    expected.push(crc(expected));
    const raw = decode(recover(periods), 10, 0, 0x4f);
    assert.deepEqual(raw, expected);
    console.log(`PASS: captured SET_PWM (${periods.length} periods): ${raw.map(b => b.toString(16).padStart(2, '0')).join(' ').toUpperCase()}`);
}
module.exports = {capture, crc, decode, recover};
