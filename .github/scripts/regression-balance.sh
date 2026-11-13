#!/bin/bash
#
# Splits the regression scenes into shards that take about the same time to render, and
# prints the names of the scenes of one shard, one per line.
#
# Usage:  regression-balance.sh <scene list> <times> <N> <K>
#
#   scene list   every scene name, one per line (mandelbulber2 --regression-list)
#   times        "<seconds> <name>" lines (regression-times.txt); lines starting with # are
#                comments, and a scene missing here counts with the average time
#   N, K         the number of shards, and the shard to print (1..N)
#
# Each scene, the slowest first, goes to the shard with the least time so far. The split
# depends only on the two files, so every shard job computes the same one.

set -eu
export LC_ALL=C
scenes="$1"
times="$2"
count="$3"
index="$4"

awk 'NR == FNR { if ($0 !~ /^#/ && NF == 2) { t[$2] = $1; sum += $1; n++ } next }
	NF { printf "%s %s\n", ($1 in t ? t[$1] : (n ? sum / n : 1)), $1 }' "$times" "$scenes" |
	sort -k1,1gr -k2,2 |
	awk -v n="$count" -v k="$index" '
		{
			best = 1
			for (i = 2; i <= n; i++) if (load[i] < load[best]) best = i
			load[best] += $1
			if (best == k) print $2
		}
		END { printf "shard %d of %d: %.0f s of renders by one build\n", k, n, load[k] > "/dev/stderr" }'
