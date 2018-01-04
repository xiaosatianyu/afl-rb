#!/bin/sh

AFL_FUZZ=./fairfuzz
OUT_OUT=/tmp/out-fairfuzz
SEED=../seed


mkdir -p $OUT_OUT

if [ "$1" = "1" ]; then
$AFL_FUZZ -i $SEED -o $OUT_OUT  -m none -S $1 ../tiff2pdf @@ -o /dev/null
else
$AFL_FUZZ -i $SEED -o $OUT_OUT  -m none -S $1 ../tiff2pdf @@ -o /dev/null
fi
