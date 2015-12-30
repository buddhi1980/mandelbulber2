#!/bin/sh
#

# this script can be executed in a debian shell to obtain and compile all necessary components
# to build mandelbulber for windows 32 / 64 bit
# !Warning! this script modifies system components, use with care and read before execution
#

if [ $# -ne 1 ]
then
	echo syntax: setup-cross-compile-to-windows.sh [32/64]
	exit
fi

CXXFLAGS=""
MANDELBULBER_QT_URL="http://download.qt.io/official_releases/qt/5.5/5.5.1/single/qt-everywhere-opensource-src-5.5.1.tar.gz"
MANDELBULBER_WIN_VERSION=$1

if [ $MANDELBULBER_WIN_VERSION -eq "64" ]
then
	MANDELBULBER_MINGW_HOST="x86_64-w64-mingw32"
	MANDELBULBER_PREFIX="/usr/local/mingw-64bit"
else
	MANDELBULBER_MINGW_HOST="i686-w64-mingw32"
	MANDELBULBER_PREFIX="/usr/local/mingw-32bit"
fi

sudo apt-get install git make g++ dh-autoreconf
sudo apt-get install mingw-w64
mkdir $MANDELBULBER_PREFIX

### qt
wget $MANDELBULBER_QT_URL
tar xf qt-everywhere-*.tar.gz
cd qt-everywhere-*
yes | ./configure -release -xplatform win32-g++ -opengl desktop -nomake examples -device-option \
	CROSS_COMPILE=$MANDELBULBER_MINGW_HOST- -prefix $MANDELBULBER_PREFIX -opensource \
	-skip qtactiveqt -skip qtcanvas3d
make -j8
sudo make -j8 install
cd ..

### gsl
apt-get source gsl
cd gsl-*
mkdir doc
cp -vax debian/Makefile.in.doc doc/Makefile.in
dh_autoreconf 
./configure --host=$MANDELBULBER_MINGW_HOST -prefix=$MANDELBULBER_PREFIX
make -j8
sudo make -j8 install
cd ..
 
### zlib
apt-get source zlib
cd zlib-*
cp win32/Makefile.gcc Makefile
make PREFIX=/usr/bin/$MANDELBULBER_MINGW_HOST- \
	INCLUDE_PATH=$MANDELBULBER_PREFIX/include LIBRARY_PATH=$MANDELBULBER_PREFIX/lib \
	BINARY_PATH=$MANDELBULBER_PREFIX/bin SHARED_MODE=1
sudo make PREFIX=/usr/bin/$MANDELBULBER_MINGW_HOST- \
	INCLUDE_PATH=$MANDELBULBER_PREFIX/include LIBRARY_PATH=$MANDELBULBER_PREFIX/lib \
	BINARY_PATH=$MANDELBULBER_PREFIX/bin SHARED_MODE=1 install
cd ..

### libpng
apt-get source libpng
cd libpng-*
./configure --host=$MANDELBULBER_MINGW_HOST -prefix=$MANDELBULBER_PREFIX LDFLAGS=-L$MANDELBULBER_PREFIX/lib
make -j8 CFLAGS=-I$MANDELBULBER_PREFIX/include CPPFLAGS=-I$MANDELBULBER_PREFIX/include
sudo make -j8 CFLAGS=-I$MANDELBULBER_PREFIX/include CPPFLAGS=-I$MANDELBULBER_PREFIX/include install
cd ..

### openexr
sudo apt-get install cmake
git clone https://github.com/openexr/openexr
cd openexr
cd IlmBase
cmake -DCMAKE_HOST_SYSTEM=$MANDELBULBER_MINGW_HOST -DCMAKE_INSTALL_PREFIX=$MANDELBULBER_PREFIX CMakeLists.txt
make
sudo make install
cd ..
cd OpenEXR
cmake -DILMBASE_PACKAGE_PREFIX=$MANDELBULBER_PREFIX -DCMAKE_INSTALL_PREFIX=$MANDELBULBER_PREFIX CMakeLists.txt
make
sudo make install
cd ..
cd ..

# qtgamepad
git clone http://code.qt.io/cgit/qt-labs/qtgamepad.git
cd qtgamepad
$MANDELBULBER_PREFIX/bin/qmake
make -j8
sudo make -j8 install
cd ..

cd ..

## clean up
#rm -r libtemp
