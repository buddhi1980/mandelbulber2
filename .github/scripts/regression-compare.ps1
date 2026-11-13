<#
.SYNOPSIS
Mandelbulber regression test for CI on Windows.

.DESCRIPTION
Builds the commit before the change as well, renders the regression scenes
with both builds and compares them (mandelbulber2 --regression-render and
--regression-compare). The renders, the difference images and report.txt go
to .\regression in the repository.

The base is built like this commit: NuGet restore and MSBuild of
mandelbulber2.sln. Both programs run in their build folders, where they find
the scenes.

Exits with 0 when every scene passes or when the comparison is skipped, and
with 1 when a scene differs too much. It is skipped, with a message, when
there is no base commit, when the base has no regression test yet, or when
the base does not build.

.PARAMETER Base
The commit to compare with: the pull request's base or the previous commit of
a push. Default: the parent of HEAD.
#>
param(
	[string]$Base = '',
	[string]$Configuration = 'Release',
	[string]$Platform = 'x64'
)

function Skip([string]$reason) {
	if ($env:GITHUB_ACTIONS) {
		Write-Host "::notice::Regression comparison skipped: $reason"
	} else {
		Write-Host "Regression comparison skipped: $reason"
	}
	exit 0
}

# renders the scenes with the program in the build folder $dir into $out
function Render([string]$dir, [string]$out) {
	$exe = Join-Path $dir 'mandelbulber2.exe'
	if (-not (Test-Path $exe)) { return 2 }
	$p = Start-Process -FilePath $exe -WorkingDirectory $dir -Wait -PassThru -NoNewWindow `
		-ArgumentList @('--nogui', '--regression-render', "`"$out`"")
	return $p.ExitCode
}

$root = (git rev-parse --show-toplevel).Trim()
Set-Location $root

if (-not $Base -or $Base -match '^0+$') {
	# a shallow checkout has no parent yet
	if (Test-Path (Join-Path (git rev-parse --git-dir) 'shallow')) {
		git fetch -q --depth 2 origin (git rev-parse HEAD) 2>$null
	}
	$Base = git rev-parse -q --verify 'HEAD^' 2>$null
	if (-not $Base) { Skip 'this run has no base commit' }
}

git cat-file -e "$Base^{commit}" 2>$null
if ($LASTEXITCODE -ne 0) {
	git fetch -q --depth 1 origin $Base 2>$null
	if ($LASTEXITCODE -ne 0) { Skip "base commit $Base cannot be fetched" }
}
git cat-file -e "${Base}:mandelbulber2/src/regression_test.cpp" 2>$null
if ($LASTEXITCODE -ne 0) { Skip "base commit $Base has no regression test" }

Write-Host "Comparing with base commit $(git log -1 --format='%h %s' $Base)"
$baseDir = Join-Path (Split-Path $root -Parent) 'regression-base'
if (Test-Path $baseDir) { Remove-Item -Recurse -Force $baseDir }
git worktree prune
git worktree add --detach $baseDir $Base
if ($LASTEXITCODE -ne 0) { Skip "base commit $Base cannot be checked out" }

$msbuild = (Get-Command msbuild -ErrorAction SilentlyContinue).Source
if (-not $msbuild) {
	$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
	$msbuild = & $vswhere -products * -version '[18.0,19.0)' -requires Microsoft.Component.MSBuild `
		-find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
}
if (-not $msbuild) { Skip 'MSBuild not found' }

$solution = Join-Path $baseDir 'mandelbulber2\msvc\mandelbulber2.sln'
nuget restore $solution
if ($LASTEXITCODE -ne 0) { Skip 'the NuGet packages of the base commit cannot be restored' }
& $msbuild $solution /m "/p:Configuration=$Configuration" "/p:Platform=$Platform" /v:minimal /nologo
if ($LASTEXITCODE -ne 0) { Skip 'the base commit does not build' }

$out = Join-Path $root 'regression'
if (Test-Path $out) { Remove-Item -Recurse -Force $out }
New-Item -ItemType Directory -Force $out | Out-Null

# a scene that fails to render is reported by the comparison
$headDir = Join-Path $root "build-win\$Configuration\$Platform"
$baseBuildDir = Join-Path $baseDir "build-win\$Configuration\$Platform"
Write-Host "base render: exit $(Render $baseBuildDir (Join-Path $out 'before'))"
Write-Host "head render: exit $(Render $headDir (Join-Path $out 'after'))"

$exe = Join-Path $headDir 'mandelbulber2.exe'
$p = Start-Process -FilePath $exe -WorkingDirectory $headDir -Wait -PassThru -NoNewWindow `
	-ArgumentList @('--nogui', '--regression-compare', "`"$out\before`"", "`"$out\after`"",
		'--output', "`"$out\comparison`"")
$report = Join-Path $out 'comparison\report.txt'
if (Test-Path $report) { Get-Content $report }
exit $p.ExitCode
