#!/bin/bash
# iterates 100 times
i="0"

DOE=$PWD

while [ $i -lt 100 ]
do
 cd $DOE
 sh $DOE/trial.log.sh
 i=$[$i+1]
 sleep 5
 #ready
done
