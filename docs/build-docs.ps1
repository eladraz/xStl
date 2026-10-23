<#
.SYNOPSIS
  Reproducible Doxygen pipeline for xStl (Windows PowerShell equivalent of docs/Makefile).

.DESCRIPTION
  Targets (same names as the Makefile):
    docs-html   Generate the HTML documentation  -> docs/build/html
    docs-xml    Generate the XML documentation   -> docs/build/xml  (for Sphinx/Breathe, doxybook, ...)
    docs-api    HTML + XML in a single Doxygen run (the "production" documentation set)
    docs-check  Audit with EXTRACT_ALL=NO; fails when Doxygen reports any warning
    docs-pdf    PDF reference manual (LaTeX): docs/build/xStl-reference.pdf. Needs pdflatex + makeindex (MiKTeX / TeX Live)
    docs-metrics Print API documentation coverage measured from the XML output
    docs-examples Compile and run docs/examples against the xStl library (needs MSVC + a built xStl.lib)
    docs-all    docs-check, docs-api, docs-metrics
    docs-clean  Remove docs/build

.PARAMETER Target  One of the targets above (default docs-all).
.PARAMETER Doxygen Path of doxygen.exe (default: first doxygen found on PATH or in %ProgramFiles%\doxygen\bin).
#>
param(
    [ValidateSet('docs-html', 'docs-xml', 'docs-api', 'docs-check', 'docs-pdf', 'docs-metrics', 'docs-examples', 'docs-all', 'docs-clean')]
    [string]$Target = 'docs-all',
    [string]$Doxygen
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot\..").Path
Set-Location $repo

if (-not $Doxygen) {
    $cmd = Get-Command doxygen -ErrorAction SilentlyContinue
    if ($cmd) { $Doxygen = $cmd.Source }
    elseif (Test-Path "$env:ProgramFiles\doxygen\bin\doxygen.exe") { $Doxygen = "$env:ProgramFiles\doxygen\bin\doxygen.exe" }
    else { throw 'doxygen was not found. Install it (winget install DimitriVanHeesch.Doxygen) or pass -Doxygen.' }
}

# Runs doxygen on a configuration file with extra "KEY = VALUE" overrides appended (read from stdin).
function Invoke-Doxygen([string]$Config, [string[]]$Overrides = @()) {
    # Appends the overrides to a copy of the configuration (UTF-8 without BOM) and runs doxygen on it.
    New-Item -ItemType Directory -Force docs\build | Out-Null
    $tmp = Join-Path $repo 'docs\build\Doxyfile.tmp'
    $text = (Get-Content $Config -Raw) + "`n" + ($Overrides -join "`n") + "`n"
    [IO.File]::WriteAllText($tmp, $text, (New-Object System.Text.UTF8Encoding($false)))
    & $Doxygen $tmp
    if ($LASTEXITCODE -ne 0) { throw "doxygen failed with exit code $LASTEXITCODE" }
}
function Clean-Dir([string]$Path) { if (Test-Path $Path) { Remove-Item $Path -Recurse -Force } }

switch ($Target) {
    'docs-html' { Clean-Dir docs\build\html; Invoke-Doxygen docs\Doxyfile @('GENERATE_XML = NO'); Write-Host 'HTML: docs\build\html\index.html' }
    'docs-xml'  { Clean-Dir docs\build\xml;  Invoke-Doxygen docs\Doxyfile @('GENERATE_HTML = NO'); Write-Host 'XML: docs\build\xml\index.xml' }
    'docs-api'  { Clean-Dir docs\build\html; Clean-Dir docs\build\xml; Invoke-Doxygen docs\Doxyfile; Write-Host 'HTML: docs\build\html\index.html   XML: docs\build\xml\index.xml' }
    'docs-check' {
        Clean-Dir docs\build\check
        Invoke-Doxygen docs\Doxyfile.check @('WARN_AS_ERROR = NO')
        $log = 'docs\build\check\doxygen-warnings.log'
        $warnings = @(Get-Content $log | Where-Object { $_.Trim() })
        if ($warnings.Count -gt 0) { $warnings | Select-Object -First 50; throw "docs-check failed: $($warnings.Count) Doxygen warning(s) in $log" }
        Write-Host 'docs-check passed: 0 Doxygen warnings with EXTRACT_ALL=NO'
    }
    'docs-pdf' {
        foreach ($tool in 'pdflatex', 'makeindex') { if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) { throw "docs-pdf needs '$tool' on PATH (install MiKTeX or TeX Live)." } }
        Clean-Dir docs\build\latex
        Invoke-Doxygen docs\Doxyfile @('GENERATE_HTML = NO', 'GENERATE_XML = NO', 'GENERATE_LATEX = YES', 'LATEX_OUTPUT = latex', 'USE_PDFLATEX = YES', 'PDF_HYPERLINKS = YES', 'LATEX_BATCHMODE = YES', 'COMPACT_LATEX = YES')
        Push-Location docs\build\latex
        try { cmd /c ".\make.bat"; if ($LASTEXITCODE -ne 0) { throw "LaTeX build failed (exit code $LASTEXITCODE); see docs\build\latex\refman.log" } } finally { Pop-Location }
        Copy-Item docs\build\latex\refman.pdf docs\build\xStl-reference.pdf -Force
        Write-Host 'PDF: docs\build\xStl-reference.pdf'
    }
    'docs-metrics' { if (-not (Test-Path docs\build\xml\index.xml)) { throw 'Run docs-api (or docs-xml) first.' }; & "$PSScriptRoot\scripts\doc-metrics.ps1" -XmlDir docs\build\xml }
    'docs-examples' { & "$PSScriptRoot\examples\build-examples.ps1" }
    'docs-all' {
        & $PSCommandPath -Target docs-check -Doxygen $Doxygen
        & $PSCommandPath -Target docs-api -Doxygen $Doxygen
        & $PSCommandPath -Target docs-metrics -Doxygen $Doxygen
    }
    'docs-clean' { Clean-Dir docs\build }
}

