param([ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug')
$ErrorActionPreference = 'Stop'
$project = Split-Path -Parent $PSScriptRoot
$binary = Join-Path $project ('bin/' + $Configuration)
$output = Join-Path $PSScriptRoot ('completion_results/' + $Configuration)
New-Item -ItemType Directory -Path $output -Force | Out-Null
$exe = Join-Path $output 'UpdateCompletionHost.exe'
& 'C:/Windows/Microsoft.NET/Framework64/v4.0.30319/csc.exe' /nologo /target:exe /main:UpdateCompletionHost ('/out:' + $exe) ('/r:' + (Join-Path $binary 'LINMaster.exe')) /r:System.Windows.Forms.dll /r:System.Drawing.dll /r:System.Core.dll (Join-Path $PSScriptRoot 'BootIndependentHost.cs') (Join-Path $PSScriptRoot 'UpdateCompletionHost.cs')
if ($LASTEXITCODE)
{
    throw 'PC completion test compilation failed'
}
& $exe $binary $output
if ($LASTEXITCODE)
{
    throw 'PC completion/rollback test failed'
}
