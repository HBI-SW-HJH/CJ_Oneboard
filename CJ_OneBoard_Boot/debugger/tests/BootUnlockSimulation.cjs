// Executes a mechanical translation of the PRODUCTION C-SPY macro against a
// register model. No debug probe is connected and no real MCU is programmed.
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');

const source = fs.readFileSync(path.join(__dirname, '..', 'CJ_PIC32CM_BootUnlock.mac'), 'utf8');
const translated = source
    .replace(/__var\s+(\w+);/g, 'let $1 = 0;')
    .replace(/^(\w+)\(([^\n]*)\)\s*$/gm, 'function $1($2)')
    .replace(/sizeof\((\w+)\)/g, '$1.length')
    .replace(/original\[0\]/g, '((original.charCodeAt(0) << 24) >> 24)')
    .replace(/__smessage "\$PROJ_DIR\$\/debugger\/UserRow_", __wallTime_ms\(\):%u, "\.hex"/g,
        '"$PROJ_DIR$/debugger/UserRow_" + __wallTime_ms() + ".hex"')
    .replace(/^(\s*)__message (.*);$/gm, '$1__message($2);');

function model(options = {}) {
    const bytes = Buffer.from(Array.from({ length: 256 }, (_, i) => (i * 73 + 251) & 255));
    bytes[0] = (bytes[0] & 0xF0) | (options.boot ?? 0xB);
    bytes.writeUInt16LE(options.regionLocks ?? 0xFFFF, 6);
    const original = Buffer.from(bytes);
    const registers = new Map([
        [0x41002018, options.device ?? 0x11060103],
        [0x41004004, 0x12340004],
        [0x41004014, 1],
        [0x41004018, options.security ? 0x100 : 0],
        [0x4100401C, 0],
        [0x41004020, bytes.readUInt16LE(6)],
        [0x40000038, options.pac ? 4 : 0],
        [0x40000034, options.pac ? 0x100 : 0],
        [0x40002000, options.watchdog ?? 0],
        [0xE000ED94, options.mpu ? 1 : 0],
        [0xE000EDF0, 0x20000]
    ]);
    const state = { bytes, original, registers, writes: [], commands: [], backups: [],
        resets: 0, now: 1000, busy: false, reads: 0, latchedBoot: bytes[0] & 15,
        latchedRegionLocks: bytes.readUInt16LE(6),
        pageBuffer: Buffer.alloc(64, 255), messages: [] };

    function read(address, size) {
        if (address >= 0x00804000 && address + size <= 0x00804100) {
            return size === 4 ? bytes.readUInt32LE(address - 0x00804000)
                : size === 2 ? bytes.readUInt16LE(address - 0x00804000)
                : bytes[address - 0x00804000];
        }
        if (address === 0x41004014 && state.busy) return 0;
        return registers.get(address) ?? 0;
    }
    function write(value, address, size) {
        state.writes.push({ value, address, size });
        if (address === 0x40000000) {
            if (!options.pacFailure) {
                if (value === 0x10022) registers.set(0x40000038, 0);
                if (value === 0x20022) registers.set(0x40000038, 4);
                if (value === 0x10008) registers.set(0x40000034, 0);
                if (value === 0x20008) registers.set(0x40000034, 0x100);
            }
            return;
        }
        if (address === 0x41004018) {
            registers.set(address, (registers.get(address) ?? 0) & ~(value & 0x1C));
            return;
        }
        if (address === 0x41004014) {
            registers.set(address, (registers.get(address) ?? 0) & ~(value & 2));
            return;
        }
        registers.set(address, value);
        if (address !== 0x41004000) return;
        assert.equal(value & 0xFF00, 0xA500, 'Command key');
        const command = value & 255;
        state.commands.push(command);
        if (options.timeoutCommand === command) { state.busy = true; return; }
        if (options.errorCommand === command) {
            registers.set(0x41004014, 3);
            registers.set(0x41004018, 8);
            return;
        }
        if (command === 0x44) { state.pageBuffer.fill(255); return; }
        const destination = registers.get(0x4100401C) * 2;
        assert.ok(destination >= 0x00804000 && destination < 0x00804100,
            'Only User Row is erased/programmed');
        if (command === 5) {
            assert.ok(state.backups.length > 0, 'Durable backup before erase');
            assert.equal(registers.get(0x40002000) & 2, 0, 'WDT disabled before erase');
            bytes.fill(255);
        } else if (command === 6) {
            state.pageBuffer.copy(bytes, destination - 0x00804000);
        } else {
            assert.fail('Unexpected or destructive command ' + command);
        }
    }

    const context = {
        __readMemory32: address => read(address, 4),
        __readMemory16: address => read(address, 2),
        __readMemory8: address => read(address, 1),
        __writeMemory32: (value, address) => write(value, address, 4),
        __writeMemory16: (value, address) => write(value, address, 2),
        __writeMemory8: (value, address) => write(value, address, 1),
        __readMemoryBuffer: (count, address) => {
            assert.ok(address >= 0x00804000 && address + count <= 0x00804100);
            const result = Buffer.from(bytes.subarray(address - 0x00804000,
                address - 0x00804000 + count));
            state.reads++;
            if (options.unstable && state.reads === 2) result[1] ^= 1;
            if (options.corruptVerify && state.commands.includes(6)) result[1] ^= 1;
            return result.toString('latin1');
        },
        __writeMemoryBuffer: (buffer, count, address) => {
            assert.equal(buffer.length, count);
            assert.ok(address >= 0x00804000 && address + count <= 0x00804100);
            state.writes.push({ address, size: count });
            Buffer.from(buffer, 'latin1').copy(state.pageBuffer, address % 64);
        },
        __makeString: (count, value) => String.fromCharCode(value & 255).repeat(count),
        __subString: (buffer, start, count) => buffer.slice(start, start + count),
        __memorySave: (start, stop, format, filename, zero) => {
            assert.equal(start, 'Memory:0x00804000');
            assert.equal(stop, 'Memory:0x008040FF');
            assert.equal(format, 'intel-extended');
            assert.equal(zero, 0);
            assert.ok(filename.startsWith('$PROJ_DIR$/debugger/UserRow_'));
            if (options.backupFailure) return 1;
            state.backups.push(Buffer.from(bytes));
            return 0;
        },
        __hwReset: delay => {
            assert.equal(delay, 0);
            state.resets++;
            if (options.resetFailure) return -2;
            state.latchedBoot = bytes[0] & 15;
            state.latchedRegionLocks = bytes.readUInt16LE(6);
            registers.set(0x41004020, state.latchedRegionLocks);
            registers.set(0xE000EDF0, options.notHalted ? 0 : 0x20000);
            registers.set(0x40002000, 0);
            registers.set(0x40000034, 0);
            registers.set(0x40000038, 0);
            return 0;
        },
        __wallTime_ms: () => state.now,
        __delay: delay => { state.now += delay; },
        __message: (...values) => state.messages.push(values.join('')),
        __abortLaunch: message => { throw new Error(message); }
    };
    vm.createContext(context);
    vm.runInContext(translated, context, { timeout: 1000 });
    state.run = (functionName = 'CJ_ProvisionFlashProtection') =>
        vm.runInContext(functionName + '()', context, { timeout: 2000 });
    state.exit = () => vm.runInContext('execUserFlashExit()', context);
    return state;
}

