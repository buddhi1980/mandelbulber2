#!/bin/bash

SRC=$PWD

# Systems #
gnome-terminal --command="ssh mic0 /home/mandelbulber2/build-mic/mandelbulber2 --nogui -n --host 192.168.1.70 --no-cli-color"
gnome-terminal --command="ssh mic1 /home/mandelbulber2/build-mic/mandelbulber2 --nogui -n --host 192.168.1.70 --no-cli-color"
gnome-terminal --command="ssh mic2 /home/mandelbulber2/build-mic/mandelbulber2 --nogui -n --host 192.168.1.70 --no-cli-color"
gnome-terminal --command="ssh mic3 /home/mandelbulber2/build-mic/mandelbulber2 --nogui -n --host 192.168.1.70 --no-cli-color"
gnome-terminal --command="ssh mic4 /home/mandelbulber2/build-mic/mandelbulber2 --nogui -n --host 192.168.1.70 --no-cli-color"
gnome-terminal --command="ssh mic5 /home/mandelbulber2/build-mic/mandelbulber2 --nogui -n --host 192.168.1.70 --no-cli-color"
gnome-terminal --command="ssh mic6 /home/mandelbulber2/build-mic/mandelbulber2 --nogui -n --host 192.168.1.70 --no-cli-color"
gnome-terminal --command="ssh mic7 /home/mandelbulber2/build-mic/mandelbulber2 --nogui -n --host 192.168.1.70 --no-cli-color"

sleep 15


gnome-terminal --command="/home/mandelbulber2/build/mandelbulber2 --server /home/mandelbulber2/sponged.fract" 



