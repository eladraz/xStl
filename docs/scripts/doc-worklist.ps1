<#
.SYNOPSIS
  Lists every public API symbol whose documentation is missing or incomplete.

.DESCRIPTION
  Reads the Doxygen XML (generated with EXTRACT_ALL=YES) and prints, per header,
  the declaration line, signature and what is missing (brief, @param names,
  @return, @tparam). Used to drive and to verify the documentation work.

.PARAMETER XmlDir  Doxygen XML directory.
.PARAMETER Module  Optional module (directory under Include/xStl) to restrict the output to.
.PARAMETER Out     Optional output file (default: console).
#>
param(
    [Parameter(Mandatory = $true)][string]$XmlDir,
    [string]$Module,
    [string]$Out
)
function Get-Text($node) { if ($null -eq $node) { return '' }; return ($node.InnerText -replace '\s+', ' ').Trim() }
function Norm-Path([string]$f) { $f = $f -replace '\\\\', '/'; if ($f -match '(^|/)(Include/)?(xStl/.*)$') { return 'Include/' + $Matches[3] }; return $f }
function Get-Mod($file) { if ($file -match 'Include/xStl/([^/]+)/') { return $Matches[1] }; return 'core' }

$items = New-Object System.Collections.Generic.List[object]

function Check-Member($m, $owner, $ownerKind) {
    $kind = $m.GetAttribute('kind')
    if ($kind -in @('friend', 'signal', 'slot', 'event', 'property', 'dcop')) { return }
    if ($ownerKind -in @('class', 'struct', 'union') -and $m.GetAttribute('prot') -ne 'public') { return }
    $loc = $m.SelectSingleNode('location'); $file = Norm-Path $loc.GetAttribute('file'); $line = [int]$loc.GetAttribute('line')
    if ($file -notmatch 'Include/xStl/') { return }
    $name = $m.SelectSingleNode('name').InnerText
    $brief = Get-Text ($m.SelectSingleNode('briefdescription')); $dd = $m.SelectSingleNode('detaileddescription'); $detail = Get-Text $dd
    $documented = ($brief -ne '') -or ($detail -ne '')
    $missing = New-Object System.Collections.Generic.List[string]
    if (-not $documented) { $missing.Add('BRIEF') }
    $sig = ''
    if ($kind -eq 'function') {
        $sig = (Get-Text ($m.SelectSingleNode('definition'))) + (Get-Text ($m.SelectSingleNode('argsstring')))
        $declared = @(); foreach ($p in @($m.SelectNodes('param'))) { $n = ($p.SelectSingleNode('declname')).InnerText; if (-not $n) { $n = ($p.SelectSingleNode('defname')).InnerText }; if ($n) { $declared += $n } }
        $docd = @(); foreach ($pn in @($dd.SelectNodes('.//parameterlist[@kind="param"]/parameteritem/parameternamelist/parametername'))) { $docd += ($pn.InnerText -split ',') | ForEach-Object { $_.Trim() } }
        $mp = @($declared | Where-Object { $docd -notcontains $_ })
        if ($mp.Count -gt 0) { $missing.Add('PARAM[' + ($mp -join ',') + ']') }
        $type = Get-Text ($m.SelectSingleNode('type'))
        $isCtor = ($type -eq '') -or ($name -like '~*')
        $needsRet = (-not $isCtor) -and ($type -notmatch '^(virtual\s+)?(static\s+)?(inline\s+)?void$')
        if ($needsRet -and $null -eq $dd.SelectSingleNode('.//simplesect[@kind="return"]')) { $missing.Add('RETURN') }
        $tpd = @(); foreach ($tp in @($m.SelectNodes('templateparamlist/param'))) { $n = ($tp.SelectSingleNode('declname')).InnerText; if (-not $n) { $n = (($tp.SelectSingleNode('type')).InnerText -replace '^(class|typename)\s+', '') }; if ($n) { $tpd += $n } }
        $tdoc = @(); foreach ($pn in @($dd.SelectNodes('.//parameterlist[@kind="templateparam"]/parameteritem/parameternamelist/parametername'))) { $tdoc += ($pn.InnerText -split ',') | ForEach-Object { $_.Trim() } }
        $mt = @($tpd | Where-Object { $tdoc -notcontains $_ })
        if ($mt.Count -gt 0) { $missing.Add('TPARAM[' + ($mt -join ',') + ']') }
    }
    elseif ($kind -eq 'define') {
        $declared = @(); foreach ($p in @($m.SelectNodes('param'))) { $n = ($p.SelectSingleNode('defname')).InnerText; if ($n) { $declared += $n } }
        $sig = "#define $name" + $(if ($declared.Count) { '(' + ($declared -join ', ') + ')' } else { '' })
        if ($name -match '^_*[A-Z0-9_]+_H$') { return }   # include guard
        if ($documented) {
            $docd = @(); foreach ($pn in @($dd.SelectNodes('.//parameterlist[@kind="param"]/parameteritem/parameternamelist/parametername'))) { $docd += ($pn.InnerText -split ',') | ForEach-Object { $_.Trim() } }
            $mp = @($declared | Where-Object { $docd -notcontains $_ })
            if ($mp.Count -gt 0) { $missing.Add('PARAM[' + ($mp -join ',') + ']') }
        }
    }
    else { $sig = (Get-Text ($m.SelectSingleNode('definition'))) }
    if ($missing.Count -gt 0) {
        $items.Add([pscustomobject]@{ File = $file; Line = $line; Kind = $kind; Owner = $owner; Sig = $sig; Missing = ($missing -join ' '); Brief = $brief })
    }
    if ($kind -eq 'enum') {
        foreach ($ev in @($m.SelectNodes('enumvalue'))) {
            $b = Get-Text ($ev.SelectSingleNode('briefdescription')); $d = Get-Text ($ev.SelectSingleNode('detaileddescription'))
            if (($b -eq '') -and ($d -eq '')) { $items.Add([pscustomobject]@{ File = $file; Line = $line; Kind = 'enumvalue'; Owner = $name; Sig = $ev.SelectSingleNode('name').InnerText; Missing = 'BRIEF'; Brief = '' }) }
        }
    }
}

