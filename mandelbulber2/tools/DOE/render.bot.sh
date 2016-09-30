#!/bin/bash
# Define a timestamp function

SRC=$PWD


mics="mic0 mic1 mic2 mic3 mic4 mic5 mic6 mic7"
for mic in $mics
do
ssh root@$mic "ln -s /home/mandelbulber2/mandelbulber2/deploy/share/mandelbulber2 /usr/share/mandelbulber2"
done

mkdir -p $SRC/render
cd $SRC && python -u $SRC/host.py
