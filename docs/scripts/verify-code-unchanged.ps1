<#
.SYNOPSIS
  Proves that only comments (and optionally string literals) changed relative to a git revision.

.DESCRIPTION
  For every changed C/C++ source file, strips comments and string literals from the
  version at -BaseRev and from the working tree and compares what is left. Any
  difference means executable code, a declaration or a preprocessor line changed.

.PARAMETER BaseRev  Git revision to compare against (default HEAD).
#>
param([string]$BaseRev = 'HEAD')
$enc = [Text.Encoding]::GetEncoding(28591)
$tok = [regex]'(?s)//[^\r\n]*|/\*.*?\*/|"(?:\\.|[^"\\\r\n])*"'
$ev = { param($m) if ($m.Value.StartsWith('"')) { '""' } else { ' ' } }
$bad = @(); $checked = 0
foreach ($f in (git diff --name-only $BaseRev | Where-Object { $_ -match '\.(cpp|h|inl)$' })) {
    $tmp = Join-Path $env:TEMP ('old_' + [IO.Path]::GetFileName($f))
    cmd /c "git show ${BaseRev}:$f > `"$tmp`"" | Out-Null
    $old = ($enc.GetString([IO.File]::ReadAllBytes($tmp))) -replace "`r`n", "`n"
    $new = ($enc.GetString([IO.File]::ReadAllBytes((Resolve-Path $f)))) -replace "`r`n", "`n"
    $so = $tok.Replace($old, [Text.RegularExpressions.MatchEvaluator]$ev)
    $sn = $tok.Replace($new, [Text.RegularExpressions.MatchEvaluator]$ev)
    # whitespace-only differences (blank lines around inserted comments) are not code changes
    $so = ($so -replace '\s+', ' ').Trim(); $sn = ($sn -replace '\s+', ' ').Trim()
    $checked++
    if ($so -cne $sn) { $bad += $f }
    Remove-Item $tmp -ErrorAction SilentlyContinue
}
"checked $checked source files; files whose code outside comments/strings differs: $($bad.Count)"
$bad
if ($bad.Count -gt 0) { exit 1 }
