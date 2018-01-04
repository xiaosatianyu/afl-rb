#!/bin/sh

AFL_FUZZ="../../afl-fuzz"
SEED="../seed"
OUT_PUT="/tmp/out-aflpara"
EXE_STR="../tiff2pdf-noasan @@ -o /dev/null"

mkdir -p $OUT_PUT

if [ "$1" = "0" ]; then
gdb --args  $AFL_FUZZ -i $SEED -o $OUT_PUT  -m none -M master  $EXE_STR 
else
gdb --args $AFL_FUZZ -i $SEED -o $OUT_PUT  -m none -S $1  $EXE_STR    
fi  
