#!/bin/bash


export CurDir=`pwd`
export AFLGO=$CurDir/../../../aflgo
export SUBJECT=$CurDir/binutils-2.25
export TMP_DIR=$CurDir/temp_aflgo


#重新将距离编译进去
export CC=$AFLGO/afl-clang-fast
export CXX=$AFLGO/afl-clang-fast++
export CFLAGS="$COPY_CFLAGS -distance=$TMP_DIR/distance.cfg.txt  -hittargets=$TMP_DIR/hittargets.txt"
export CXXFLAGS="$COPY_CXXFLAGS -distance=$TMP_DIR/distance.cfg.txt  -hittargets=$TMP_DIR/hittargets.txt"
cp $CurDir/hittargets.txt $TMP_DIR

# 重新生成
cd $SUBJECT
./configure --disable-shared
make clean
AFL_DONT_OPTIMIZE=1  AFL_USE_ASAN=1 make -j8
cp $SUBJECT/binutils/readelf $TMP_DIR/readelf-asan

##得到执行程序 加asan
make clean
AFL_DONT_OPTIMIZE=1   make -j8
cp $SUBJECT/binutils/readelf $TMP_DIR/readelf-no-asan

