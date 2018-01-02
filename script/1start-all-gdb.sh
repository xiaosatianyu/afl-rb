#!/bin/bash
if [ $# -eq 0 ]; then
	exit
fi

x-terminal-emulator -e ./start-master-gdb.sh &
sleep 4

num=$1
while [ $num -ge 1 ]
do 
	x-terminal-emulator -e ./start-slave-gdb.sh $num &
	num=$((num-1))
done
