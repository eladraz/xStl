<#
.SYNOPSIS
  Measures Doxygen documentation coverage from the XML produced with EXTRACT_ALL=YES.

.DESCRIPTION
  A public API symbol is: a class/struct/union, or a public member of one, or a
  file-level define/typedef/enum/function/variable (plus enum values).
  "Documented" means the symbol has a non-empty brief or detailed description.

.PARAMETER XmlDir   Doxygen XML output directory.
.PARAMETER Json     Optional path to write the per-symbol records as JSON.
#>
param(
    [Parameter(Mandatory = $true)][string]$XmlDir,
    [string]$Json
)
$ErrorActionPreference = 'Stop'

function Get-Text($node) {
    if ($null -eq $node) { return '' }
    return ($node.InnerText -replace '\s+', ' ').Trim()
}
function Norm-Path([string]$f) { $f = $f -replace '\\\\', '/'; if ($f -match '(^|/)(Include/)?(xStl/.*)$') { return 'Include/' + $Matches[3] }; return $f }
function Get-Module($file) {
    if ($file -match 'Include/xStl/([^/]+)/') { return $Matches[1] }
    return 'core'
}

$records = New-Object System.Collections.Generic.List[object]

function Add-Member($m, $compoundName, $compoundKind) {
    $kind = $m.GetAttribute('kind')
    if ($kind -in @('friend', 'signal', 'slot', 'event', 'property', 'dcop')) { return }
    $prot = $m.GetAttribute('prot')
    if ($compoundKind -in @('class', 'struct', 'union') -and $prot -ne 'public') { return }
    $name = $m.SelectSingleNode('name').InnerText
    $loc = $m.SelectSingleNode('location')
    $file = (Norm-Path $loc.GetAttribute('file'))
    if ($file -notmatch 'Include/xStl/') { return }
    if ($kind -eq 'define' -and $name -match '^_*[A-Z0-9_]+_H$') { return }   # include guard
    $brief = Get-Text ($m.SelectSingleNode('briefdescription'))
    $dd = $m.SelectSingleNode('detaileddescription'); $detail = Get-Text $dd
    $documented = ($brief -ne '') -or ($detail -ne '')

    $declared = @(); $documentedParams = @()
    if ($kind -eq 'function') {
        foreach ($p in @($m.SelectNodes('param'))) {
            $n = ($p.SelectSingleNode('declname')).InnerText; if (-not $n) { $n = ($p.SelectSingleNode('defname')).InnerText }
            if ($n) { $declared += $n }
        }
        foreach ($pl in @($dd.SelectNodes('.//parameterlist[@kind="param"]/parameteritem/parameternamelist/parametername'))) {
            $documentedParams += ($pl.InnerText -split ',') | ForEach-Object { $_.Trim() }
        }
    }
    $tdecl = @(); $tdoc = @()
    foreach ($tp in @($m.SelectNodes('templateparamlist/param'))) {
        $n = ($tp.SelectSingleNode('declname')).InnerText; if (-not $n) { $n = (($tp.SelectSingleNode('type')).InnerText -replace '^(class|typename)\s+', '') }
        if ($n) { $tdecl += $n }
    }
    foreach ($pl in @($dd.SelectNodes('.//parameterlist[@kind="templateparam"]/parameteritem/parameternamelist/parametername'))) {
        $tdoc += ($pl.InnerText -split ',') | ForEach-Object { $_.Trim() }
    }
    $needsReturn = $false
    if ($kind -eq 'function') {
        $type = Get-Text ($m.SelectSingleNode('type'))
        $isCtor = ($type -eq '') -or ($name -like "~*") -or ($name -like 'operator *')
        $needsReturn = (-not $isCtor) -and ($type -notmatch '^(virtual\s+)?(static\s+)?(inline\s+)?void$')
    }
    $hasReturn = ($null -ne $dd.SelectSingleNode('.//simplesect[@kind="return"]'))

    $qual = if ($compoundKind -in @('class', 'struct', 'union')) { "$compoundName::$name" } else { $name }
    $qual = $qual -replace '@\d+', '@anon'   # anonymous enum ids are not stable
    if ($kind -eq 'function') { $qual = $qual + (Get-Text ($m.SelectSingleNode('argsstring'))) }   # keep overloads apart
    $records.Add([pscustomobject]@{
        Kind = $kind; Name = $qual; File = $file; Module = (Get-Module $file)
        Documented = $documented
        Params = $declared.Count
        ParamsDocumented = @($declared | Where-Object { $documentedParams -contains $_ }).Count
        TParams = $tdecl.Count
        TParamsDocumented = @($tdecl | Where-Object { $tdoc -contains $_ }).Count
        NeedsReturn = $needsReturn; HasReturn = $hasReturn
    })
    if ($kind -eq 'enum') {
        foreach ($ev in @($m.SelectNodes('enumvalue'))) {
            $b = Get-Text ($ev.SelectSingleNode('briefdescription')); $d = Get-Text ($ev.SelectSingleNode('detaileddescription'))
            $records.Add([pscustomobject]@{
                Kind = 'enumvalue'; Name = "$qual::$($ev.SelectSingleNode('name').InnerText)"; File = $file; Module = (Get-Module $file)
                Documented = (($b -ne '') -or ($d -ne ''))
                Params = 0; ParamsDocumented = 0; TParams = 0; TParamsDocumented = 0
                NeedsReturn = $false; HasReturn = $false
            })
        }
    }
}

