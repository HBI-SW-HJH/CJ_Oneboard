param(
    [string]$RepoRoot = ""
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($RepoRoot)) {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\\..")).Path
}

$featureRoot = Join-Path $RepoRoot "codes\\comsect1\\project\\features"
if (-not (Test-Path $featureRoot)) {
    Write-Host "[arch-check] feature root not found: $featureRoot"
    exit 1
}
$coreRoot = Join-Path $RepoRoot "codes\\comsect1\\infra\\bootstrap"
$serviceRoot = Join-Path $RepoRoot "codes\\comsect1\\infra\\service"
$halRoot = Join-Path $RepoRoot "codes\\comsect1\\infra\\platform\\hal"
$bspRoot = Join-Path $RepoRoot "codes\\comsect1\\infra\\platform\\bsp"

$errors = New-Object System.Collections.Generic.List[string]

function Add-ArchError {
    param([string]$Message)
    $null = $errors.Add($Message)
}

function Get-IncludeTokens {
    param([string]$FilePath)
    $tokens = New-Object System.Collections.Generic.List[string]
    $lines = Get-Content -Path $FilePath
    foreach ($line in $lines) {
        if ($line -match '^\s*#include\s+[<"]([^">]+)[">]') {
            $null = $tokens.Add($matches[1])
        }
    }
    return $tokens
}

