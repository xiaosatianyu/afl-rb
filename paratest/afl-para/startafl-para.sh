#!/bin/sh

AFL_FUZZ="../../afl-fuzz"
SEED="../seed"
OUT_PUT="/tmp/out-aflpara"
EXE_STR="../tiff2pdf-noasan @@ -o /dev/null"

if [ "$1" = "0" ]; then
rm -rf  $OUT_PUT
mkdir -p $OUT_PUT
gdb --args  $AFL_FUZZ -i $SEED -o $OUT_PUT  -m none -M master  $EXE_STR 
else
gdb --args  $AFL_FUZZ -i $SEED -o $OUT_PUT  -m none -S $1  $EXE_STR    
fi  
