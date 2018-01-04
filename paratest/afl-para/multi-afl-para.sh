#!/bin/bash

if [ $# -eq 0 ]; then	
	echo "need a num para"	
	exit
fi
num=$1

x-terminal-emulator -e "`pwd`/startafl-para.sh 0 " &
sleep 5
while [ $num -ge 1 ]
do
	echo $num
	x-terminal-emulator -e "`pwd`/startafl-para.sh $num " &
	num=$((num-1))
done



