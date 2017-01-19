#!/bin/sh
#

# this script can be executed in a debian shell to compile mandelbulber for windows 32 / 64 bit
#

if [ $# -ne 2 ]
then
	echo syntax: cross-compile-to-windows.sh [number_new] [32/64]
	exit
fi

CXXFLAGS=""

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

set -e # if any of the commands fail the script will exit immediately

if [ ! -d mandelbulber2 ]; then
	git clone https://github.com/buddhi1980/mandelbulber2
	cd mandelbulber2/mandelbulber2
else
	cd mandelbulber2
	git pull
	#git checkout 2.09-bugfixes
	git checkout master
	git pull
	cd mandelbulber2
fi

MANDELBULBER_DLL_TARGET=deploy/win$MANDELBULBER_WIN_VERSION/dll/

# purge dll folder
mkdir -p $MANDELBULBER_DLL_TARGET

## copy dlls
cp $MANDELBULBER_PREFIX/bin/libgsl-*.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/libgslcblas-*.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/libpng*.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Core.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Gui.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Network.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Svg.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Widgets.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Test.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Gamepad.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/Qt5Multimedia.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/zlib*.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/libtiff*.dll $MANDELBULBER_DLL_TARGET
cp $MANDELBULBER_PREFIX/bin/libsndfile*.dll $MANDELBULBER_DLL_TARGET

mkdir -p $MANDELBULBER_DLL_TARGET/iconengines
cp $MANDELBULBER_PREFIX/plugins/iconengines/* $MANDELBULBER_DLL_TARGET/iconengines/
mkdir -p $MANDELBULBER_DLL_TARGET/platforms
cp $MANDELBULBER_PREFIX/plugins/platforms/* $MANDELBULBER_DLL_TARGET/platforms/
mkdir -p $MANDELBULBER_DLL_TARGET/imageformats
cp $MANDELBULBER_PREFIX/plugins/imageformats/* $MANDELBULBER_DLL_TARGET/imageformats/
mkdir -p $MANDELBULBER_DLL_TARGET/gamepads
cp $MANDELBULBER_PREFIX/plugins/gamepads/* $MANDELBULBER_DLL_TARGET/gamepads/
mkdir -p $MANDELBULBER_DLL_TARGET/audio
cp $MANDELBULBER_PREFIX/plugins/audio/* $MANDELBULBER_DLL_TARGET/audio/
mkdir -p $MANDELBULBER_DLL_TARGET/mediaservice
cp $MANDELBULBER_PREFIX/plugins/mediaservice/* $MANDELBULBER_DLL_TARGET/mediaservice/

cp /usr/$MANDELBULBER_MINGW_HOST/lib/libwinpthread-1.dll $MANDELBULBER_DLL_TARGET

cp /usr/lib/gcc/$MANDELBULBER_MINGW_HOST/6.2-win32/libstdc++-6.dll $MANDELBULBER_DLL_TARGET
cp /usr/lib/gcc/$MANDELBULBER_MINGW_HOST/6.2-win32/libgomp-1.dll $MANDELBULBER_DLL_TARGET

if [ $MANDELBULBER_WIN_VERSION -eq "64" ]
then
	cp /usr/lib/gcc/$MANDELBULBER_MINGW_HOST/6.2-win32/libgcc_s_seh-1.dll $MANDELBULBER_DLL_TARGET
else
	cp /usr/lib/gcc/$MANDELBULBER_MINGW_HOST/6.2-win32/libgcc_s_sjlj-1.dll $MANDELBULBER_DLL_TARGET
fi

mkdir -p $MANDELBULBER_BUILD_FOLDER
cd $MANDELBULBER_BUILD_FOLDER
$MANDELBULBER_PREFIX/bin/qmake ../Release/mandelbulber.pro -r -spec win32-g++
make -j8
cd ..
./make-package.sh $MANDELBULBER_VERSION $MANDELBULBER_BINARY_TARGET
tar cf $MANDELBULBER_BINARY_TARGET.tar.gz $MANDELBULBER_BINARY_TARGET

echo Finished

# to get the compiled binaries, do
# sftp root@thisHost
# get /tmp/mandelbulberBinaries.tar.gz
