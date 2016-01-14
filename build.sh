#/bin/sh
PWD=`pwd`
./autogen.sh && ./configure --prefix=${PWD}/out --enable-debug --enable-unicode --enable-tests && make -j `nproc`  && make install


