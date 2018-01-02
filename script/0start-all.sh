#!/bin/bash
if [ $# -eq 0 ]; then
	echo "need a para"
	exit
fi

x-terminal-emulator -e ./start-master.sh &
sleep 1

num=$1
while [ $num -ge 1 ]
do 
	echo $num
	x-terminal-emulator -e ./start-slave.sh $num &
	num=$((num-1))
done
