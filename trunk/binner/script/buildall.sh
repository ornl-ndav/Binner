#!/bin/sh


#build
cd ../vcb/src
make
make -f makefile_withgl
make clean

cd ../libtestsrc
make
make install
make clean

cd ../../src
make all
make install
make -f makefile_withgl seeall
mv seeall ../bin
make clean

exit 0
