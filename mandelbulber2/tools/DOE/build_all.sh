#!/bin/bash

SRC=$PWD
KNC_LIB=$SRC/../KNC-Lib

# build libs #
echo $KNC_LIB
cd $KNC_LIB/freetype2/ && \
sh $KNC_LIB/freetype2/build.bot.log.sh
cd $KNC_LIB/libpng/ && \
sh $KNC_LIB/libpng/build.bot.log.sh
cd $KNC_LIB/pcre/ && \
sh $KNC_LIB/pcre/build.bot.log.sh
cd $KNC_LIB/gsl/ && \
sh $KNC_LIB/gsl/build.bot.log.sh
cd $KNC_LIB/qt5/ && \
sh $KNC_LIB/qt5/build.bot.log.sh

# build #
cd $SRC/ && \
sh $SRC/build.bot.log.sh
