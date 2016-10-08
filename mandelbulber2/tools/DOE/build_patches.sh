#!/bin/bash

# Build Systems #
source $PWD/local.sh

# PATCHES #
PATCH1=$DOE/0001.patch
cd $DOE &&
wget \
https://github.com/buddhi1980/mandelbulber2/commit/898fd8a1db7ad789ebe6381a66046cf99ed1456a.patch \
--output-document=$PATCH1
PATCH2=$DOE/0002.patch
cd $DOE &&
wget \
https://github.com/buddhi1980/mandelbulber2/commit/c494aee5e9a7bccc8773437e775c96a43ec37e16.patch \
--output-document=$PATCH2
PATCH3a=$DOE/0003a.patch
cd $DOE &&
wget \
https://github.com/buddhi1980/mandelbulber2/commit/68cacbfbafe623030978d21b2e3ade36c1df7f3c.patch \
--output-document=$PATCH3a
PATCH3b=$DOE/0003b.patch
cd $DOE &&
wget \
https://github.com/buddhi1980/mandelbulber2/commit/56c6082db20fe2532d6b4b23c66b0149be58f81c.patch \
--output-document=$PATCH3b
# ARGS #
APPLYARG="--recount --ignore-space-change --ignore-whitespace -v"
WHITESPACE=$DOE/white-space-v2.08-1.patch

# Release Mandelbulber v2.08-1 - Jul 3, 2016 #
BASELINE=daec61022499faf1f75a8fa9d3bf5e861635c503

# Release Mandelbulber v2.09 - Oct 4, 2016 #
RELEASE=013208e3ee969c498a01b4160f834ad20f2a2f13

# Enumerate all the factors #
factors="1000 000 001 010 011 100 101 110 111"
for I in $factors
do
# Configurations #
echo FACTOR $I
FACTOR_DIR=$SRC/../FACTOR_$I
FACTOR_DIR_TOOLS=$FACTOR_DIR/mandelbulber2/tools
FACTOR_DIR_DOE=$FACTOR_DIR_TOOLS/DOE
FACTOR_BIN=$FACTOR_DIR/BIN
# SRC #
rm -rf $FACTOR_DIR
cp -rf $SRC $FACTOR_DIR
# BASELINE #
cd $FACTOR_DIR/ && \
git reset --hard $BASELINE
# WHITESPACE #
cd $FACTOR_DIR/ && \
git apply $APPLYARG $WHITESPACE
# Test $I for 001 011 101 111
if \
[ $I -eq 001 ] || \
[ $I -eq 011 ] || \
[ $I -eq 101 ] || \
[ $I -eq 111 ]
then
# APPLY 001 PATCH #
cd $FACTOR_DIR/ && git apply $APPLYARG $PATCH1
fi
# Test $I for 010 011 110 111 
if \
[ $I -eq 010 ] || \
[ $I -eq 011 ] || \
[ $I -eq 110 ] || \
[ $I -eq 111 ]
then
# APPLY 010 PATCH #
cd $FACTOR_DIR/ && git apply $APPLYARG $PATCH2
fi
# Test $I for 100 101 110 111
if \
[ $I -eq 100 ] || \
[ $I -eq 101 ] || \
[ $I -eq 110 ] || \
[ $I -eq 111 ]
then
# APPLY 100 PATCH #
cd $FACTOR_DIR/ && git apply $APPLYARG $PATCH3a
cd $FACTOR_DIR/ && git apply $APPLYARG $PATCH3b
fi
# Test $I for 1000
if \
[ $I -eq 1000 ]
then
# RELEASE #
cd $FACTOR_DIR/ && \
git reset --hard $RELEASE
fi
# BUILD #
mkdir -p $FACTOR_DIR_TOOLS
cp -rf $DOE $FACTOR_DIR_TOOLS
cd $FACTOR_DIR_DOE/ && \
sh $FACTOR_DIR_DOE/build.bot.log.sh
# COPY #
mkdir -p $FACTOR_BIN
cp -rf $BUILD $FACTOR_BIN/build
cp -rf $BUILDMIC $FACTOR_BIN/build-mic
done
# CLEANUP #
rm -rf $PATCH1
rm -rf $PATCH2
rm -rf $PATCH3a
rm -rf $PATCH3b

exit

