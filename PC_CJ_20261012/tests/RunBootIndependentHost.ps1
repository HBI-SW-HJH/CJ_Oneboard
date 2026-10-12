param([ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug')
$ErrorActionPreference = 'Stop'
$project = Split-Path -Parent $PSScriptRoot
$binary = Join-Path $project ('bin/' + $Configuration)
$output = Join-Path $PSScriptRoot 'boot_independent_results'
New-Item -ItemType Directory -Path $output -Force | Out-Null
$exe = Join-Path $output 'BootIndependentHost.exe'
& 'C:/Windows/Microsoft.NET/Framework64/v4.0.30319/csc.exe' /nologo /target:exe ('/out:' + $exe) ('/r:' + (Join-Path $binary 'LINMaster.exe')) /r:System.Windows.Forms.dll /r:System.Drawing.dll /r:System.Core.dll (Join-Path $PSScriptRoot 'BootIndependentHost.cs')
if ($LASTEXITCODE) { throw 'PC Boot host compile failed' }
& $exe $binary $output
if ($LASTEXITCODE) { throw 'PC Boot isolation/retry test failed' }
