$ErrorActionPreference = 'Stop'
$project = Split-Path -Parent $PSScriptRoot
$env:PATH = 'C:\mingw64\bin;' + $env:PATH
Push-Location $project
try {
    $common = @('-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', '-I', 'tests/mocks')
    & gcc @common -I applications/inc -I drivers/inc tests/IdentityMigrationHost.c -o tests/IdentityAppHost.exe
    if ($LASTEXITCODE) { throw 'App identity compile failed' }
    & ./tests/IdentityAppHost.exe tests/AppIdentityWire.bin
    if ($LASTEXITCODE) { throw 'App identity test failed' }
    & ../CJ_OneBoard_Boot_20260930/tests/RunBootIndependentHost.ps1

    $source = [IO.File]::ReadAllText((Join-Path $project 'applications/src/OSPControl.c'))
    $start = $source.IndexOf('static void OSP_WriteColor(')
    $end = $source.IndexOf('static void OSP_ReadStatic(', $start)
    if ($start -lt 0 -or $end -le $start) { throw 'Cannot locate production OSP_WriteColor' }
    [IO.File]::WriteAllText((Join-Path $PSScriptRoot 'OSPWriteColor.generated.h'), $source.Substring($start, $end - $start))
    foreach ($limit in @('default', '0', '1', '82', '150')) {
        $option = @()
        if ($limit -ne 'default') { $option = @('-DTEST_LIMIT_MA=' + $limit) }
        & gcc -std=c11 -O2 -Wall -Wextra -Werror -I applications/inc @option tests/OSPCurrentHost.c -o tests/OSPCurrentHost.exe
        if ($LASTEXITCODE) { throw 'OSP limit compile failed' }
        & ./tests/OSPCurrentHost.exe
        if ($LASTEXITCODE) { throw 'OSP limit test failed' }
    }
}
finally { Pop-Location }