let passed = 0;
function test(name, action) { action(); passed++; console.log('PASS ' + name); }
function checkProvisioned(state) {
    const expected = Buffer.from(state.original);
    expected[0] |= 15;
    expected.writeUInt16LE(0, 6);
    assert.deepEqual(state.bytes, expected, 'All other 2028 bits preserved');
    assert.deepEqual(state.backups[0], state.original);
    assert.deepEqual(state.commands, [5, 0x44, 6, 0x44, 6, 0x44, 6, 0x44, 6]);
    assert.equal(state.resets, 1);
    assert.equal(state.latchedBoot, 15);
    assert.equal(state.latchedRegionLocks, 0);
}

test('Already provisioned: zero preparation writes/backups/resets', () => {
    const state = model({ boot: 15, regionLocks: 0 }); state.run();
    assert.equal(state.writes.length, 0); assert.equal(state.backups.length, 0);
    assert.equal(state.resets, 0); assert.deepEqual(state.bytes, state.original);
});
for (let boot = 0; boot < 15; boot++) {
    test('BOOTPROT=' + boot.toString(16) + ': preserve all other bits', () => {
        const state = model({ boot }); state.run(); checkProvisioned(state);
        const count = state.writes.length;
        state.run(); assert.equal(state.writes.length, count, 'Second download skips provisioning');
        assert.equal(state.resets, 1); assert.equal(state.backups.length, 1);
    });
}
test('BOOTPROT ready but regions unlocked: enable persistent protection', () => {
    const state = model({ boot: 15, regionLocks: 0xFFFF }); state.run();
    checkProvisioned(state);
});
test('Regions protected but BOOTPROT active: disable BOOTPROT', () => {
    const state = model({ boot: 0xB, regionLocks: 0 }); state.run();
    checkProvisioned(state);
});
test('Silicon revision/die variation', () => {
    const state = model({ device: 0x1106A203 }); state.run(); checkProvisioned(state);
});
for (const device of [0x11060102, 0x11060115, 0x10010003, 0xFFFFFFFF]) {
    test('Wrong MCU ' + device.toString(16) + ': no writes', () => {
        const state = model({ device }); assert.throws(() => state.run(), /requires PIC32CM/);
        assert.equal(state.writes.length, 0); assert.deepEqual(state.bytes, state.original);
    });
}
for (const [name, options, message] of [
    ['Security', { security: true }, /security/],
    ['Unstable read', { unstable: true }, /stable/],
    ['Backup failure', { backupFailure: true }, /save/],
    ['Always-on WDT', { watchdog: 0x82 }, /watchdog/],
    ['PAC failure', { pac: true, pacFailure: true }, /prepare/]
]) {
    test(name + ': stop before User Row erase', () => {
        const state = model(options); assert.throws(() => state.run(), message);
        assert.equal(state.commands.length, 0); assert.deepEqual(state.bytes, state.original);
    });
}
for (const command of [5, 0x44, 6]) {
    test('Command ' + command + ' failure: stop, preserve original backup', () => {
        const state = model({ errorCommand: command });
        assert.throws(() => state.run(), /command failed/);
        assert.deepEqual(state.backups[0], state.original); assert.equal(state.resets, 0);
    });
    test('Command ' + command + ' timeout: bounded stop', () => {
        const state = model({ timeoutCommand: command });
        assert.throws(() => state.run(), /timeout/);
        assert.deepEqual(state.backups[0], state.original); assert.equal(state.resets, 0);
        assert.ok(state.now <= 2000);
    });
}
for (const [name, options, message] of [
    ['Readback mismatch', { corruptVerify: true }, /verification failed/],
    ['Reset failure', { resetFailure: true }, /reset failed/],
    ['CPU not halted', { notHalted: true }, /reconnect verification/]
]) {
    test(name + ': do not continue application download', () => {
        const state = model(options); assert.throws(() => state.run(), message);
        assert.deepEqual(state.backups[0], state.original);
    });
}
test('Full IAR hook: prepare before save; restore post-reset state', () => {
    const state = model({ pac: true, watchdog: 2, mpu: true });
    state.run('execUserFlashInit'); checkProvisioned(state); state.exit();
    assert.equal(state.registers.get(0x40002000), 0, 'No stale pre-reset WDT restore');
    assert.equal(state.registers.get(0x40000038), 0, 'No stale pre-reset PAC restore');
});
test('Full IAR hook on provisioned MCU retains normal PAC/WDT restore', () => {
    const state = model({ boot: 15, regionLocks: 0, pac: true, watchdog: 2, mpu: true });
    state.run('execUserFlashInit'); assert.equal(state.commands.length, 0);
    state.exit(); assert.equal(state.registers.get(0x40002000), 2);
    assert.equal(state.registers.get(0x40000038), 4);
    assert.equal(state.registers.get(0x40000034), 0x100);
    assert.equal(state.registers.get(0xE000ED94), 1);
    assert.equal(state.resets, 0);
});
console.log('RESULT ' + passed + ' PASS / 0 FAIL (register model; not hardware)');
