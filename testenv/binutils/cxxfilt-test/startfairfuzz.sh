#!/bin/sh

FAIRFUZZ="`pwd`/../../../fairfuzz/afl-fuzz"

SEED="`pwd`/seed"
OUT_PUT="/tmp/out-fairfuzz-cxxfilt"
EXE_STR="`pwd`/cxxfilt2.26 "

if [ "$1" = "0" ]; then
rm -rf  $OUT_PUT
mkdir -p $OUT_PUT
  $FAIRFUZZ -i $SEED -o $OUT_PUT -r -q 2   -M master  $EXE_STR 
else
   $FAIRFUZZ -i $SEED -o $OUT_PUT  -r -q 2  -S $1  $EXE_STR    
fi  
