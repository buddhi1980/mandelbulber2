#!/bin/bash

# Build Systems #
source $PWD/local.sh

# iterates 10 times
i="0"

while [ $i -lt 10 ]
do
 cd $DOE
 sh $DOE/trial.log.sh
 i=$[$i+1]
 sleep 5
 #ready
done
