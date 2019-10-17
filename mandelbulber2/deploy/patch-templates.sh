#!/bin/sh

# $1 is deploy directory, $2 is SHARED_PATH, or /usr if unset
if [ -z "$1" ]; then
  echo "Usage: $0 <path to mandelbulber2/deploy directory> [SHARED_PATH]"
  exit 1
fi

export SHARED_PATH=${2:-/usr}

# Names can contain spaces but hopefully they don't contain newlines
IFS='
'
for f in `find "$1" -name '*.template'`; do
  envsubst '$SHARED_PATH' < "$f" > "${f%.template}"
done
