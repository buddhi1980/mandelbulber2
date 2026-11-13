<#
.SYNOPSIS
Renders one shard of the regression scenes with two builds and compares them, on Windows.

.DESCRIPTION
Each program runs in its build folder, where it finds its libraries and the examples.
Writes <Out>\before, after and comparison (with report.txt). Exits with 0 when every scene
of the shard passes, and 1 when one differs.
#>
param(
	[Parameter(Mandatory = $true)][string]$Base,
	[Parameter(Mandatory = $true)][string]$Head,
	[Parameter(Mandatory = $true)][string]$Shard,
	[Parameter(Mandatory = $true)][string]$Out
)

New-Item -ItemType Directory -Force $Out | Out-Null
$Out = (Resolve-Path $Out).Path

function Run([string]$dir, [string[]]$arguments) {
	$dir = (Resolve-Path $dir).Path
	$quoted = $arguments | ForEach-Object { if ($_ -match '\s') { "`"$_`"" } else { $_ } }
	$p = Start-Process -FilePath (Join-Path $dir 'mandelbulber2.exe') -WorkingDirectory $dir `
		-ArgumentList $quoted -Wait -PassThru -NoNewWindow
	return $p.ExitCode
}

Write-Host "Shard ${Shard}: rendering with the base build"
Write-Host "exit $(Run $Base @('--nogui', '--regression-shard', $Shard, '--regression-render', "$Out\before"))"
Write-Host "Shard ${Shard}: rendering with the head build"
Write-Host "exit $(Run $Head @('--nogui', '--regression-shard', $Shard, '--regression-render', "$Out\after"))"
Write-Host "Shard ${Shard}: comparing"
$result = Run $Head @('--nogui', '--regression-shard', $Shard, '--regression-compare',
	"$Out\before", "$Out\after", '--output', "$Out\comparison")
$report = Join-Path $Out 'comparison\report.txt'
if (Test-Path $report) { Get-Content $report | Select-String -Pattern '^\[FAIL\]|failures:|^RESULT' }
exit $result
