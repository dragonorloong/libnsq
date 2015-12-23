#!/bin/bash
CUR_DIR=$(cd `dirname $0`;pwd)
mkdir -p 3rd
git clone git@github.com:nmathewson/Libevent.git 3rd/libevent
git clone git@github.com:open-source-parsers/jsoncpp.git 3rd/jsoncpp
cd 3rd/libevent
./autogen.sh
echo "prefix = ${CUR_DIR}/3rd/libevent/lib"
./configure --prefix=${CUR_DIR}/3rd/libevent/lib
make
make install
cd ../jsoncpp
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DARCHIVE_INSTALL_DIR=../../lib   -DCMAKE_INSTALL_PREFIX=../../lib/ -G "Unix Makefiles" ../..
make
make install
cd ../../../../
CUR_DIR=$(cd `dirname $0`;pwd)
echo "prefix = ${CUR_DIR}/3rd/libevent/lib"
make
make -f Makefile.bin clean
make -f Makefile.bin
