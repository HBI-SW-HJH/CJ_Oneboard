// Compare the user's earlier project with the current on-board TX capture.
'use strict';
const fs = require('node:fs');
const assert = require('node:assert/strict');
const path = require('node:path');
const referenceRoot = 'D:/jhhan/program/CJ/CJ_Final_20260715_OTP_Test';
const reference = fs.readFileSync(path.join(referenceRoot, 'applications/src/spi_led.c'), 'utf8');
const tableText = reference.match(/manLookUpTable\[256\]\s*=\s*\{([\s\S]*?)\};/)[1];
const table = [...tableText.matchAll(/0x([0-9a-f]+)/gi)].map(match => parseInt(match[1], 16));
assert.equal(table.length, 256);
for (let byte = 0; byte < 256; byte++) {
    let encoded = 0;
    for (let bit = 7; bit >= 0; bit--) encoded = (encoded << 2) | ((byte & (1 << bit)) ? 1 : 2);
    assert.equal(encoded, table[byte], `Manchester byte ${byte}`);
}
const model = require('./OspCaptureModel.cjs');
const current = fs.readFileSync(path.resolve(__dirname, '../applications/src/OSPControl.c'), 'utf8');
function lookup(name) {
    return [...current.match(new RegExp(name + '\\[16U\\]\\s*=\\s*\\{([\\s\\S]*?)\\};'))[1]
        .matchAll(/0x([0-9a-f]+)U/gi)].map(match => parseInt(match[1], 16));
}
const manNibble = lookup('encodedNibble'), crcNibble = lookup('crcNibble');
for (let byte = 0; byte < 256; byte++) {
    assert.equal((manNibble[byte >> 4] << 8) | manNibble[byte & 15], table[byte]);
    for (let initial = 0; initial < 256; initial++) {
        let crc = initial ^ byte;
        crc = ((crc << 4) ^ crcNibble[crc >> 4]) & 255;
        crc = ((crc << 4) ^ crcNibble[crc >> 4]) & 255;
        assert.equal(crc, model.crc([initial ^ byte]));
    }
}
console.log('PASS: current C nibble tables; all 256 Manchester bytes; all 65,536 CRC state/byte combinations.');
function referenceEncode(raw) {
    return raw.flatMap(byte => [table[byte] >> 8, table[byte] & 255]);
}
const frames = [
    ['RESET', [0xa0, 0, 0, 0x22]],
    ['SET_SETUP', [0xa0, 0, 0xcd, 0x24, 0xc8]],
    ['INIT_BIDIR', [0xa0, 4, 2, 0xa9]],
    ['GO_ACTIVE', [0xa0, 0, 5, 0xb1]],
    ['RED_BROADCAST', [0xa0, 3, 0x4f, 0x85, 4, 0x80, 0, 0x80, 0]],
    ['RED_NODE1', [0xa0, 7, 0x4f, 0x85, 4, 0x80, 0, 0x80, 0]]
];
const hex = raw => raw.map(byte => byte.toString(16).padStart(2, '0')).join(' ').toUpperCase();
for (const [name, raw] of frames) {
    if (raw.length === 9) raw.push(model.crc(raw));
    assert.equal(model.crc(raw.slice(0, -1)), raw.at(-1));
    console.log(`${name}: RAW=${hex(raw)} ENCODED=${hex(referenceEncode(raw))}`);
}
const captured = fs.readFileSync(path.resolve(__dirname, '../../../tmp/iar_busy_diagnostic/OSP_Logo_Red_Capture.bin'));
const periods = [];
for (let index = 0; index < captured.length; index += 2) {
    const value = captured.readUInt16LE(index);
    if (!value) break;
    periods.push(value);
}
assert.deepEqual(model.decode(model.recover(periods), 10, 0, 0x4f), frames[4][1]);
console.log('PASS: all 256 Manchester encodings match the reference LUT; captured broadcast RED has the expected OSP data and CRC.');
module.exports = {table, frames, referenceEncode};
