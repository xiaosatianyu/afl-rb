#!/bin/sh

AFL_FUZZ="/home/binzhang/git/afl-rb/afl-fuzz"
SEED="/home/binzhang/Desktop/tiff2pdf/seed"
OUT_PUT="/tmp/out-aflpara"
EXE_STR="/home/binzhang/Desktop/tiff2pdf/tiff2pdf @@ -o /dev/null"

rm -rf  $OUT_PUT
mkdir -p $OUT_PUT

if [ "$1" = "0" ]; then
$AFL_FUZZ -i $SEED -o $OUT_PUT  -m none -M master  $EXE_STR 
else
$AFL_FUZZ -i $SEED -o $OUT_PUT  -m none -S $1  $EXE_STR    
fi  
