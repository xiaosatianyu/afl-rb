#!/bin/bash

TARGET=tiff2pdf-distance-noasan
AFL_HOME=../
INPUT=`pwd`/input/
OUTPUT=`pwd`/output/


# Create test environment for a CB
create_env () {
    #if [ ! -d $INPUT ]; then
    #   mkdir -p $INPUT
    #fi

    rm -rf $INPUT
    cp -r seed  $INPUT

    #if [ ! -d $OUTPUT ]; then
    #    mkdir -p $OUTPUT
    #fi

    rm -rf $OUTPUT
    mkdir -p $OUTPUT
}

create_env

REAL_TARGET=./$TARGET
if [ ! -f $REAL_TARGET ]; then
    if [ ! -f "${REAL_TARGET}_1" ]; then
        echo "No target found"
        exit
    fi
  
fi

echo $REAL_TARGET
#$AFL_HOME/afl-fuzz -m none -i $INPUT -d -o $OUTPUT -r -q 2 $REAL_TARGET -o /dev/null @@
gdb --args $AFL_HOME/afl-fuzz -m none -i $INPUT -d -o $OUTPUT -r -q 2 $REAL_TARGET -o /dev/null @@