foreach ($f in Get-ChildItem $XmlDir -Filter *.xml) {
    if ($f.Name -in @('index.xml', 'Doxyfile.xml') -or $f.Name -like 'combine*') { continue }
    [xml]$x = Get-Content $f.FullName -Raw
    $c = $x.SelectSingleNode('doxygen/compounddef'); if ($null -eq $c) { continue }
    $ck = $c.GetAttribute('kind')
    if ($ck -notin @('class', 'struct', 'union', 'file', 'group')) { continue }
    if ($ck -in @('file', 'group')) {
        foreach ($s in @($c.SelectNodes('sectiondef'))) { foreach ($m in @($s.SelectNodes('memberdef'))) { Check-Member $m '' 'file' } }
    }
    else {
        if ($c.GetAttribute('prot') -ne 'public') { continue }
        $loc = $c.SelectSingleNode('location'); $file = Norm-Path $loc.GetAttribute('file')
        if ($file -notmatch 'Include/xStl/') { continue }
        $cn = $c.SelectSingleNode('compoundname').InnerText
        $b = Get-Text ($c.SelectSingleNode('briefdescription')); $d = Get-Text ($c.SelectSingleNode('detaileddescription'))
        $miss = New-Object System.Collections.Generic.List[string]
        if (($b -eq '') -and ($d -eq '')) { $miss.Add('BRIEF') }
        $tpd = @(); foreach ($tp in @($c.SelectNodes('templateparamlist/param'))) { $n = ($tp.SelectSingleNode('declname')).InnerText; if (-not $n) { $n = (($tp.SelectSingleNode('type')).InnerText -replace '^(class|typename)\s+', '') }; if ($n) { $tpd += $n } }
        $tdoc = @(); foreach ($pn in @($c.SelectNodes('detaileddescription//parameterlist[@kind="templateparam"]/parameteritem/parameternamelist/parametername'))) { $tdoc += ($pn.InnerText -split ',') | ForEach-Object { $_.Trim() } }
        $mt = @($tpd | Where-Object { $tdoc -notcontains $_ }); if ($mt.Count -gt 0) { $miss.Add('TPARAM[' + ($mt -join ',') + ']') }
        if ($miss.Count -gt 0) { $items.Add([pscustomobject]@{ File = $file; Line = [int]$loc.GetAttribute('line'); Kind = $ck; Owner = ''; Sig = $cn; Missing = ($miss -join ' '); Brief = $b }) }
        foreach ($s in @($c.SelectNodes('sectiondef'))) { foreach ($m in @($s.SelectNodes('memberdef'))) { Check-Member $m $cn $ck } }
    }
}

$sel = $items | Where-Object { (-not $Module) -or ((Get-Mod $_.File) -eq $Module) } | Sort-Object File, Line
$lines = New-Object System.Collections.Generic.List[string]
foreach ($g in ($sel | Group-Object File)) {
    $lines.Add("## $($g.Name)")
    foreach ($i in $g.Group) {
        $b = if ($i.Brief) { ' || ' + $i.Brief.Substring(0, [Math]::Min(70, $i.Brief.Length)) } else { '' }
        $lines.Add(("L{0} {1} {2} -> {3}{4}" -f $i.Line, $i.Kind, $i.Sig, $i.Missing, $b))
    }
}
$lines.Add("TOTAL: $(@($sel).Count)")
if ($Out) { $lines | Set-Content $Out -Encoding utf8 } else { $lines }



