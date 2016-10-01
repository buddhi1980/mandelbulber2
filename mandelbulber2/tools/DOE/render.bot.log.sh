#!/bin/bash
# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

SRC=$PWD/../../../
DOE=$PWD

build_logs=/mnt/nfs/System/build_logs
filename=$build_logs/mandelbulber2-render_bot-$(timestamp).txt

mkdir -p $SRC/render


#2>&1 \

cd $DOE && \
sh -x $DOE/render.bot.sh \
| ts '[%Y%m%d%H%M%S]' \
| tee -a $filename
