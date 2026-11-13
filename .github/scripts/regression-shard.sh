#!/bin/bash
#
# Renders one shard of the regression scenes with two builds and compares them.
#
# Usage:  regression-shard.sh <base program> <head program> <K/N> <output folder>
#
# Writes <output folder>/before, after and comparison (with report.txt).
# Exits with 0 when every scene of the shard passes, and 1 when one differs.

set -u
base_program="$1"
head_program="$2"
shard="$3"
out="$(mkdir -p "$4" && cd "$4" && pwd)"

echo "Shard $shard: rendering with the base build"
"$base_program" --nogui --regression-shard "$shard" --regression-render "$out/before" || true
echo "Shard $shard: rendering with the head build"
"$head_program" --nogui --regression-shard "$shard" --regression-render "$out/after" || true
echo "Shard $shard: comparing"
"$head_program" --nogui --regression-shard "$shard" \
	--regression-compare "$out/before" "$out/after" --output "$out/comparison"
