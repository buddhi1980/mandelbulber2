#!/bin/bash
# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

SRC=$PWD
build_logs=/mnt/nfs/System/build_logs
filename=$build_logs/mandelbulber2-NaNtester-$(timestamp).txt

cd $SRC && sh -x $SRC/NaNtester.sh \
2>&1 \
| tee -a $filename
