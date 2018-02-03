#!/bin/bash

#开启rarity
#关闭distance ; -k 表示开启distance_mask
#-s 表示shadow模式

AFL_HOME=`pwd`/../../../
INPUT=`pwd`/input
OUTPUT=/tmp/output-rd5
target="./xmllint --valid --recover  @@"

# Create test environment for a CB
create_env () {
    rm -rf $OUTPUT
    mkdir -p $OUTPUT
}
create_env

gdb --args $AFL_HOME/afl-fuzz -i $INPUT -o $OUTPUT -m none -r -q 2  -s  $target
