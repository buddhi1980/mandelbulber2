#!/bin/bash
# Prints a Markdown summary of the regression shard reports in the given folder
# (one subfolder report-<family>-<shard> per shard, each with report.txt).
set -u
reports="$1"

echo "## Regression test"
echo
echo "| Family | Shards | Scenes compared | Failed | Not valid in either build |"
echo "|---|---|---|---|---|"
for family in linux-x86_64 windows-x64 macos-arm64 macos-x86_64; do
	shards=0 compared=0 failed=0 broken=0
	for f in "$reports"/report-"$family"-*/report.txt; do
		[ -f "$f" ] || continue
		shards=$((shards + 1))
		pass=$(grep -c '^\[PASS\]' "$f")
		fail=$(grep -c '^\[FAIL\]' "$f")
		compared=$((compared + pass + fail))
		failed=$((failed + fail))
		broken=$((broken + $(grep -c 'not valid in either build\|both builds render an image that is not valid' "$f")))
	done
	echo "| $family | $shards | $compared | $failed | $broken |"
done

echo
echo "### Failed scenes"
echo
found=0
for f in "$reports"/report-*/report.txt; do
	[ -f "$f" ] || continue
	family="$(basename "$(dirname "$f")")"
	awk -v family="$family" '/^\[FAIL\]/ {scene=$2} /failures:/ && scene {sub(/^ *failures: */, ""); printf "- `%s` %s: %s\n", family, scene, $0; scene=""}' "$f"
	grep -q '^\[FAIL\]' "$f" && found=1
done
[ "$found" = 1 ] || echo "None."