function Get-FeatureNameFromPath {
    param(
        [string]$RootPath,
        [string]$FilePath
    )

    $normalizedRoot = $RootPath.Replace('/', '\').TrimEnd('\')
    $normalizedPath = $FilePath.Replace('/', '\')

    if (-not $normalizedPath.StartsWith($normalizedRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        return ""
    }

    $relative = $normalizedPath.Substring($normalizedRoot.Length).TrimStart('\')
    if ([string]::IsNullOrWhiteSpace($relative)) {
        return ""
    }

    return ($relative -split '\\')[0]
}

$poiHeaders = Get-ChildItem -Path $featureRoot -Recurse -File -Filter "poi_*.h"
$poiHeadersByName = @{}
foreach ($header in $poiHeaders) {
    $key = $header.Name.ToLowerInvariant()
    if (-not $poiHeadersByName.ContainsKey($key)) {
        $poiHeadersByName[$key] = New-Object System.Collections.Generic.List[object]
    }

    $entry = [PSCustomObject]@{
        Path = $header.FullName
        Feature = Get-FeatureNameFromPath -RootPath $featureRoot -FilePath $header.FullName
    }
    $null = $poiHeadersByName[$key].Add($entry)
}

$prxHeaders = Get-ChildItem -Path $featureRoot -Recurse -File -Filter "prx_*.h"
$prxHeadersByName = @{}
foreach ($header in $prxHeaders) {
    $key = $header.Name.ToLowerInvariant()
    if (-not $prxHeadersByName.ContainsKey($key)) {
        $prxHeadersByName[$key] = New-Object System.Collections.Generic.List[object]
    }

    $entry = [PSCustomObject]@{
        Path = $header.FullName
        Feature = Get-FeatureNameFromPath -RootPath $featureRoot -FilePath $header.FullName
    }
    $null = $prxHeadersByName[$key].Add($entry)
}

# Rule 1: ida_ in features/core must not include low-level middleware/BSP/external stack headers.
$idaFiles = @(
    Get-ChildItem -Path $featureRoot -Recurse -File -Filter "ida_*.c"
    Get-ChildItem -Path $featureRoot -Recurse -File -Filter "ida_*.h"
)
if (Test-Path $coreRoot) {
    $idaFiles += Get-ChildItem -Path $coreRoot -File -Filter "ida_*.c"
    $idaFiles += Get-ChildItem -Path $coreRoot -File -Filter "ida_*.h"
}
foreach ($file in $idaFiles) {
    $sourceFeature = Get-FeatureNameFromPath -RootPath $featureRoot -FilePath $file.FullName
    $includes = Get-IncludeTokens -FilePath $file.FullName
    foreach ($inc in $includes) {
        $incLeaf = Split-Path -Path $inc -Leaf
        if ($inc -match '^_hatbit') {
            Add-ArchError("$($file.FullName): ida_ file includes external stack header '$inc'")
        }
        if ($inc -match '^bsp_') {
            Add-ArchError("$($file.FullName): ida_ file includes BSP header '$inc'")
        }
        if ($inc -match '^mdw_') {
            Add-ArchError("$($file.FullName): ida_ file includes middleware header '$inc'")
        }
        if ($inc -match '^svc_') {
            Add-ArchError("$($file.FullName): ida_ file includes service header '$inc'")
        }
        if (($inc -match '^cfg_') -and ($inc -ne 'cfg_core.h')) {
            Add-ArchError("$($file.FullName): ida_ file includes non-core config header '$inc'")
        }
        if ($inc -match '^db_') {
            Add-ArchError("$($file.FullName): ida_ file includes database header '$inc'")
        }
        if ($inc -match '^stm_') {
            Add-ArchError("$($file.FullName): ida_ file includes datastream header '$inc'")
        }
        if ($inc -match '^hal_') {
            Add-ArchError("$($file.FullName): ida_ file includes HAL header '$inc'")
        }

        # IDA policy lock: feature-level ida_ may include only own prx_/poi_ headers.
        if ((-not [string]::IsNullOrWhiteSpace($sourceFeature)) -and ($incLeaf -match '^prx_.*\.h$')) {
            $headerKey = $incLeaf.ToLowerInvariant()
            if (-not $prxHeadersByName.ContainsKey($headerKey)) {
                Add-ArchError("$($file.FullName): ida_ file includes unresolved prx header '$incLeaf'")
                continue
            }

            $targets = $prxHeadersByName[$headerKey]
            $sameFeatureTargets = @($targets | Where-Object { $_.Feature -eq $sourceFeature })
            if ($sameFeatureTargets.Count -eq 0) {
                $targetFeatures = @($targets | ForEach-Object { $_.Feature } | Sort-Object -Unique) -join ", "
                Add-ArchError("$($file.FullName): ida_ file includes cross-feature prx header '$incLeaf' (caller='$sourceFeature', owner='$targetFeatures')")
            }
        }

        if ((-not [string]::IsNullOrWhiteSpace($sourceFeature)) -and ($incLeaf -match '^poi_.*\.h$')) {
            $headerKey = $incLeaf.ToLowerInvariant()
            if (-not $poiHeadersByName.ContainsKey($headerKey)) {
                Add-ArchError("$($file.FullName): ida_ file includes unresolved poi header '$incLeaf'")
                continue
            }

            $targets = $poiHeadersByName[$headerKey]
            $sameFeatureTargets = @($targets | Where-Object { $_.Feature -eq $sourceFeature })
            if ($sameFeatureTargets.Count -eq 0) {
                $targetFeatures = @($targets | ForEach-Object { $_.Feature } | Sort-Object -Unique) -join ", "
                Add-ArchError("$($file.FullName): ida_ file includes cross-feature poi header '$incLeaf' (caller='$sourceFeature', owner='$targetFeatures')")
            }
        }
    }
}

# Rule 2: prx_ and poi_ implementation files must not include ida_ headers.
$prxFiles = Get-ChildItem -Path $featureRoot -Recurse -File -Filter "prx_*.c"
$poiFiles = Get-ChildItem -Path $featureRoot -Recurse -File -Filter "poi_*.c"
foreach ($file in @($prxFiles + $poiFiles)) {
    $includes = Get-IncludeTokens -FilePath $file.FullName
    foreach ($inc in $includes) {
        if ($inc -match '^ida_.*\.h$') {
            Add-ArchError("$($file.FullName): implementation file includes ida header '$inc'")
        }
    }
}

# Rule 3: prx_ public headers should not include other prx_ headers directly.
foreach ($file in $prxHeaders) {
    $includes = Get-IncludeTokens -FilePath $file.FullName
    foreach ($inc in $includes) {
        if ($inc -match '^prx_.*\.h$') {
            Add-ArchError("$($file.FullName): prx_ header includes another prx header '$inc'")
        }
    }
}

# Rule 4: poi_ public headers should not include other poi_ headers directly.
foreach ($file in $poiHeaders) {
    $includes = Get-IncludeTokens -FilePath $file.FullName
    foreach ($inc in $includes) {
        if ($inc -match '^poi_.*\.h$') {
            Add-ArchError("$($file.FullName): poi_ header includes another poi header '$inc'")
        }
        if ($inc -match '^ida_.*\.h$') {
            Add-ArchError("$($file.FullName): poi_ header includes ida header '$inc'")
        }
    }
}

# Rule 4b: prx_ public headers should not include ida_ headers directly.
foreach ($file in $prxHeaders) {
    $includes = Get-IncludeTokens -FilePath $file.FullName
    foreach ($inc in $includes) {
        if ($inc -match '^ida_.*\.h$') {
            Add-ArchError("$($file.FullName): prx_ header includes ida header '$inc'")
        }
    }
}

# Rule 5: prx_ must not include poi_ headers owned by another feature.
$allPrxFiles = @($prxFiles + $prxHeaders)
foreach ($file in $allPrxFiles) {
    $sourceFeature = Get-FeatureNameFromPath -RootPath $featureRoot -FilePath $file.FullName
    $includes = Get-IncludeTokens -FilePath $file.FullName

    foreach ($inc in $includes) {
        $incLeaf = Split-Path -Path $inc -Leaf
        if ($incLeaf -notmatch '^poi_.*\.h$') {
            continue
        }

        $headerKey = $incLeaf.ToLowerInvariant()
        if (-not $poiHeadersByName.ContainsKey($headerKey)) {
            Add-ArchError("$($file.FullName): prx_ file includes unresolved poi header '$incLeaf'")
            continue
        }

        $targets = $poiHeadersByName[$headerKey]
        $sameFeatureTargets = @($targets | Where-Object { $_.Feature -eq $sourceFeature })
        if ($sameFeatureTargets.Count -eq 0) {
            $targetFeatures = @($targets | ForEach-Object { $_.Feature } | Sort-Object -Unique) -join ", "
            Add-ArchError("$($file.FullName): prx_ file includes cross-feature poi header '$incLeaf' (caller='$sourceFeature', owner='$targetFeatures')")
        }
    }
}

# Rule 6: generic static symbol names are forbidden (debug symbol uniqueness).
$genericStaticRoots = @($featureRoot)
if (Test-Path $coreRoot) {
    $genericStaticRoots += $coreRoot
}
if (Test-Path $serviceRoot) {
    $genericStaticRoots += $serviceRoot
}
if (Test-Path $halRoot) {
    $genericStaticRoots += $halRoot
}
if (Test-Path $bspRoot) {
    $genericStaticRoots += $bspRoot
}

$genericStaticFiles = New-Object System.Collections.Generic.List[object]
foreach ($root in $genericStaticRoots) {
    $rootFiles = Get-ChildItem -Path $root -Recurse -File -Include "*.c", "*.h"
    foreach ($rootFile in $rootFiles) {
        $null = $genericStaticFiles.Add($rootFile)
    }
}

$genericStaticPattern = '^\s*static\b[^;]*\b(s_ctx|s_state|s_data)\b'
foreach ($file in $genericStaticFiles) {
    $lineNumber = 0
    $lines = Get-Content -Path $file.FullName
    foreach ($line in $lines) {
        $lineNumber++
        if ($line -match $genericStaticPattern) {
            Add-ArchError("$($file.FullName):${lineNumber}: generic static symbol '$($matches[1])' is forbidden; include layer+feature in name")
        }
    }
}

# Rule 7: project layer must not include dependency repository paths directly.
$projectLayerFiles = Get-ChildItem -Path $featureRoot -Recurse -File -Include "ida_*.c", "ida_*.h", "prx_*.c", "prx_*.h", "poi_*.c", "poi_*.h"
foreach ($file in $projectLayerFiles) {
    $includes = Get-IncludeTokens -FilePath $file.FullName
    foreach ($inc in $includes) {
        if ($inc -match '(^|[\\/])deps([\\/]|$)') {
            Add-ArchError("$($file.FullName): project layer includes dependency path '$inc' directly")
        }
    }
}

# Rule 8: prx_ implementation files must document why PRX existence is required.
foreach ($file in $prxFiles) {
    $hasExistenceCondition = Select-String -Path $file.FullName -Pattern 'PRX_EXISTENCE_CONDITION\s*:' -Quiet
    if (-not $hasExistenceCondition) {
        Add-ArchError("$($file.FullName): prx_ file is missing 'PRX_EXISTENCE_CONDITION:' declaration")
    }
}

# Rule 9: layer-prefixed APIs must be consumed via headers, not local extern declarations.
$externScanFiles = @(
    Get-ChildItem -Path $featureRoot -Recurse -File -Include "ida_*.c", "prx_*.c", "poi_*.c"
)
if (Test-Path $coreRoot) {
    $externScanFiles += Get-ChildItem -Path $coreRoot -File -Include "ida_*.c", "prx_*.c", "poi_*.c"
}
$externApiPattern = '^\s*extern\s+[^;]*\b(Hal|Svc|Mdw|Stm|Db|Cfg|Ida|Prx|Poi)_[A-Za-z0-9_]+\s*\('
foreach ($file in $externScanFiles) {
    $lineNumber = 0
    $lines = Get-Content -Path $file.FullName
    foreach ($line in $lines) {
        $lineNumber++
        if ($line -match $externApiPattern) {
            Add-ArchError("$($file.FullName):${lineNumber}: local extern declaration for layer API '$($matches[1])_' is forbidden; include owning header")
        }
    }
}

if ($errors.Count -gt 0) {
    Write-Host "[arch-check] FAILED: $($errors.Count) violation(s) found."
    foreach ($err in $errors) {
        Write-Host "  - $err"
    }
    exit 1
}

Write-Host "[arch-check] PASSED"
Write-Host "  ida files checked : $($idaFiles.Count)"
Write-Host "  prx c files checked: $($prxFiles.Count)"
Write-Host "  prx h files checked: $($prxHeaders.Count)"
Write-Host "  poi c files checked: $($poiFiles.Count)"
Write-Host "  poi h files checked: $($poiHeaders.Count)"
exit 0

