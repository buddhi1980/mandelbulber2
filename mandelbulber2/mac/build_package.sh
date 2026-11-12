#!/bin/bash
#
# Mandelbulber v2 - builds and packs the macOS application without Homebrew
#
# Builds mandelbulber-opencl-mac.pro with Apple's compiler against the
# dependencies made by build_deps.sh, packs the application bundle with
# macdeployqt, and checks that the result runs on the oldest macOS it is
# meant for and needs nothing from outside the bundle.
#
# Usage:  ./build_package.sh <deps prefix> <build dir> [package name]
#
#   deps prefix    folder made by ./build_deps.sh
#   build dir      emptied and used for the build; the packages end up in
#                  <build dir>/artifacts/<package name>.zip and .dmg
#   package name   default: mandelbulber2-macOS-<arch>
#
# Environment:
#   ARCH                      x86_64 or arm64 (default: this machine's); must
#                             be the one the dependencies were built for
#   MACOSX_DEPLOYMENT_TARGET  oldest macOS to support (default: 11.0)

set -euo pipefail

[ $# -ge 2 ] || { echo "usage: $0 <deps prefix> <build dir> [package name]" >&2; exit 2; }

ARCH="${ARCH:-$(uname -m)}"
export ARCH
if [ "$(uname -m)" != "$ARCH" ]; then
	exec arch "-$ARCH" /bin/bash "$0" "$@"
fi

FILEDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SUPPORT="$(dirname "$FILEDIR")"   # the mandelbulber2 folder with src, qt, qmake, deploy
DEPS="$(cd "$1" && pwd)"
mkdir -p "$2"
BUILD="$(cd "$2" && pwd)"
NAME="${3:-mandelbulber2-macOS-$ARCH}"
export MACOSX_DEPLOYMENT_TARGET="${MACOSX_DEPLOYMENT_TARGET:-11.0}"

[ -f "$DEPS/.built" ] || { echo "$DEPS is not a finished build_deps.sh folder" >&2; exit 1; }
grep -q "arch=$ARCH " "$DEPS/.built" || { echo "$DEPS was built for another architecture: $(cat "$DEPS/.built")" >&2; exit 1; }
QT_DIR="$(ls -d "$DEPS"/qt/*/macos | head -n 1)"
[ -x "$QT_DIR/bin/qmake" ] || { echo "no Qt in $DEPS/qt" >&2; exit 1; }

# Qt 6.7 still links OpenGL programs against Apple's AGL framework, which
# newer macOS SDKs no longer contain and which nothing here uses
# (grep finds nothing in a Qt that was already cleaned, which is not an error)
{ find "$QT_DIR/mkspecs" "$QT_DIR/lib" -type f \( -name '*.conf' -o -name '*.pri' -o -name '*.prl' \) \
	-exec grep -l -e '-framework AGL' {} + 2>/dev/null || true; } | while IFS= read -r file; do
	echo "Removing the AGL framework from $file"
	sed -i '' 's/ *-framework AGL//g' "$file"
done

export PATH="$QT_DIR/bin:$PATH"
# keep qmake's pkg-config lookups away from a Homebrew that happens to be installed
export PKG_CONFIG_LIBDIR="$DEPS/lib/pkgconfig"
unset PKG_CONFIG_PATH CPATH LIBRARY_PATH CC CXX CPPFLAGS CFLAGS CXXFLAGS LDFLAGS

step() { printf '\n=== %s\n' "$*"; }

# --------------------------------------------------------------------------
# Build
# --------------------------------------------------------------------------
step "Building for $ARCH, macOS $MACOSX_DEPLOYMENT_TARGET and newer, with Qt $(qmake -query QT_VERSION)"
rm -rf "${BUILD:?}"/*
cd "$BUILD"
EXTRA_CXXFLAGS=""
if [ "$ARCH" = "arm64" ]; then
	# Qt 6.7's qyieldcpu.h calls __yield() without including the header that
	# declares it, which newer Apple compilers reject
	EXTRA_CXXFLAGS="-include arm_acle.h"
fi
qmake "$SUPPORT/qmake/mandelbulber-opencl-mac.pro" \
	CONFIG+=macdeps MAC_DEPS="$DEPS" \
	QMAKE_APPLE_DEVICE_ARCHS="$ARCH" \
	QMAKE_MACOSX_DEPLOYMENT_TARGET="$MACOSX_DEPLOYMENT_TARGET" \
	QMAKE_CXXFLAGS+="-DCL_SILENCE_DEPRECATION -DCL_TARGET_OPENCL_VERSION=120 $EXTRA_CXXFLAGS" \
	QMAKE_CFLAGS+="-DCL_SILENCE_DEPRECATION -DCL_TARGET_OPENCL_VERSION=120"
make -j"$(sysctl -n hw.ncpu)"

PACK="$BUILD/mandelbulber2.app"
binary="$PACK/Contents/MacOS/mandelbulber2"
[ -f "$binary" ] || { echo "Error Building $binary" >&2; exit 1; }
built="$(lipo -archs "$binary")"
[ "$built" = "$ARCH" ] || { echo "$binary is $built, expected $ARCH" >&2; exit 1; }

# --------------------------------------------------------------------------
# Pack
# --------------------------------------------------------------------------
step "Packing $PACK"
RESOURCES="$PACK/Contents/Resources"
mkdir -p "$RESOURCES"
# copying source files
cp -R "$SUPPORT/src" "$PACK/"
cp -R "$SUPPORT/qt" "$PACK/"
# copying makefiles into Apple standard Resources subfolders
mkdir -p "$RESOURCES/makefiles"
cp "$SUPPORT/qmake/mandelbulber.pro" "$RESOURCES/makefiles/"
cp "$SUPPORT/qmake/mandelbulber-opencl-mac.pro" "$RESOURCES/makefiles/"
cp "$SUPPORT/qmake/common.pri" "$RESOURCES/makefiles/"
# copying documentation files into standard Resources subfolders
mkdir -p "$RESOURCES/doc"
cp "$SUPPORT/deploy/NEWS" "$RESOURCES/doc/"
DOCFILE="$(curl -s https://api.github.com/repos/mandelbulber/mandelbulber2/releases/latest | grep browser_download_url | grep -i '\.pdf' | head -n 1 | cut -d '"' -f 4 || true)"
echo "Downloading manual from: $DOCFILE"
if [ -n "$DOCFILE" ]; then
	curl -fsSL -o "$RESOURCES/doc/Mandelbulber_Manual.pdf" "$DOCFILE" || echo "PDF manual download failed, continuing"
else
	echo "No PDF manual found in latest release, skipping download"
fi

macdeployqt mandelbulber2.app -dmg -verbose=2

# --------------------------------------------------------------------------
# Checks: every program and library in the bundle
# --------------------------------------------------------------------------
step "Checking the bundle"
target_major="${MACOSX_DEPLOYMENT_TARGET%%.*}"
failed=0
while IFS= read -r file; do
	file -b "$file" | grep -q 'Mach-O' || continue
	short="${file#"$PACK/"}"
	# nothing may load from Homebrew, MacPorts or the dependency folder
	outside="$(otool -L "$file" | tail -n +2 | awk '{print $1}' | grep -E "^(/opt/|/usr/local/|$DEPS/)" || true)"
	if [ -n "$outside" ]; then
		echo "FAIL $short loads from outside the bundle: $outside"
		failed=1
	fi
	# must start on the deployment target
	newest="$(vtool -show-build "$file" 2>/dev/null | awk '/minos/ {split($2, v, "."); if (v[1] + 0 > max) max = v[1] + 0} END {print max + 0}')"
	if [ "$newest" -gt "$target_major" ]; then
		echo "FAIL $short needs macOS $newest"
		failed=1
	fi
done < <(find "$PACK/Contents" -type f \( -perm -u+x -o -name '*.dylib' \))
[ "$failed" -eq 0 ] || exit 1
echo "All programs and libraries in the bundle start on macOS $MACOSX_DEPLOYMENT_TARGET and load nothing from outside it"
echo "mandelbulber2: $(lipo -archs "$binary"), $(vtool -show-build "$binary" | awk '/minos/ {print "macOS " $2 " and newer"; exit}')"

# --------------------------------------------------------------------------
# Artifacts
# --------------------------------------------------------------------------
step "Artifacts"
mkdir -p "$BUILD/artifacts"
ditto -c -k --keepParent "$PACK" "$BUILD/artifacts/$NAME.zip"
mv "$BUILD/mandelbulber2.dmg" "$BUILD/artifacts/$NAME.dmg"
ls -lh "$BUILD/artifacts"
