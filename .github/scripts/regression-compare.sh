#!/bin/bash
#
# Mandelbulber regression test for CI on Linux and macOS.
#
# Builds the commit before the change as well, renders the regression scenes
# with both builds and compares them (mandelbulber2 --regression-render and
# --regression-compare). The renders, the difference images and report.txt go
# to ./regression in the repository.
#
# Usage:  regression-compare.sh <program> [base commit]
#
#   program       the mandelbulber2 program built from this commit
#   base commit   the commit to compare with: the pull request's base or the
#                 previous commit of a push; default: the parent of HEAD
#
# The base is built the same way as this commit: on Linux with its own
# deploy/linux/install --no-install, on macOS with its own
# mac/build_package.sh and the dependencies in $DEPS.
#
# Exits with 0 when every scene passes or when the comparison is skipped, and
# with 1 when a scene differs too much. It is skipped, with a message, when
# there is no base commit, when the base has no regression test yet, or when
# the base does not build.

set -u

program="$1"
base="${2:-}"

skip() {
	if [ -n "${GITHUB_ACTIONS:-}" ]; then
		echo "::notice::Regression comparison skipped: $*"
	else
		echo "Regression comparison skipped: $*"
	fi
	exit 0
}

root="$(git rev-parse --show-toplevel)"
cd "$root"

if [ -z "$base" ] || [ "$base" = "0000000000000000000000000000000000000000" ]; then
	# a shallow checkout has no parent yet
	if [ -f "$(git rev-parse --git-dir)/shallow" ]; then
		git fetch -q --depth 2 origin "$(git rev-parse HEAD)" || true
	fi
	base="$(git rev-parse -q --verify 'HEAD^')" || skip "this run has no base commit"
fi

if ! git cat-file -e "$base^{commit}" 2>/dev/null; then
	git fetch -q --depth 1 origin "$base" || skip "base commit $base cannot be fetched"
fi
git cat-file -e "$base:mandelbulber2/src/regression_test.cpp" 2>/dev/null ||
	skip "base commit $base has no regression test"

echo "Comparing with base commit $(git log -1 --format='%h %s' "$base")"
base_dir="$(dirname "$root")/regression-base"
rm -rf "$base_dir"
git worktree prune
git worktree add --detach "$base_dir" "$base" || skip "base commit $base cannot be checked out"

case "$(uname -s)" in
	Linux)
		(cd "$base_dir/mandelbulber2" &&
			./deploy/linux/install --opencl --no-deps --no-install \
				--build-dir "$PWD/build" --jobs "$(nproc)") ||
			skip "the base commit does not build"
		# it reads the scenes that the installed program of this commit reads
		base_program="$base_dir/mandelbulber2/build/mandelbulber2"
		;;
	Darwin)
		"$base_dir/mandelbulber2/mac/build_package.sh" "$DEPS" "$base_dir/build" regression-base ||
			skip "the base commit does not build"
		# each app bundle reads the scenes in its own Resources folder
		base_program="$base_dir/build/mandelbulber2.app/Contents/MacOS/mandelbulber2"
		;;
	*)
		skip "no base build for $(uname -s)"
		;;
esac
[ -x "$base_program" ] || skip "the base commit built no program"

out="$root/regression"
rm -rf "$out"
mkdir -p "$out"
# a scene that fails to render is reported by the comparison
"$base_program" --nogui --regression-render "$out/before" || true
"$program" --nogui --regression-render "$out/after" || true
"$program" --nogui --regression-compare "$out/before" "$out/after" --output "$out/comparison"
