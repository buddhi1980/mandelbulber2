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
#MANDELBULBER_QT_URL="http://download.qt.io/archive/qt/5.6/5.6.1-1/single/qt-everywhere-opensource-src-5.6.1-1.tar.gz"
MANDELBULBER_QT_URL="http://download.qt.io/official_releases/qt/5.7/5.7.0/single/qt-everywhere-opensource-src-5.7.0.tar.gz"
MANDELBULBER_WIN_VERSION=$1

if [ $MANDELBULBER_WIN_VERSION -eq "64" ]
then
	MANDELBULBER_MINGW_HOST="x86_64-w64-mingw32"
	MANDELBULBER_PREFIX="/usr/local/mingw-64bit"
else
	MANDELBULBER_MINGW_HOST="i686-w64-mingw32"
	MANDELBULBER_PREFIX="/usr/local/mingw-32bit"
fi

set -e # if any of the commands fail the script will exit immediately

sudo apt-get install git make g++ dh-autoreconf
sudo apt-get install mingw-w64
mkdir -p $MANDELBULBER_PREFIX

### qt
if [ ! -f qt-everywhere-opensource-src-5.7.0.tar.gz ]; then
	wget $MANDELBULBER_QT_URL
fi

if [ ! -d qt-everywhere-opensource-src-5.7.0 ]; then
	tar xf qt-everywhere-*.tar.gz
	cd qt-everywhere-*
	yes | ./configure -release -xplatform win32-g++ -opengl desktop -nomake examples -device-option \
		CROSS_COMPILE=$MANDELBULBER_MINGW_HOST- -prefix $MANDELBULBER_PREFIX -opensource \
		-skip qtactiveqt -skip qtcanvas3d
	make -j3
	sudo make -j3 install
	cd ..
fi

### gsl
if [ ! -f gsl-latest.tar.gz ]; then
        wget ftp://ftp.gnu.org/gnu/gsl/gsl-latest.tar.gz
fi

if [ ! -d gsl-* ]; then
        tar xf gsl-latest.tar.gz
	cd gsl-*
	./configure --host=$MANDELBULBER_MINGW_HOST -prefix=$MANDELBULBER_PREFIX
	make -j8
	sudo make -j8 install
	cd ..
fi

### zlib
if [ ! -d zlib-* ]; then
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
fi

### libpng
if [ ! -f libpng.tar.gz ]; then
	wget -O libpng.tar.gz http://download.sourceforge.net/libpng/libpng-1.6.25.tar.gz
fi

if [ ! -d libpng-* ]; then
	
	tar xf libpng.tar.gz
	#apt-get source libpng
	cd libpng-*
	./configure --host=$MANDELBULBER_MINGW_HOST -prefix=$MANDELBULBER_PREFIX LDFLAGS=-L$MANDELBULBER_PREFIX/lib
	make -j8 CFLAGS=-I$MANDELBULBER_PREFIX/include CPPFLAGS=-I$MANDELBULBER_PREFIX/include
	sudo make -j8 CFLAGS=-I$MANDELBULBER_PREFIX/include CPPFLAGS=-I$MANDELBULBER_PREFIX/include install
	cd ..
fi

### openexr
sudo apt-get install cmake
if [ ! -d openexr ]; then
	git clone https://github.com/openexr/openexr
	cd openexr
	
	cd IlmBase
	cmake -DCMAKE_SYSTEM=$MANDELBULBER_MINGW_HOST \
		-DZLIB_INCLUDE_DIR=$MANDELBULBER_PREFIX/include \
		-DZLIB_ROOT=$MANDELBULBER_PREFIX/lib \
		-DCMAKE_INSTALL_PREFIX=$MANDELBULBER_PREFIX CMakeLists.txt
	make LIBRARY_PATH=$MANDELBULBER_PREFIX/lib
	sudo make install
	cd ..

	cd OpenEXR
	cmake -DCMAKE_SYSTEM=$MANDELBULBER_MINGW_HOST \
		-DILMBASE_PACKAGE_PREFIX=$MANDELBULBER_PREFIX \
		-DZLIB_INCLUDE_DIR=$MANDELBULBER_PREFIX/include \
		-DZLIB_ROOT=$MANDELBULBER_PREFIX/lib \
		-DCMAKE_INSTALL_PREFIX=$MANDELBULBER_PREFIX CMakeLists.txt
	make LIBRARY_PATH=$MANDELBULBER_PREFIX/lib
	sudo make install
	cd ..
	cd ..
fi

# qtgamepad
if [ ! -d qtgamepad ]; then
	git clone https://github.com/qtproject/qtgamepad
	cd qtgamepad
	$MANDELBULBER_PREFIX/bin/qmake
	make -j8
	sudo make -j8 install
	cd ..
fi

# tiff
if [ ! -d tiff-* ]; then
	apt-get source tiff
	cd tiff-*
	./configure --host=$MANDELBULBER_MINGW_HOST -prefix=$MANDELBULBER_PREFIX \
	    --with-zlib-include-dir=$MANDELBULBER_PREFIX/include \
	    --with-zlib-lib-dir=$MANDELBULBER_PREFIX/lib
	make
	sudo make install
	cd ..
fi

# sndfile
if [ ! -f libsndfile.tar.gz ]; then
	wget -O libsndfile.tar.gz http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.27.tar.gz
fi

if [ ! -d libsndfile-* ]; then
	tar xf libsndfile.tar.gz
	cd libsndfile-*
	./configure --host=$MANDELBULBER_MINGW_HOST -prefix=$MANDELBULBER_PREFIX
	make
	sudo make install
	cd ..
fi


echo Finished
## clean up
#rm -r libtemp
