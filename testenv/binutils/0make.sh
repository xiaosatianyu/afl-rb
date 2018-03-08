cd binutils-2.26
make distclean
export CC=`pwd`/../../../afl-clang-fast
export CXX=`pwd`/../../../afl-clang-fast++ 
./configure
make  -j20
