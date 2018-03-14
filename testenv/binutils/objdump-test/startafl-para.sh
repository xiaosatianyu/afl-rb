#!/bin/sh

PARA_FUZZ="`pwd`/../../../afl-fuzz"

SEED="`pwd`/seed"
OUT_PUT="/tmp/out-para-objdump"
EXE_STR="`pwd`/objdump2.26  -axd @@ "

if [ "$1" = "0" ]; then
rm -rf  $OUT_PUT
mkdir -p $OUT_PUT
  $PARA_FUZZ -i $SEED -o $OUT_PUT -r -q 2   -M master  $EXE_STR 
else
   $PARA_FUZZ -i $SEED -o $OUT_PUT  -r -q 2  -S $1  $EXE_STR    
fi  