foreach ($f in Get-ChildItem $XmlDir -Filter *.xml) {
    if ($f.Name -in @('index.xml', 'Doxyfile.xml') -or $f.Name -like 'combine*') { continue }
    [xml]$x = Get-Content $f.FullName -Raw
    $c = $x.SelectSingleNode('doxygen/compounddef')
    if ($null -eq $c) { continue }
    $ck = $c.GetAttribute('kind')
    if ($ck -notin @('class', 'struct', 'union', 'file', 'group')) { continue }
    if ($ck -in @('class', 'struct', 'union')) {
        if ($c.GetAttribute('prot') -ne 'public') { continue }
        $loc = $c.SelectSingleNode('location')
        $file = (Norm-Path $loc.GetAttribute('file'))
        if ($file -notmatch 'Include/xStl/') { continue }
        $b = Get-Text ($c.SelectSingleNode('briefdescription')); $cdd = $c.SelectSingleNode('detaileddescription'); $d = Get-Text $cdd
        $tdecl = @(); $tdoc = @()
        foreach ($tp in @($c.SelectNodes('templateparamlist/param'))) {
            $n = ($tp.SelectSingleNode('declname')).InnerText; if (-not $n) { $n = (($tp.SelectSingleNode('type')).InnerText -replace '^(class|typename)\s+', '') }
            if ($n) { $tdecl += $n }
        }
        foreach ($pl in @($cdd.SelectNodes('.//parameterlist[@kind="templateparam"]/parameteritem/parameternamelist/parametername'))) {
            $tdoc += ($pl.InnerText -split ',') | ForEach-Object { $_.Trim() }
        }
        $records.Add([pscustomobject]@{
            Kind = $ck; Name = $c.SelectSingleNode('compoundname').InnerText; File = $file; Module = (Get-Module $file)
            Documented = (($b -ne '') -or ($d -ne ''))
            Params = 0; ParamsDocumented = 0
            TParams = $tdecl.Count; TParamsDocumented = @($tdecl | Where-Object { $tdoc -contains $_ }).Count
            NeedsReturn = $false; HasReturn = $false
        })
        foreach ($s in @($c.SelectNodes('sectiondef'))) { foreach ($m in @($s.SelectNodes('memberdef'))) { Add-Member $m $c.SelectSingleNode('compoundname').InnerText $ck } }
    }
    else {
        foreach ($s in @($c.SelectNodes('sectiondef'))) { foreach ($m in @($s.SelectNodes('memberdef'))) { Add-Member $m $c.compoundname 'file' } }
    }
}

# de-duplicate (same symbol seen from several compounds)
$uniq = $records | Sort-Object Kind, Name, File -Unique

if ($Json) { $uniq | ConvertTo-Json -Depth 4 | Set-Content $Json -Encoding utf8 }

$total = @($uniq).Count
$doc = @($uniq | Where-Object Documented).Count
$funcs = @($uniq | Where-Object { $_.Kind -eq 'function' })
$pTotal = ($funcs | Measure-Object Params -Sum).Sum
$pDoc = ($funcs | Measure-Object ParamsDocumented -Sum).Sum
$tp = @($uniq | Where-Object { $_.TParams -gt 0 })
$tpTotal = ($tp | Measure-Object TParams -Sum).Sum
$tpDoc = ($tp | Measure-Object TParamsDocumented -Sum).Sum
$needRet = @($funcs | Where-Object NeedsReturn)
$retDoc = @($needRet | Where-Object HasReturn).Count

[pscustomobject]@{
    Symbols = $total; Documented = $doc
    CoveragePct = [math]::Round(100.0 * $doc / [math]::Max(1, $total), 1)
    Params = $pTotal; ParamsDocumented = $pDoc; ParamsMissing = ($pTotal - $pDoc)
    TParams = $tpTotal; TParamsDocumented = $tpDoc
    ReturnsNeeded = $needRet.Count; ReturnsDocumented = $retDoc
} | Format-List

"By symbol kind:"
$uniq | Group-Object Kind | Sort-Object Name | ForEach-Object {
    $d = @($_.Group | Where-Object Documented).Count
    "{0,-10} {1,5} symbols, {2,5} documented ({3,5:N1}%)" -f $_.Name, $_.Count, $d, (100.0 * $d / $_.Count)
}
""
"By module:"
$uniq | Group-Object Module | Sort-Object Name | ForEach-Object {
    $d = @($_.Group | Where-Object Documented).Count
    "{0,-10} {1,5} symbols, {2,5} documented ({3,5:N1}%)" -f $_.Name, $_.Count, $d, (100.0 * $d / $_.Count)
}





