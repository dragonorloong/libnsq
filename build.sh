#!/bin/bash
CUR_DIR=$(cd `dirname $0`;pwd)
#mkdir -p 3rd
#git clone https://github.com/nmathewson/Libevent.git 3rd/libevent
#git clone https://github.com/open-source-parsers/jsoncpp.git 3rd/jsoncpp
cd 3rd/libevent
./autogen.sh
echo "prefix = ${CUR_DIR}/3rd/libevent/lib"
./configure --prefix=${CUR_DIR}/3rd_install
make
make install
cd ../jsoncpp
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DARCHIVE_INSTALL_DIR=../../../../3rd_install   -DCMAKE_INSTALL_PREFIX=../../../../3rd_install -G "Unix Makefiles" ../..
make
make install
mv ${CUR_DIR}/3rd_install/libjsoncpp.a ${CUR_DIR}/3rd_install/lib/
cd ../../../../
CUR_DIR=$(cd `dirname $0`;pwd)
make
cd nsq_test
make 
