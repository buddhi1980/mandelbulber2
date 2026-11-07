#!/usr/bin/env bash
# Enforces the floating-point build rules of Principle III in .specify/memory/constitution.md.
#   check-flags.sh search            Searches the tracked build files, scripts, and CI files, before a build.
#   check-flags.sh flags <Makefile>  Reads the flags a generated Makefile passes to the compiler and the linker.
# Exit codes: 0 every rule passed, 1 a rule failed, 2 the check could not run.
# This file holds the only list of prohibited options. specs/003-strict-floating-point/contracts/flag-check.md
# describes the rules.
set -u

OPTIONS=(-ffast-math -Ofast -funsafe-math-optimizations -ffinite-math-only
	-fassociative-math -freciprocal-math -fno-signed-zeros -ffp-model=fast
	-march=native -mtune=native -mcpu=native)

# An option matches only as a whole option, so -fno-fast-math and -march=x86-64 do not match.
BEFORE='(^|[^[:alnum:]_-])'
AFTER='([^[:alnum:]_-]|$)'
CONTRACT='-ffp-contract=[[:alnum:]_-]*'

# Tracked files that can set a compiler flag on a build. The search leaves out this action, which
# holds the list, NaNtester.sh, which builds a test program of its own, and the project documents.
BUILD_FILES=(
	':(glob)**/*.pro' ':(glob)**/*.pri' ':(glob)**/CMakeLists.txt' ':(glob)**/*.cmake'
	':(glob)**/*.vcxproj' ':(glob)**/*.props' ':(glob)**/*.targets' ':(glob)**/*.sln'
	':(glob)**/*.sh' ':(glob)**/*.bat' ':(glob)**/*.yml' ':(glob)**/*.yaml'
	Dockerfile mandelbulber2/deploy/linux/install
	':!.github/actions/compile-engine' ':!mandelbulber2/tools/DOE/NaNtester.sh'
	':!specs' ':!.specify' ':!.claude')

# The Makefile lines whose values reach the compiler and the linker.
MAKE_VARIABLES=(CC CXX LINK CFLAGS CXXFLAGS LFLAGS LIBS)

failures=0

report() {
	echo "FAILED: $1"
	failures=$((failures + 1))
}

stop() {
	echo "ERROR: $1"
	exit 2
}

# Reports every line of a tracked build file that matches a pattern, as <file>:<line>.
search_for() { # name, pattern
	local found status hit file rest
	found=$(git grep --no-color -nIE -e "$2" -- "${BUILD_FILES[@]}" 2>/dev/null)
	status=$?
	case $status in
	0)
		while IFS= read -r hit; do
			file=${hit%%:*}
			rest=${hit#*:}
			report "$1 in $file:${rest%%:*}"
		done <<<"$found"
		;;
	1) ;;
	*) stop "git grep exited with $status while searching for $1" ;;
	esac
}

# Reports every -ffp-contract value other than off, as <file>:<line>.
search_contraction() {
	local found status hit
	found=$(git grep --no-color -nIoE -e "$CONTRACT" -- "${BUILD_FILES[@]}" 2>/dev/null)
	status=$?
	case $status in
	0)
		while IFS= read -r hit; do
			[ "${hit##*:}" = "-ffp-contract=off" ] || report "${hit##*:} in ${hit%:*}"
		done <<<"$found"
		;;
	1) ;;
	*) stop "git grep exited with $status while searching for -ffp-contract" ;;
	esac
}

search_build_files() {
	local top count option
	top=$(git rev-parse --show-toplevel 2>/dev/null) || stop "no build files found, because $PWD is not in a git checkout"
	cd "$top" || stop "cannot enter $top"
	count=$(git ls-files -- "${BUILD_FILES[@]}" 2>/dev/null | wc -l)
	[ "$count" -gt 0 ] || stop "no build files found in $top"
	for option in "${OPTIONS[@]}"; do
		search_for "$option" "$BEFORE$option$AFTER"
	done
	search_contraction
	search_for "/fp:fast" "/fp:fast"
	search_for "/fp:contract" "/fp:contract"
	search_for "FloatingPointModel Fast" "<FloatingPointModel>[[:space:]]*Fast"
	echo "searched $count build files"
}

check_makefile() {
	local makefile=$1 variable line option value
	[ -f "$makefile" ] || stop "cannot read $makefile"
	[ -s "$makefile" ] || stop "cannot read $makefile, because it is empty"
	for variable in "${MAKE_VARIABLES[@]}"; do
		line=$(grep -E "^$variable[[:space:]]*=" "$makefile") || stop "$makefile has no $variable line"
		for option in "${OPTIONS[@]}"; do
			if grep -qE -e "$BEFORE$option$AFTER" <<<"$line"; then
				report "$option in $variable"
			fi
		done
		for value in $(grep -oE -e "$CONTRACT" <<<"$line"); do
			[ "$value" = "-ffp-contract=off" ] || report "$value in $variable"
		done
	done
	for variable in CFLAGS CXXFLAGS; do
		grep -E "^$variable[[:space:]]*=" "$makefile" | grep -qE -e "$BEFORE-ffp-contract=off$AFTER" ||
			report "-ffp-contract=off missing from $variable"
	done
}

case "${1:-}" in
search)
	[ $# -eq 1 ] || stop "usage: check-flags.sh search"
	search_build_files
	;;
flags)
	[ $# -eq 2 ] || stop "usage: check-flags.sh flags <Makefile>"
	check_makefile "$2"
	;;
*)
	stop "usage: check-flags.sh search | flags <Makefile>"
	;;
esac

if [ "$failures" -gt 0 ]; then
	echo "$failures check(s) failed"
	exit 1
fi
echo "passed"
