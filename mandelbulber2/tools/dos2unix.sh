#!/bin/sh
#
# this file runs dos2unix on all source code files
#
# this fixes:
# line endings: CRLF -> LF
# remove BOM from utf files

set -e # if any of the commands fail the script will exit immediately

BASEDIR=$(dirname "$0")

dos2unix $BASEDIR/../src/*.cpp
dos2unix $BASEDIR/../src/*.hpp
# dos2unix $BASEDIR/../src/*.c
dos2unix $BASEDIR/../src/*.h

dos2unix $BASEDIR/../qt/*.cpp
dos2unix $BASEDIR/../qt/*.hpp
#dos2unix $BASEDIR/../qt/*.c
dos2unix $BASEDIR/../qt/*.h

dos2unix $BASEDIR/../opencl/*.hpp
dos2unix $BASEDIR/../opencl/*.h

dos2unix $BASEDIR/../formula/opencl/*.cl
dos2unix $BASEDIR/../formula/ui/*.ui
