#!/bin/csh -f


cd ../bin
rm  compvol rebin seeall bslicer rot3d clip3d gencell scale3d tran3d
rm *.d

cd ../src
make clean

cd ../vcb/bin
rm vcbcomp vcbcut vcbhead vcbrange vcbrcast

rm ../lib/*.a

cd ../src
make clean

cd ../libtestsrc
make clean

