#!/bin/bash

# Build Systems #
source $PWD/local.sh

cd $SRC && git rev-parse HEAD

# Incremental Build #
INCREMENTDISABLE=TRUE

# XEON PHI Build #
echo "XEON PHI Build"
BUILDTREE=$BUILDMIC/mandelbulber2

# Clean Build Tree #
if test ${INCREMENTDISABLE+defined}; then
echo "Incremental Build disabled"
rm -rf $BUILDTREE
else
echo "Incremental Build enabled"
fi
mkdir -p $BUILDTREE

# Build k1om #
cd $BUILDTREE \
&& CC=icc CXX=icpc \
CXXFLAGS="-03 -g -fPIC -mmic -wd39,10006" \
CFLAGS="-03 -g -fPIC -mmic -wd39,10006" \
cmake3 \
-DCMAKE_PREFIX_PATH=$KNC_LIB/qt5/build-mic/qtbase/lib/cmake \
-DQt5UiTools_DIR=$KNC_LIB/qt5/build-mic/qttools/lib/cmake/Qt5UiTools \
-DQt5Multimedia_DIR=$KNC_LIB/qt5/build-mic/qtmultimedia/lib/cmake/Qt5Multimedia \
-DCMAKE_SYSTEM_NAME=Linux \
-DCMAKE_SYSTEM_PROCESSOR=k1om \
-DCMAKE_FIND_ROOT_PATH=$MPSSDIR \
-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
-DPNG_LIBRARY=$KNC_LIB/libpng/install-mic/lib/libpng15.so.15.27.0 \
-DPNG_PNG_INCLUDE_DIR=$KNC_LIB/libpng/install-mic/include \
-DJPEG_LIBRARY=$KNC_LIB/libjpeg/build-mic/libjpeg.a \
-DJPEG_INCLUDE_DIR=$KNC_LIB/libjpeg \
-DGSL_INCLUDE_DIR=$KNC_LIB/gsl/install-mic/include \
-DGSL_LIBRARY=$KNC_LIB/gsl/install-mic/lib/libgsl.a \
-DGSL_CBLAS_LIBRARY=$KNC_LIB/gsl/install-mic/lib/libgslcblas.a \
-DCMAKE_INSTALL_PREFIX=$SRC/install-mic \
-G "Eclipse CDT4 - Unix Makefiles" \
$SRC/mandelbulber2/cmake/
cd $BUILDTREE && make -j12 VERBOSE=1

# XEON x64 Build #
BUILDTREE=$BUILD/mandelbulber2

# Clean Build Tree #
if test ${INCREMENTDISABLE+defined}; then
echo "Incremental Build disabled"
rm -rf $BUILDTREE
else
echo "Incremental Build enabled"
fi
mkdir -p $BUILDTREE

# Build x64 #
cd $BUILDTREE \
&& CC=icc CXX=icpc \
CXXFLAGS="-03 -g -fPIC -wd39,10006" \
CFLAGS="-03 -g -fPIC -wd39,10006" \
cmake3 \
-DCMAKE_PREFIX_PATH=/opt/Qt5.7.0/5.7/gcc_64/lib/cmake \
-DQt5UiTools_DIR=/opt/Qt5.7.0/5.7/gcc_64/lib/cmake/Qt5UiTools \
-DUSE_GAMEPAD=1 \
-G "Eclipse CDT4 - Unix Makefiles" \
$SRC/mandelbulber2/cmake/
cd $BUILDTREE && make -j12 VERBOSE=1
# error using icc w/ cmake # call make twice
cd $BUILDTREE && make -j12 VERBOSE=1

# Deploy support 
ln -s $SRC/mandelbulber2/deploy/share/mandelbulber2 /usr/share/mandelbulber2

exit

