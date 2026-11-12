# Summarizes the warnings of an MSBuild run for the GitHub Actions summary page.
#
# Reads <LogDir>\msbuild-warnings.log (MSBuild file logger, WarningsOnly), which
# holds compiler, linker and code analysis warnings. A header's warning repeats
# in every file that includes it, so each one is counted once per place and
# message. Writes <LogDir>\findings.csv with the complete list and appends
# tables by rule and by file, and the findings in the project's own source, to
# the job summary.

param(
	[Parameter(Mandatory = $true)][string]$LogDir,
	[Parameter(Mandatory = $true)][string]$Title,
	[string]$ArtifactName = 'build-logs',
	[string]$Root = $env:GITHUB_WORKSPACE,
	[string]$SummaryFile = $env:GITHUB_STEP_SUMMARY
)

$log = Join-Path $LogDir 'msbuild-warnings.log'
if (-not (Test-Path $log)) {
	Write-Host "No $log, the build did not start"
	exit 0
}

$rootPrefix = if ($Root) { $Root.TrimEnd('\') + '\' } else { $null }
$positioned = '^\s*(?:[\d:]+>)?(?<file>.+?)\((?<line>\d+)(?:,(?<col>\d+))?[^)]*\)\s*:\s*warning\s+(?<code>[A-Za-z]+\d+)\s*:\s*(?<msg>.*?)(?:\s+\[[^\]]+\])?\s*$'
$plain = '^\s*(?:[\d:]+>)?(?<file>.+?)\s*:\s*warning\s+(?<code>[A-Za-z]+\d+)\s*:\s*(?<msg>.*?)(?:\s+\[[^\]]+\])?\s*$'

$seen = @{}
$findings = foreach ($text in Get-Content $log -Encoding UTF8) {
	if ($text -match $positioned -or $text -match $plain) {
		$m = $Matches
		$file = $m['file'].Trim()
		if ($rootPrefix -and $file.StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase)) {
			$file = $file.Substring($rootPrefix.Length)
		}
		$key = "$file|$($m['line'])|$($m['col'])|$($m['code'])|$($m['msg'])"
		if (-not $seen.ContainsKey($key)) {
			$seen[$key] = $true
			# NuGet packages and anything outside the checkout is not the project's code
			$external = $file -match '\\packages\\' -or $file -match '^[A-Za-z]:\\'
			[pscustomobject]@{
				Code    = $m['code'].ToUpper()
				Origin  = if ($external) { 'external' } else { 'project' }
				File    = $file
				Line    = if ($m['line']) { [int]$m['line'] } else { 0 }
				Column  = $m['col']
				Message = $m['msg']
			}
		}
	}
}
$findings = @($findings)
$findings | Sort-Object File, Line, Code |
	Export-Csv (Join-Path $LogDir 'findings.csv') -NoTypeInformation -Encoding utf8

function Cell($value) { "$value".Replace('|', '\|').Replace('<', '&lt;').Replace('>', '&gt;') }

$own = @($findings | Where-Object Origin -eq 'project')
$md = [System.Collections.Generic.List[string]]::new()
$md.Add("## ${Title}: $($findings.Count) findings")
$md.Add('')
$md.Add("$($own.Count) are in the project's own source and $($findings.Count - $own.Count) in external headers. " +
	"The complete list is ``findings.csv`` in the ``$ArtifactName`` artifact, next to the full build log.")

if ($findings.Count -gt 0) {
	$md.Add('')
	$md.Add('### By rule')
	$md.Add('')
	$md.Add('| Rule | Findings | In project source | Example |')
	$md.Add('| --- | ---: | ---: | --- |')
	foreach ($group in $findings | Group-Object Code | Sort-Object Count -Descending) {
		$inProject = @($group.Group | Where-Object Origin -eq 'project').Count
		$example = $group.Group[0].Message
		if ($example.Length -gt 160) { $example = $example.Substring(0, 160) + '...' }
		$md.Add("| $($group.Name) | $($group.Count) | $inProject | $(Cell $example) |")
	}

	$md.Add('')
	$md.Add('### By file (top 40)')
	$md.Add('')
	$md.Add('| File | Findings | Rules |')
	$md.Add('| --- | ---: | --- |')
	foreach ($group in $findings | Group-Object File | Sort-Object Count -Descending | Select-Object -First 40) {
		$rules = ($group.Group | Group-Object Code | Sort-Object Count -Descending | Select-Object -First 6 |
			ForEach-Object { "$($_.Name) x$($_.Count)" }) -join ', '
		$md.Add("| $(Cell $group.Name) | $($group.Count) | $rules |")
	}

	# every finding in the project's own source, as far as the 1 MB summary limit allows
	$md.Add('')
	$md.Add('<details><summary>Findings in project source</summary>')
	$md.Add('')
	$md.Add('| File | Line | Rule | Message |')
	$md.Add('| --- | ---: | --- | --- |')
	$size = ($md | Measure-Object -Property Length -Sum).Sum
	$shown = 0
	foreach ($f in $own | Sort-Object File, Line, Code) {
		$row = "| $(Cell $f.File) | $($f.Line) | $($f.Code) | $(Cell $f.Message) |"
		if ($size + $row.Length -gt 800000) { break }
		$md.Add($row)
		$size += $row.Length + 1
		$shown++
	}
	$md.Add('')
	if ($shown -lt $own.Count) {
		$md.Add("Showing $shown of $($own.Count); the rest are in ``findings.csv``.")
		$md.Add('')
	}
	$md.Add('</details>')
}

if ($SummaryFile) {
	$md -join "`n" | Out-File -FilePath $SummaryFile -Encoding utf8 -Append
}
Write-Host "$($findings.Count) findings, $($own.Count) in project source"
$findings | Group-Object Code | Sort-Object Count -Descending | Format-Table Name, Count -AutoSize
