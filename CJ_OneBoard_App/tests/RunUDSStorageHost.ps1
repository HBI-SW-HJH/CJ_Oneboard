$ErrorActionPreference = 'Stop'
$project = Split-Path -Parent $PSScriptRoot
$gcc = 'C:\mingw64\bin\gcc.exe'
if (-not (Test-Path -LiteralPath $gcc)) {
    throw "Host GCC not found: $gcc"
}
$env:PATH = (Split-Path -Parent $gcc) + ';' + $env:PATH
$common = @('-std=c11', '-O2', '-Wall', '-Wextra', '-Werror',
    '-Wno-int-to-pointer-cast', '-I', 'tests\mocks', '-I', 'applications\inc',
    '-I', 'drivers\inc', 'tests\UDSStorageHost.c')
Push-Location $project
try {
    & $gcc @common '-o' 'tests\UDSStorageHost.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Base host compile failed.' }
    & '.\tests\UDSStorageHost.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Base host test failed.' }

    & $gcc @common '-DCONFIG_EXTENSION_TEST' '-o' 'tests\UDSStorageExtendedHost.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Extended host compile failed.' }
    & '.\tests\UDSStorageExtendedHost.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Extended host test failed.' }
}
finally {
    Pop-Location
}
