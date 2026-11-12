#!/bin/bash
#
# Mandelbulber v2 - builds the macOS dependencies from source, without Homebrew
#
# Homebrew builds its libraries for the macOS version of the machine it runs
# on, so a program linked against them only starts on that version or newer.
# This script builds the libraries with a deployment target instead, so the
# program also runs on older Macs.
#
# What it puts under <prefix>:
#   include/, lib/   static libraries: libpng, libtiff, lzo, gsl, libsndfile
#                    and the OpenMP runtime, plus the OpenCL C++ headers
#   qt/              Qt's official build for macOS (universal: x86_64 and arm64)
#   tools/           CMake and the Qt installer used during the build
#
# Usage:  ./build_deps.sh <prefix>
#
# Environment:
#   ARCH                      x86_64 or arm64 (default: this machine's).
#                             x86_64 on Apple Silicon builds under Rosetta 2.
#   MACOSX_DEPLOYMENT_TARGET  oldest macOS to support (default: 11.0, the
#                             oldest that the Qt version below supports)
#   QT_VERSION                Qt version to install (default: 6.7.3)
#
# The script is idempotent: a finished <prefix> is left alone, so a cached
# copy of it can be reused until this file changes.

set -euo pipefail

[ $# -eq 1 ] || { echo "usage: $0 <prefix>" >&2; exit 2; }

ARCH="${ARCH:-$(uname -m)}"
case "$ARCH" in
	x86_64 | arm64) ;;
	*) echo "ARCH must be x86_64 or arm64, not '$ARCH'" >&2; exit 2 ;;
esac
export ARCH

# run the whole build as the requested architecture, so every configure
# script and CMake project detects that architecture and not the machine's
if [ "$(uname -m)" != "$ARCH" ]; then
	exec arch "-$ARCH" /bin/bash "$0" "$@"
fi

mkdir -p "$1"
PREFIX="$(cd "$1" && pwd)"

export MACOSX_DEPLOYMENT_TARGET="${MACOSX_DEPLOYMENT_TARGET:-11.0}"
QT_VERSION="${QT_VERSION:-6.7.3}"

CMAKE_VERSION=3.31.6
AQT_VERSION=3.3.0
SEVENZIP_VERSION=2301
LIBPNG_VERSION=1.6.44
TIFF_VERSION=4.7.0
LZO_VERSION=2.10
GSL_VERSION=2.8
SNDFILE_VERSION=1.2.2
OPENMP_VERSION=19.1.7
CLHPP_VERSION=2024.10.24

STAMP="$PREFIX/.built"
WANTED="arch=$ARCH target=$MACOSX_DEPLOYMENT_TARGET qt=$QT_VERSION png=$LIBPNG_VERSION tiff=$TIFF_VERSION lzo=$LZO_VERSION gsl=$GSL_VERSION sndfile=$SNDFILE_VERSION openmp=$OPENMP_VERSION clhpp=$CLHPP_VERSION"
if [ -f "$STAMP" ] && [ "$(cat "$STAMP")" = "$WANTED" ]; then
	echo "Dependencies are already built in $PREFIX ($WANTED)"
	exit 0
fi

echo "Building dependencies into $PREFIX ($WANTED)"
rm -rf "$PREFIX"
mkdir -p "$PREFIX/include" "$PREFIX/lib" "$PREFIX/tools"

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
JOBS="$(sysctl -n hw.ncpu)"

export CFLAGS="-O2 -arch $ARCH"
export CXXFLAGS="-O2 -arch $ARCH"
export LDFLAGS="-arch $ARCH"
# nothing may be picked up from a Homebrew that happens to be installed
export PKG_CONFIG_LIBDIR="$PREFIX/lib/pkgconfig"
unset PKG_CONFIG_PATH CPATH LIBRARY_PATH

step() { printf '\n=== %s\n' "$*"; }

# These two set a variable instead of printing a path, so that a failed
# download, checksum or unpack stops the script.

# fetch <url> <sha256>: downloads to $WORK and checks the checksum; sets ARCHIVE
fetch() {
	ARCHIVE="$WORK/$(basename "$1")"
	curl -fsSL --retry 5 --retry-delay 5 -o "$ARCHIVE" "$1"
	echo "$2  $ARCHIVE" | shasum -a 256 -c -
}

