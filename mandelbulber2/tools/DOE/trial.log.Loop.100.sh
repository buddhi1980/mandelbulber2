#!/bin/bash
# iterates 100 times
i="0"

while [ $i -lt 100 ]
do
 cd /home/mandelbulber2
 sh ./trial.log.sh
 i=$[$i+1]
 sleep 5
 #ready
done
