#!/bin/sh


#build
cd ../vcb/src
make
make clean

cd ../libtestsrc
make
make install
make clean

cd ../../src
make all
make install
make clean

exit 0
