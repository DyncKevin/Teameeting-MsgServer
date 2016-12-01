#!/usr/bin/sh

###g++ -o main main.cpp -I../build_out/include -l../build_out/lib/libzksdk.so -fPIC -shared
g++ -o main main.cpp -I../build_out/include -I../include -I../include/common/include -I../include/zk_include -L. -lzksdk -L../lib -lzkclient -L../lib/zk_lib -lzookeeper_mt -L/usr/local/lib -ljsoncpp -lpthread -g
