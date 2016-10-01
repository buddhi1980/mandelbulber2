#!/bin/bash
# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

SRC=$PWD/../../../
DOE=$PWD
build_logs=/mnt/nfs/System/build_logs
filename=$build_logs/mandelbulber2-trial-mic-$(timestamp).txt

mkdir -p $SRC/render


cd $DOE && \
sh -x $DOE/trial.sh \
| ts '[%Y%m%d%H%M%S]' \
| tee -a $filename
