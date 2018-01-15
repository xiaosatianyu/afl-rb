#!/bin/bash

TARGET=./binary/xmllint-foraflgo  #tiff2pdf-distance-noasan
AFL_HOME=../afl2.5.2
INPUT=`pwd`/input
OUTPUT=/tmp/output-afl2.5.2


# Create test environment for a CB
create_env () {
    if [ ! -d $INPUT ]; then
        cp -r seed  $INPUT
    fi

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
#$AFL_HOME/afl-fuzz -m none -i $INPUT -o $OUTPUT  -d $REAL_TARGET  @@ -o /dev/null
#gdb --args $AFL_HOME/afl-fuzz -m none -i $INPUT  -o $OUTPUT  -d  $REAL_TARGET @@ -o /dev/null

$AFL_HOME/afl-fuzz -m none -i $INPUT -o $OUTPUT  -d $REAL_TARGET --valid --recover @@
