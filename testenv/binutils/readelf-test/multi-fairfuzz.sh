#!/bin/bash

if [ $# -eq 0 ]; then	
	echo "need a num para"
	exit
fi
num=$1

x-terminal-emulator -e "`pwd`/startfairfuzz.sh 0" &
num=$((num-1))
while [ $num -ge 1 ]
do
	x-terminal-emulator -e "`pwd`/startfairfuzz.sh $num " &	
	num=$((num-1))
done



