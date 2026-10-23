<#
.SYNOPSIS
  Compiles and runs the Doxygen examples against the xStl static library (MSVC, x64).

.PARAMETER Config  Debug or Release (default Release). The matching xStl.lib must be built already
                   (out\lib<Config>64\xStl\xStl.lib).
#>
param([ValidateSet('Debug', 'Release')][string]$Config = 'Release')
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot\..\..").Path
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$bat = "$vs\VC\Auxiliary\Build\vcvars64.bat"
$out = Join-Path $repo "docs\build\examples"
New-Item -ItemType Directory -Force $out | Out-Null
$lib = "$repo\out\lib${Config}64\xStl\xStl.lib"
if (-not (Test-Path $lib)) { throw "Missing $lib - build the xStl library first." }
$rt = if ($Config -eq 'Debug') { '/MTd /D_DEBUG' } else { '/MT /DNDEBUG' }
$failed = 0
foreach ($src in Get-ChildItem "$PSScriptRoot\*.cpp") {
    $exe = Join-Path $out ($src.BaseName + '.exe')
    $cmd = "`"$bat`" >nul 2>nul && cl /nologo /EHsc /W3 $rt /DWIN32 /D_CONSOLE /DXSTL_UNICODE /DUNICODE /I`"$repo\Include`" /Fo`"$out\\`" /Fe`"$exe`" `"$($src.FullName)`" `"$lib`" ws2_32.lib >`"$out\$($src.BaseName).log`" 2>&1"
    cmd /c $cmd
    if ($LASTEXITCODE -ne 0) { Write-Host "FAIL (compile) $($src.Name)"; Get-Content "$out\$($src.BaseName).log" | Select-Object -Last 15; $failed++; continue }
    Write-Host "== $($src.Name)"
    & $exe
    if ($LASTEXITCODE -ne 0) { Write-Host "FAIL (run, exit $LASTEXITCODE) $($src.Name)"; $failed++ }
}
if ($failed) { throw "$failed example(s) failed" }
Write-Host "All examples compiled and ran successfully."

