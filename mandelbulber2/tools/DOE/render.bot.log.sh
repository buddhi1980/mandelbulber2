#!/bin/bash
# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

# Build Systems #
source $PWD/local.sh

filename=$build_logs/mandelbulber2-render_bot-$(timestamp).txt

mkdir -p $SRC/render

cd $DOE && \
sh -x $DOE/render.bot.sh \
| ts '[%Y%m%d%H%M%S]' \
| tee -a $filename