# get <url> <sha256>: fetches and unpacks a source archive; sets SRC to its top folder
get() {
	local dir
	fetch "$1" "$2"
	dir="$(mktemp -d "$WORK/src.XXXXXX")"
	tar -xf "$ARCHIVE" -C "$dir"
	SRC="$dir/$(ls "$dir" | head -n 1)"
	[ -d "$SRC" ]
}

# --------------------------------------------------------------------------
# CMake (Kitware's universal build, so it also runs under Rosetta 2)
# --------------------------------------------------------------------------
step "CMake $CMAKE_VERSION"
fetch "https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-macos-universal.tar.gz" \
	330b9514f5112e5ed4fb08b8b05803b776fd9b539a6ae12927d14dcc0ee2ba8d
tar -xf "$ARCHIVE" -C "$PREFIX/tools"
CMAKE="$PREFIX/tools/cmake-$CMAKE_VERSION-macos-universal/CMake.app/Contents/bin/cmake"
"$CMAKE" --version

cmake_build() {
	local src="$1"
	shift
	"$CMAKE" -S "$src" -B "$src/build-mandelbulber" \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX="$PREFIX" \
		-DCMAKE_INSTALL_LIBDIR=lib \
		-DCMAKE_PREFIX_PATH="$PREFIX" \
		-DCMAKE_IGNORE_PREFIX_PATH="/opt/homebrew;/usr/local;/opt/local" \
		-DCMAKE_OSX_ARCHITECTURES="$ARCH" \
		-DCMAKE_OSX_DEPLOYMENT_TARGET="$MACOSX_DEPLOYMENT_TARGET" \
		-DCMAKE_FIND_FRAMEWORK=NEVER \
		-DBUILD_SHARED_LIBS=OFF \
		-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
		"$@"
	"$CMAKE" --build "$src/build-mandelbulber" --parallel "$JOBS"
	"$CMAKE" --install "$src/build-mandelbulber"
}

autotools_build() {
	local src="$1"
	shift
	(
		cd "$src"
		./configure --prefix="$PREFIX" --disable-shared --enable-static \
			--disable-dependency-tracking "$@"
		make -j"$JOBS"
		make install
	)
}

# --------------------------------------------------------------------------
# Libraries
# --------------------------------------------------------------------------
step "libpng $LIBPNG_VERSION"
get "https://github.com/pnggroup/libpng/archive/refs/tags/v$LIBPNG_VERSION.tar.gz" \
	0ef5b633d0c65f780c4fced27ff832998e71478c13b45dfb6e94f23a82f64f7c
cmake_build "$SRC" -DPNG_SHARED=OFF -DPNG_STATIC=ON -DPNG_FRAMEWORK=OFF -DPNG_TESTS=OFF -DPNG_TOOLS=OFF

step "libtiff $TIFF_VERSION"
get "https://download.osgeo.org/libtiff/tiff-$TIFF_VERSION.tar.gz" \
	67160e3457365ab96c5b3286a0903aa6e78bdc44c4bc737d2e486bcecb6ba976
# only the codecs that need no further library: the program writes
# uncompressed, LZW and Deflate files
cmake_build "$SRC" -Dtiff-tools=OFF -Dtiff-tests=OFF -Dtiff-contrib=OFF -Dtiff-docs=OFF \
	-Dtiff-opengl=OFF -Dcxx=OFF -Dzlib=ON -Dpixarlog=ON -Djpeg=OFF -Dold-jpeg=OFF \
	-Djbig=OFF -Dlzma=OFF -Dzstd=OFF -Dwebp=OFF -Dlerc=OFF -Dlibdeflate=OFF

step "lzo $LZO_VERSION"
get "https://www.oberhumer.com/opensource/lzo/download/lzo-$LZO_VERSION.tar.gz" \
	c0f892943208266f9b6543b3ae308fab6284c5c90e627931446fb49b4221a072
autotools_build "$SRC"

step "gsl $GSL_VERSION"
get "https://ftpmirror.gnu.org/gsl/gsl-$GSL_VERSION.tar.gz" \
	6a99eeed15632c6354895b1dd542ed5a855c0f15d9ad1326c6fe2b2c9e423190
autotools_build "$SRC"

step "libsndfile $SNDFILE_VERSION"
get "https://github.com/libsndfile/libsndfile/releases/download/$SNDFILE_VERSION/libsndfile-$SNDFILE_VERSION.tar.xz" \
	3799ca9924d3125038880367bf1468e53a1b7e3686a934f098b7e1d286cdb80e
