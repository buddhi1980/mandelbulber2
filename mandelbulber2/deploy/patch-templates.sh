#!/bin/sh

IFS='\0'
for f in `find deploy -name '*.template' -print0`; do
  envsubst '$SHARED_PATH' < $f > ${f%.template}
done
