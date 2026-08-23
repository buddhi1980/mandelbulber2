#!/bin/bash
# Build Parameters #
FILEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SRC=$FILEDIR/../..
BUILD=$SRC/build/

# Clean build directory
rm -rf $BUILD
mkdir -p $BUILD

# Minimum Requirement
# Check Homebrew Install
tput bold ; echo ; echo '♻️ ' Check Homebrew Install ; tput sgr0 ; sleep 1
if ls /usr/local/bin/ | grep brew > /dev/null ; then tput sgr0 ; echo "HomeBrew AllReady Installed" ; else tput bold ; echo "Installing HomeBrew" ; tput sgr0 ; /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)" ; fi

brew update
brew install llvm qt gsl lzo jpeg libpng libtiff libsndfile tree p7zip wget

# 1. Clone the repository into a clean local directory
CLONE_DIR="$BUILD/OpenCL-CLHPP-Repo"
git clone https://github.com/KhronosGroup/OpenCL-CLHPP.git "$CLONE_DIR"
# 2. Build the exact OpenCL folder structure required by Mandelbulber Mac profiles
mkdir -p "$BUILD/OpenCL"
# 3. Copy the base contents safely into the folder
cp -vfr "$CLONE_DIR/include/CL/"* "$BUILD/OpenCL/"
# 4. Generate the missing cl.hpp file name mapping using the universal source base
cp -vf "$BUILD/OpenCL/opencl.hpp" "$BUILD/OpenCL/cl.hpp"

# build operation
cd $BUILD && qmake $SRC/mandelbulber2/qmake/mandelbulber-opencl-mac.pro \
QMAKE_CXXFLAGS+="-I$BUILD -DCL_SILENCE_DEPRECATION -DCL_TARGET_OPENCL_VERSION=120" \
QMAKE_CFLAGS+="-I$BUILD -DCL_SILENCE_DEPRECATION -DCL_TARGET_OPENCL_VERSION=120"
cd $BUILD && make -j8

# binary requirement
binary=$BUILD/mandelbulber2.app/Contents/MacOS/mandelbulber2
if [ -f $binary ];
then
echo "$binary found"
else
echo "Error Building $binary"
exit
fi

#macOS build package
SUPPORT=$SRC/mandelbulber2
PACK=$BUILD/mandelbulber2.app
RESOURCES="$PACK/Contents/Resources"
#making directories
#mkdir -vp "$PACK"
mkdir -p "$RESOURCES"

#copying source files	
cp -vr "$SUPPORT/src" "$PACK/"
cp -vr "$SUPPORT/qt" "$PACK/"
#cp -vr "$SUPPORT/opencl" "$PACK/"
# copying makefiles into Apple standard Resources subfolders
mkdir -vp "$RESOURCES/makefiles"
cp -v "$SUPPORT/qmake/mandelbulber.pro" "$RESOURCES/makefiles/"
cp -v "$SUPPORT/qmake/mandelbulber-opencl-mac.pro" "$RESOURCES/makefiles/"
cp -v "$SUPPORT/qmake/common.pri" "$RESOURCES/makefiles/"
# copying documentation files into standard Resources subfolders
mkdir -vp "$RESOURCES/doc"
cp -v "$SUPPORT/deploy/NEWS" "$RESOURCES/doc/"
DOCFILE="$(curl -s https://api.github.com/repos/mandelbulber/mandelbulber2/releases/latest | grep browser_download_url | grep -i '\.pdf' | head -n 1 | cut -d '"' -f 4 || true)"
echo "Downloading manual from: $DOCFILE"
if [ -n "$DOCFILE" ]; then
  wget -O "$RESOURCES/doc/Mandelbulber_Manual.pdf" "$DOCFILE" || echo "PDF manual download failed, continuing"
else
  echo "No PDF manual found in latest release, skipping download"
fi
# c++abicopy (files may not exist on macOS 12+ due to dyld shared cache)
mkdir -p "$PACK/Contents/Frameworks/"
if [ -f "/usr/lib/libc++abi.dylib" ]; then
  cp "/usr/lib/libc++abi.dylib" "$PACK/Contents/Frameworks/"
fi
if [ -f "/usr/lib/libc++abi.1.dylib" ]; then
  cp "/usr/lib/libc++abi.1.dylib" "$PACK/Contents/Frameworks/"
fi
if [ -f "$PACK/Contents/Frameworks/libc++abi.dylib" ]; then
  cp "$PACK/Contents/Frameworks/libc++abi.dylib" "$PACK/Contents/Frameworks/libc++abi.1.dylib"
fi
cd $BUILD && macdeployqt mandelbulber2.app -dmg -verbose=3
