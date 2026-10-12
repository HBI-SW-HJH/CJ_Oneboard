# CJ IAR download setup

This folder is part of the project. No external programmer application is run.
Debug and Release use `CJ_PIC32CM5164JH.board` and IAR's installed flash loader.

- Press the normal IAR Download/Debug button.
- Only PIC32CM5164JH01032 is accepted; silicon revisions are permitted.
- If BOOTPROT is 0xF and REGION_LOCKS is 0x0000, the User Row is not written or reset again.
- Otherwise, a `UserRow_<time>.hex` original backup is saved in this folder.
- All 256 bytes are read. BOOTPROT bits 3:0 are changed to 0xF and
  REGION_LOCKS bits 15:0 are changed to 0x0000. Every other bit is preserved.
- EAR and four WAP commands restore the complete User Row, then it is verified.
- The probe resets and immediately halts the MCU before the normal loader starts.
- Both Boot and App use this BOOTPROT=0xF preparation.
- After download, execUserFlashExit saves BOOTPROT=0x4 (64 KB Boot protection)
  before restoring WDT/PAC/MPU. This BOOTPROT value takes effect on the next reset.
- All 16 Main Flash regions are write protected after reset and power cycling.
- IAR's PIC32CM JH loader temporarily unlocks each region only while programming,
  then restores the lock state. No manual re-lock step is required after download.
- Keep power stable during the first download. RESET must be connected to the probe.
- Chip Erase, security-bit removal, and Chip Erase Hard Lock are not automated.
- Main and Data Flash are not erased by the BOOTPROT preparation step.
  The Boot project's Main Flash download range is 0x00000000..0x0000FFFF.
  The App project's range is 0x00010000..0x0007FFFF. PDS is not deliberately erased.
- Boot vectors are at 0x00000000; App vectors are at 0x00010000.
  IAR Linker Edit, debugger vector address and download range must agree.
- Boot Reprogram_Init calls Reprogram_WriteProtect_Lock for Main Flash before entering App.
  Both functions take no arguments: Lock covers all 512 KB; Unlock covers only the 448 KB App.
  Download start unlocks the complete App after storing both pending flags.
  Completion, rejected requests, reselection, session restart and the 5-second timeout relock Main Flash.
  Individual row writes do not change region locks.
  These C functions change runtime lock bits only; they do not rewrite the User Row.

Move the complete project folder, including this folder. `$PROJ_DIR$` paths follow it.
IAR EWARM 9.70.4 with PIC32CM JH support and a supported debug probe are required.
The current project uses J-Link.
Close/reopen an already-open IAR workspace once after this configuration update so
the IDE does not retain its old in-memory Download settings.

Verification: project build and IAR macro parser plus register-model tests are possible
without a board. Physical User Row programming/reset/download still require a board
check; register-model tests do not emulate silicon, probe timing, or power failures.

The files under debugger/tests are historical helpers for the earlier macro interface.
They need adaptation to the current function arguments before use.
Never load BootUnlockParser.mac into a real hardware debug session.
This address/protection cleanup has not been built, simulated or downloaded;
the user requested source/style review before verification.

References:
- IAR installed `arm/doc/FlashLoaderGuide.ENU.pdf`, flash-loader process and macros.
- IAR installed `arm/doc/EWARM_DebuggingGuide.ENU.pdf`, macro strings, memory save,
  `__abortLaunch`, `__hwReset`, and `--drv_vector_table_base`.
- Microchip DS60001632H, 10.2.1, 26.5.5-26.5.7, and 50.1.
