# PIC32CM5164JH01032 SRAM ECC startup

## Applied change

`startup/startup_pic32cmjh01.c` now routes the Reset vector through
`Reset_Handler`, then enters the normal IAR `__iar_program_start` runtime.
Debug and Release ELF entry points in `CJ_OneBoard_APP.ewp` are also
`Reset_Handler`, so debugger startup does not bypass the SRAM fill.

The reset function is IAR `__stackless`: it must not use CSTACK, call a
helper, or use byte/halfword writes before the fill completes. It writes
zero to each aligned 32-bit word from `DRAM_ADDR` to `DRAM_ADDR + DRAM_SIZE`
(exclusive), using the selected device header (currently 64KB).
Interrupts are masked during the fill, a DSB completes writes, and the
normal runtime performs its original .data/.bss initialization afterwards.

This clears volatile SRAM, including CSTACK/HEAP. Do not store reset-retained
application data in SRAM without adapting this startup routine. The current
project stores persistent chair configuration in Data Flash, not SRAM.
Main Flash, Data Flash and User Row are not written by this reset function.

MISRA note: the integer-to-volatile-pointer conversion and IAR function
attributes are explicit low-level startup deviations. Builds and generated
machine code were checked; a full certified MISRA analysis was not performed.

## Why the previous build could work intermittently

SRAM ECC is enabled by default on this device. An aligned 32-bit write
initializes both data and ECC, while byte/halfword writes internally read
the existing word first. If the word has not been initialized, that read
can report an ECC error.

Earlier firmware or debugger/loader operations may already have initialized
the addresses used by the stack. Power-on state, stack placement and call
paths can differ later. This is a plausible explanation for earlier success,
not a measured reconstruction of the user's first successful download.

This requirement depends on the exact PIC family and its startup code; it is
not a defect unique to all PIC products. A different device without SRAM ECC,
or with an appropriate ROM/runtime SRAM initialization, need not show this
failure. Compare the other device's datasheet rather than assuming all PICs
have the same reset behavior.

## Verification on 2026-09-15

- IAR EWARM 9.70.4 Debug and Release: zero errors and warnings.
- Both generated reset functions use aligned STR writes and no stack access
  before the fill. Actual hardware Reset PC is the new reset entry 0x368C.
- Normal IAR flash-loader download succeeded after temporarily halting the
  old software-reset loop. An initial connection while that loop ran failed
  reading AIRCR; that failed attempt did not load the flash loader.
- J-Link VerifyBin matched all 14528 bytes of the Debug image in Main Flash.
- Main loop 0x3752 was reached with both initialization and normal operation.
- After a normal SYSRESETREQ, the reset vector and subsequent completion of
  SYS_Init were verified again with a hardware-only breakpoint.
- MCRAMC.INTSTA = 0; the old faulting address 0x2000FE8C is readable.
- All 65536 SRAM bytes were successfully read after initialization.
- HardFault/reset hardware breakpoints did not trigger during the first
  10-second observation of normal operation.
- A second 10-second observation after SYSRESETREQ also completed without
  HardFault/reset breakpoint hits. Temporary breakpoints were cleared and
  the MCU was left running.
- Data Flash (8192 bytes) and User Row (256 bytes) before/after snapshots
  are byte-identical by SHA-256. No chair ID/configuration change was requested.

The verification scope is reset initialization and removal of the observed
SRAM ECC fault. It does not prove every LED function, Manchester waveform,
LIN transaction or power-on corner case. A complete power-off/on was not done.

Separate observation: initial OSP diagnostics waited about 19 seconds before
SYS_Init completed on the connected setup (first-loop timer = 19253ms after
SYSRESETREQ). This response-wait delay is separate from the fixed ECC reset
loop. No OSP timeout, diagnostic schedule or LED behavior was changed here.

Reopen an already-open IAR project after the EWP entry-point change.

## References

- Microchip DS60001632H, 45.4.2 SRAM Initialization:
  https://onlinedocs.microchip.com/oxy/GUID-76938A18-C47D-4351-9D02-463E8A957829-en-US-8/GUID-6F65C95B-9DB7-4744-AF81-6939C3856D93.html
- IAR EWARM 9.70 extended keywords, `__stackless`:
  https://docs.iar.com/ewarm/9.7x/en/iar-c-c---development/extended-keywords/summary-of-extended-keywords.html
