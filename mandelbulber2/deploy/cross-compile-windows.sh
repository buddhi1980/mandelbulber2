#!/bin/sh
#

# this script can be executed in a debian shell to compile mandelbulber for windows 32 / 64 bit
#

if [ $# -ne 2 ]
then
	echo syntax: cross-compile-to-windows.sh [number_new] [32/64]
	exit
fi

MANDELBULBER_WIN_VERSION=$2
MANDELBULBER_VERSION=$1
MANDELBULBER_BINARY_TARGET="/tmp/mandelbulberBinaries"

if [ $MANDELBULBER_WIN_VERSION -eq "64" ]
then
	MANDELBULBER_MINGW_HOST="x86_64-w64-mingw32"	
	MANDELBULBER_PREFIX="/usr/local/mingw-64bit"
	MANDELBULBER_BUILD_FOLDER="build-mandelbulber-MinGw64Qt-Release"
else
	MANDELBULBER_MINGW_HOST="i686-w64-mingw32"
	MANDELBULBER_PREFIX="/usr/local/mingw-32bit"
	MANDELBULBER_BUILD_FOLDER="build-mandelbulber-MinGwQt-Release"
fi

git clone https://github.com/buddhi1980/mandelbulber2
cd mandelbulber2/mandelbulber2

MANDELBULBER_DLL_TARGET=deploy/win$MANDELBULBER_WIN_VERSION/dll/

# purge dll folder
rm -r $MANDELBULBER_DLL_TARGET*

## copy dlls
cp $MANDELBULBER_PREFIX/bin/libgsl-*.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/libgslcblas-*.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/libpng*.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Core.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Gui.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Network.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Svg.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Widgets.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Gamepad.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/zlib*.dll $MANDELBULBER_DLL_TARGET

mkdir $MANDELBULBER_DLL_TARGET/iconengines
cp $MANDELBULBER_PREFIX/plugins/iconengines/* $MANDELBULBER_DLL_TARGET/iconengines/
mkdir $MANDELBULBER_DLL_TARGET/platforms
cp $MANDELBULBER_PREFIX/plugins/platforms/* $MANDELBULBER_DLL_TARGET/platforms/
mkdir $MANDELBULBER_DLL_TARGET/imageformats
cp $MANDELBULBER_PREFIX/plugins/imageformats/* $MANDELBULBER_DLL_TARGET/imageformats/

cp $MANDELBULBER_PREFIX/lib/libwinpthread-1.dll $MANDELBULBER_DLL_TARGET

cp /usr/lib/gcc/$MANDELBULBER_MINGW_HOST/4.9-win32/libstdc++-6.dll $MANDELBULBER_DLL_TARGET
cp /usr/lib/gcc/$MANDELBULBER_MINGW_HOST/4.9-win32/libgomp.dll $MANDELBULBER_DLL_TARGET
cp /usr/lib/gcc/$MANDELBULBER_MINGW_HOST/4.9-win32/libgcc_s_seh-1.dll $MANDELBULBER_DLL_TARGET

mkdir $MANDELBULBER_BUILD_FOLDER
cd $MANDELBULBER_BUILD_FOLDER
$MANDELBULBER_PREFIX/bin/qmake ../Release/mandelbulber.pro
make
cd ..
./make-package.sh $MANDELBULBER_VERSION $MANDELBULBER_BINARY_TARGET
tar cf $MANDELBULBER_BINARY_TARGET.tar.gz $MANDELBULBER_BINARY_TARGET

# to get the compiled binaries, do
# sftp root@thisHost
# get /tmp/mandelbulberBinaries.tar.gz
