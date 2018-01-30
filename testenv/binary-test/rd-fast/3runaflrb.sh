#!/bin/bash

AFL_HOME=`pwd`/../../../afl-rb
INPUT=`pwd`/input
OUTPUT=/tmp/output-fairfuzz
target="./xmllint --valid --recover  @@"

# Create test environment for a CB
create_env () {
    rm -rf $OUTPUT
    mkdir -p $OUTPUT
}

create_env



#$AFL_HOME/afl-fuzz -m none -i $INPUT -o $OUTPUT  -d $REAL_TARGET  @@ -o /dev/null
#gdb --args $AFL_HOME/afl-fuzz -m none -i $INPUT  -o $OUTPUT  -d  $REAL_TARGET @@ -o /dev/null

 $AFL_HOME/afl-fuzz -m none -i $INPUT -o $OUTPUT  -r -q 2 -d $target
