<#
.SYNOPSIS
  Classifies a Doxygen warning log into categories and prints the counts.

.PARAMETER Log   Path of the Doxygen WARN_LOGFILE.
.PARAMETER Detail  Also print every warning that is not a plain "not documented" warning.
#>
param(
    [Parameter(Mandatory = $true)][string]$Log,
    [switch]$Detail
)
$counts = [ordered]@{}
function Add-Count([string]$k) { if ($counts.Contains($k)) { $counts[$k] = $counts[$k] + 1 } else { $counts[$k] = 1 } }

$other = New-Object System.Collections.Generic.List[string]
$lines = if (Test-Path $Log) { Get-Content $Log } else { @() }
foreach ($l in $lines) {
    if ([string]::IsNullOrWhiteSpace($l)) { continue }
    if ($l -match 'warning: Compound .* is not documented') { Add-Count 'undocumented: compound (class/struct/union)' }
    elseif ($l -match 'warning: Member .* \(function\) .* is not documented') { Add-Count 'undocumented: function' }
    elseif ($l -match 'warning: Member .* \(variable\) .* is not documented') { Add-Count 'undocumented: variable' }
    elseif ($l -match 'warning: Member .* \(typedef\) .* is not documented') { Add-Count 'undocumented: typedef' }
    elseif ($l -match 'warning: Member .* \(define\) .* is not documented') { Add-Count 'undocumented: macro' }
    elseif ($l -match 'warning: Member .* \(enumeration\) .* is not documented') { Add-Count 'undocumented: enum' }
    elseif ($l -match 'warning: Member .* \(enumvalue\) .* is not documented') { Add-Count 'undocumented: enum value' }
    elseif ($l -match 'warning: Member .* is not documented') { Add-Count 'undocumented: other member' }
    elseif ($l -match 'warning: File .* is not documented') { Add-Count 'undocumented: file' }
    elseif ($l -match 'warning: parameters of member .* are not (\(all\) )?documented') { Add-Count 'parameters not (all) documented' }
    elseif ($l -match 'warning: return type of member .* is not documented') { Add-Count 'return type not documented' }
    elseif ($l -match 'warning: unable to resolve reference') { Add-Count 'BROKEN REFERENCE' }
    elseif ($l -match 'warning: .*(Unsupported xml/html tag|unsupported|Unknown command|unknown command|explicit link request)') { Add-Count 'markup problem (tag/command)' }
    elseif ($l -match 'warning: The following parameter|warning: argument .* of command @param|warning: .*\\param|warning: .*@param|warning: .*template parameter') { Add-Count 'param/tparam mismatch' }
    else { Add-Count 'other'; $other.Add($l) }
}
$total = ($counts.Values | Measure-Object -Sum).Sum
$counts.GetEnumerator() | ForEach-Object { "{0,6}  {1}" -f $_.Value, $_.Key }
"{0,6}  TOTAL" -f [int]$total
if ($Detail -and $other.Count -gt 0) { ""; "Uncategorised warnings:"; $other | ForEach-Object { $_ } }

