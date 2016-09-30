#!/bin/bash
# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

SRC=$PWD
build_logs=/mnt/nfs/System/build_logs
filename=$build_logs/mandelbulber2-trial-mic-$(timestamp).txt

mkdir -p $SRC/render


#2>&1 \

cd $SRC && \
sh -x $SRC/trial.sh \
| ts '[%Y%m%d%H%M%S]' \
| tee -a $filename
