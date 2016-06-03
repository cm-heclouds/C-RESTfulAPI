#!/bin/sh

rm -rf Output/*
make clean
make

if [ ! -f Lib/libRestFulLinux.a ];then
    exit
fi

if [ ! -f Lib/libosAdaption.a ];then
    exit
fi

cp Lib/*.a Output/

if [ ! -d Output/include ];then
    mkdir Output/include
fi

cp Inc/RF*.h Output/include/

if [ ! -d Output/include/osAdaption ];then
    mkdir Output/include/osAdaption
fi

#cp Inc/osAdaption/os*.h Output/include/osAdaption
#cp Inc/osAdaption/OSAErrno.h Output/include/osAdaption
#cp Inc/osAdaption/OSASocket.h Output/include/osAdaption
cp Inc/osAdaption/*.h Output/include/osAdaption

doxygen doxygen.conf

