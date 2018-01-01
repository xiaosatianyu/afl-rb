#!/bin/bash
x-terminal-emulator -e ./start-master.sh &
sleep 4
x-terminal-emulator -e ./start-slave.sh 1 &
x-terminal-emulator -e ./start-slave.sh 2 &
x-terminal-emulator -e ./start-slave.sh 3 &
