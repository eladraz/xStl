#!/bin/sh
PWD=`pwd`
./autogen.sh && ./configure --prefix=${PWD}/out --enable-debug --enable-unicode --enable-tests && make -j `nproc 2>/dev/null || sysctl -n hw.ncpu`  && make install


