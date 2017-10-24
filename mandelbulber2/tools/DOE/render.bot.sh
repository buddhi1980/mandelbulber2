#!/bin/bash

# Build Systems #
source $PWD/local.sh

SETTINGS=$SRC/mandelbulber2/deploy/share/mandelbulber2

mics="mic0 mic1 mic2 mic3 mic4 mic5 mic6 mic7"
for mic in $mics
do
ssh root@$mic "ln -s $SETTINGS /usr/share"
ssh root@$mic "ln -s $SETTINGS /root"
done

mkdir -p $SRC/render
cd $DOE && python -u $DOE/host.py
