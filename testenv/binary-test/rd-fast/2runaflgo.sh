#!/bin/bash

AFL_HOME=`pwd`/../../../aflgo
INPUT=`pwd`/input
OUTPUT=/tmp/output-aflgo
target="./xmllint --valid --recover  @@"

# Create test environment for a CB
create_env () {
    
    rm -rf $OUTPUT
    mkdir -p $OUTPUT
}

create_env



#$AFL_HOME/afl-fuzz -m none -i $INPUT -o $OUTPUT  -d $REAL_TARGET  @@ -o /dev/null
#gdb --args $AFL_HOME/afl-fuzz -m none -i $INPUT  -o $OUTPUT  -d  $REAL_TARGET @@ -o /dev/null

 $AFL_HOME/afl-fuzz -m none -i $INPUT -o $OUTPUT   -z exp -c 240m -d $target
