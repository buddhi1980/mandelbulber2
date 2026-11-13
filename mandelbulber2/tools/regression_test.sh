#!/bin/bash
#
# Compares the renders of two Mandelbulber builds on this machine, the way the regression
# workflow (.github/workflows/regression.yml) does on CI, in one command and without shards.
#
# Usage:  mandelbulber2/tools/regression_test.sh <before program> <after program> [output folder]
#
#   before program   the build without the change, such as a build of master
#   after program    the build with the change
#   output folder    where the renders and the report go (default: ./regression)
#
# Renders every regression scene (the fixed list of examples and one scene for every formula)
# twice with the before build, whose two renders give each scene's noise, and once with the
# after build, then compares them. Prints the report and exits with 0 when every scene passes
# and 1 when one differs; <output folder>/before/comparison has the before, after and
# difference images of every scene, and report.txt. It takes about an hour on 8 cores; for a
# quick check of a few core scenes, run "mandelbulber2 --test".
#
# Each program is copied into the output folder beside a link to the scenes of this source
# tree, so both render the same examples wherever they were built.

set -eu
if [ $# -lt 2 ]; then
	sed -n '3,/^$/s/^# \{0,1\}//p' "$0"
	exit 2
fi
root="$(cd "$(dirname "$0")/../.." && pwd)"
out="$(mkdir -p "${3:-regression}" && cd "${3:-regression}" && pwd)"
for side in before after; do
	program="$1"
	shift
	[ -x "$program" ] || { echo "Not a program: $program"; exit 2; }
	mkdir -p "$out/programs/$side/bin" "$out/programs/$side/share"
	cp "$program" "$out/programs/$side/bin/mandelbulber2"
	ln -sfn "$root/mandelbulber2/deploy/share/mandelbulber2" "$out/programs/$side/share/mandelbulber2"
done

status=0
"$root/.github/scripts/regression-shard.sh" "$out/programs/after/bin/mandelbulber2" all "$out" \
	before="$out/programs/before/bin/mandelbulber2" || status=$?
echo
grep -E "^\[(FAIL|NEW )\]" "$out/before/comparison/report.txt" || true
echo "Report and images: $out/before/comparison"
exit $status
