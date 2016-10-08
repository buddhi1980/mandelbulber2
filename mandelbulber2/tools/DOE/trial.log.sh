#!/bin/bash
# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

# Build Systems #
source $PWD/local.sh

filename=$build_logs/mandelbulber2-trial-mic-$(timestamp).txt

mkdir -p $SRC/render


cd $DOE && \
sh -x $DOE/trial.sh \
| ts '[%Y%m%d%H%M%S]' \
| tee -a $filename
