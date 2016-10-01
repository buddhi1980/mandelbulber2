#!/bin/bash

SRC=$PWD/../../../
DOE=$PWD
BUILDTREE=$SRC/../build-mic/mandelbulber2
BUILDTREE64=$SRC/../build/mandelbulber2
HOSTIP=192.168.1.70
SETTINGS=$SRC/mandelbulber2/deploy/share/mandelbulber2/examples/aexion001.fract

# Systems #
gnome-terminal --command="ssh mic0 $BUILDTREE/mandelbulber2 --nogui -n --host $HOSTIP --no-cli-color"
gnome-terminal --command="ssh mic1 $BUILDTREE/mandelbulber2 --nogui -n --host $HOSTIP --no-cli-color"
gnome-terminal --command="ssh mic2 $BUILDTREE/mandelbulber2 --nogui -n --host $HOSTIP --no-cli-color"
gnome-terminal --command="ssh mic3 $BUILDTREE/mandelbulber2 --nogui -n --host $HOSTIP --no-cli-color"
gnome-terminal --command="ssh mic4 $BUILDTREE/mandelbulber2 --nogui -n --host $HOSTIP --no-cli-color"
gnome-terminal --command="ssh mic5 $BUILDTREE/mandelbulber2 --nogui -n --host $HOSTIP --no-cli-color"
gnome-terminal --command="ssh mic6 $BUILDTREE/mandelbulber2 --nogui -n --host $HOSTIP --no-cli-color"
gnome-terminal --command="ssh mic7 $BUILDTREE/mandelbulber2 --nogui -n --host $HOSTIP --no-cli-color"

sleep 15


gnome-terminal --command="$BUILDTREE64/mandelbulber2 --server $SETTINGS" 



