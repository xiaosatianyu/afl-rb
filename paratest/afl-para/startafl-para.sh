#!/bin/sh

AFL_FUZZ="../../afl-fuzz"
SEED="../seed"
OUT_PUT="/tmp/out-aflpara"
EXE_STR="../tiff2pdf @@ -o /dev/null"

rm -rf  $OUT_PUT
mkdir -p $OUT_PUT

if [ "$1" = "0" ]; then
$AFL_FUZZ -i $SEED -o $OUT_PUT  -m none -M master  $EXE_STR 
else
$AFL_FUZZ -i $SEED -o $OUT_PUT  -m none -S $1  $EXE_STR    
fi  
