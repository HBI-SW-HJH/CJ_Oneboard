$ErrorActionPreference = 'Stop'
$bootProject = Split-Path -Parent $PSScriptRoot
$suite = Split-Path -Parent $bootProject
$appProject = Join-Path $suite 'CJ_OneBoard_App_20260930'
$output = Join-Path $PSScriptRoot 'generated'
New-Item -ItemType Directory -Path $output -Force | Out-Null
$env:PATH = 'C:\mingw64\bin;' + $env:PATH
foreach ($mode in @('boot', 'app')) {
    $project = if ($mode -eq 'boot') { $bootProject } else { $appProject }
    $source = [IO.File]::ReadAllText((Join-Path $project 'applications/src/Reprogram.c'))
    $source = [regex]::Replace($source, 'static uint8_t Reprogram_Command\(uint8_t command\)\s*\{.*?\n\}', 'static uint8_t Reprogram_Command(uint8_t command) { return HostCommand(command); }', 'Singleline')
    $source = [regex]::Replace($source, 'static uint8_t Reprogram_WriteRow\(uint32_t address, const uint32_t \*data\)\s*\{.*?\n\}', 'static uint8_t Reprogram_WriteRow(uint32_t address, const uint32_t *data) { return HostWriteRow(address, data); }', 'Singleline')
    $source = [regex]::Replace($source, '__stackless __noreturn static void Reprogram_StartApp\(uint32_t address\)\s*\{.*?\n\}', 'static void Reprogram_StartApp(uint32_t address) { HostJump(address); }', 'Singleline')
    $prefix = ''
    if ($mode -eq 'app') {
        foreach ($name in @('gUpdateMain', 'gUpdateBackup', 'gReprogram', 'Reprogram_Init', 'Reprogram_Process', 'Reprogram_Request', 'Reprogram_HardFault')) { $prefix += "#define $name App_$name`n" }
        $source += "`nvoid HostAppStart(void) { memset(&gReprogram, 0, sizeof(gReprogram)); Reprogram_Init(); }`n"
    }
    $prefix += '#include "' + ((Join-Path $project 'applications/inc/Reprogram.h') -replace '\\', '/') + '"' + "`n"
    [IO.File]::WriteAllText((Join-Path $output "generated_$mode.c"), $prefix + $source)
}
& 'C:/mingw64/bin/gcc.exe' -std=c11 -O2 -Wall -Wextra -Wno-int-to-pointer-cast -Wno-clobbered -Wno-unused-function -I (Join-Path $PSScriptRoot 'mocks') -I (Join-Path $bootProject 'applications/inc') -I (Join-Path $bootProject 'drivers/inc') -I $output (Join-Path $PSScriptRoot 'BootRecoveryHost.c') (Join-Path $output 'generated_app.c') -o (Join-Path $output 'BootRecoveryHost.exe')
if ($LASTEXITCODE) { throw 'Boot host compile failed' }
& (Join-Path $output 'BootRecoveryHost.exe')
if ($LASTEXITCODE) { throw 'Boot isolation/recovery test failed' }