# without the Ogg, FLAC, Opus and MPEG libraries: the program hands the files
# libsndfile cannot open to Qt Multimedia
cmake_build "$SRC" -DBUILD_PROGRAMS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF \
	-DENABLE_EXTERNAL_LIBS=OFF -DENABLE_MPEG=OFF -DENABLE_PACKAGE_CONFIG=ON -DINSTALL_MANPAGES=OFF

step "OpenMP runtime $OPENMP_VERSION"
# Apple's compiler supports OpenMP but ships no runtime library for it
get "https://github.com/llvm/llvm-project/releases/download/llvmorg-$OPENMP_VERSION/openmp-$OPENMP_VERSION.src.tar.xz" \
	bd7e6901ab086fd268750363017935fd4a717c153dad3c2aab86cb0140d9e3fe
OPENMP_SRC="$SRC"
get "https://github.com/llvm/llvm-project/releases/download/llvmorg-$OPENMP_VERSION/cmake-$OPENMP_VERSION.src.tar.xz" \
	11c5a28f90053b0c43d0dec3d0ad579347fc277199c005206b963c19aae514e3
# the runtime's build looks for LLVM's CMake modules in ../cmake
mv "$SRC" "$(dirname "$OPENMP_SRC")/cmake"
cmake_build "$OPENMP_SRC" -DLIBOMP_ENABLE_SHARED=OFF -DLIBOMP_INSTALL_ALIASES=OFF -DOPENMP_ENABLE_LIBOMPTARGET=OFF

step "OpenCL C++ headers $CLHPP_VERSION"
get "https://github.com/KhronosGroup/OpenCL-CLHPP/archive/refs/tags/v$CLHPP_VERSION.tar.gz" \
	51aebe848514b3bc74101036e111f8ee98703649eec7035944831dc6e05cec14
# the Mac build includes them as <OpenCL/...>, next to the system framework's C headers
mkdir -p "$PREFIX/include/OpenCL"
cp -R "$SRC/include/CL/"* "$PREFIX/include/OpenCL/"
cp "$PREFIX/include/OpenCL/opencl.hpp" "$PREFIX/include/OpenCL/cl.hpp"

# --------------------------------------------------------------------------
# Qt (official universal build; its libraries carry their own deployment target)
# --------------------------------------------------------------------------
step "Qt $QT_VERSION"
# Qt's archives are 7z files. The installer's own 7z module does not load on
# every Python version, so give it 7-Zip's universal command line program.
fetch "https://www.7-zip.org/a/7z$SEVENZIP_VERSION-mac.tar.xz" \
	343eae9ccbbd8f68320adaaa3c87e0244cf39fad0fbec6b9d2cd3e5b0f8a5fbf
mkdir -p "$PREFIX/tools/7zip"
tar -xf "$ARCHIVE" -C "$PREFIX/tools/7zip"
python3 -m venv "$PREFIX/tools/venv"
"$PREFIX/tools/venv/bin/pip" install --quiet "aqtinstall==$AQT_VERSION"
"$PREFIX/tools/venv/bin/aqt" install-qt mac desktop "$QT_VERSION" clang_64 -m qtmultimedia \
	-O "$PREFIX/qt" --external "$PREFIX/tools/7zip/7zz"
QT_DIR="$PREFIX/qt/$QT_VERSION/macos"
"$QT_DIR/bin/qmake" -query QT_VERSION

# --------------------------------------------------------------------------
# Checks
# --------------------------------------------------------------------------
step "Checking the built libraries"
for lib in libpng.a libtiff.a liblzo2.a libgsl.a libgslcblas.a libsndfile.a libomp.a; do
	[ -f "$PREFIX/lib/$lib" ] || { echo "missing $PREFIX/lib/$lib" >&2; exit 1; }
	archs="$(lipo -archs "$PREFIX/lib/$lib")"
	echo "$lib: $archs"
	[ "$archs" = "$ARCH" ] || { echo "$lib is not $ARCH" >&2; exit 1; }
done
# no shared copies: the linker would prefer them over the static libraries
if ls "$PREFIX"/lib/*.dylib >/dev/null 2>&1; then
	echo "unexpected shared libraries in $PREFIX/lib" >&2
	exit 1
fi

echo "$WANTED" > "$STAMP"
echo
echo "Dependencies are ready in $PREFIX"
