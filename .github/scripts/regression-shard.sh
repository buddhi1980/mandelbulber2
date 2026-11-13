#!/bin/bash
#
# Renders one shard of the regression scenes with the build under test (head) once, and with
# each older build twice, and compares head with each older build.
#
# Usage:  regression-shard.sh <head program> <K/N> <output folder> <name>=<program>...
#
#   name=program   an older build to compare with, such as previous=run-previous/bin/mandelbulber2
#
# Writes <output folder>/head, and <output folder>/<name>/before and <name>/comparison (with
# report.txt) for each older build. Exits with 0 when every scene of the shard passes every
# comparison, and 1 when one differs.
#
# Each render step may take at most RENDER_TIMEOUT (default 90m), so that a build that hangs
# on a scene fails the shard's scenes instead of holding the runner for hours.

set -u
timeout="${RENDER_TIMEOUT:-90m}"

# runs a render step; reports when it was stopped by the time limit
render() {
	timeout --kill-after=1m "$timeout" "$@"
	local status=$?
	if [ $status -eq 124 ] || [ $status -eq 137 ]; then
		echo "::error::A render step ran longer than $timeout and was stopped: $*"
	fi
	return 0
}
head_program="$1"
shard="$2"
out="$(mkdir -p "$3" && cd "$3" && pwd)"
shift 3

echo "Shard $shard: rendering with the head build"
render "$head_program" --nogui --regression-shard "$shard" --regression-once --regression-render "$out/head"

status=0
for pair in "$@"; do
	name="${pair%%=*}"
	program="${pair#*=}"
	echo "Shard $shard: rendering with the $name build"
	render "$program" --nogui --regression-shard "$shard" --regression-render "$out/$name/before"
	echo "Shard $shard: comparing with the $name build"
	"$head_program" --nogui --regression-shard "$shard" \
		--regression-compare "$out/$name/before" "$out/head" --output "$out/$name/comparison" || status=1
done
exit $status
