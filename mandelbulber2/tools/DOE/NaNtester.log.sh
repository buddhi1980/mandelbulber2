#!/bin/bash
# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

# Build Systems #
source $PWD/local.sh

filename=$build_logs/mandelbulber2-NaNtester-$(timestamp).txt

cd $DOE && sh -x $DOE/NaNtester.sh \
2>&1 \
| tee -a $filename
