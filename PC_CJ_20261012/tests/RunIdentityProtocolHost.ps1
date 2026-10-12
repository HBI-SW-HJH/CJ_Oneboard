param([ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug')
$ErrorActionPreference = 'Stop'
$project = Split-Path -Parent $PSScriptRoot
$root = Split-Path -Parent $project
$binary = Join-Path $project ('bin/' + $Configuration)
$fixtures = Join-Path $root 'CJ_OneBoard_App_20260930/tests'
$results = Join-Path $PSScriptRoot 'results'
New-Item -ItemType Directory -Path $results -Force | Out-Null
$compiler = 'C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe'
$exe = Join-Path $PSScriptRoot 'IdentityProtocolHost.exe'
& $compiler /nologo /target:exe ('/out:' + $exe) ('/r:' + (Join-Path $binary 'LINMaster.exe')) /r:System.Windows.Forms.dll /r:System.Drawing.dll /r:System.Core.dll (Join-Path $PSScriptRoot 'IdentityProtocolHost.cs')
if ($LASTEXITCODE) { throw 'PC host test compilation failed' }
& $exe $binary $results $fixtures
if ($LASTEXITCODE) { throw 'PC identity/Scan/Write/Update test failed' }
