#!/bin/csh -f

rm ../*~
rm ../vcb/*~

cd ../bin
rm  compvol rebin seeall bslicer rot3d clip3d gencell scale3d tran3d collector getfrom giveto netrebin sorter pix2mesh seepara serve bmeshrebin seebmeshvol seebmeshvolc bounds counts export2vcb export2vtk largebmeshrebin
rm *~
rm *.d
rm -rf MR*

cd ../script
rm *~
rm -rf MR*

cd ../src
make clean

cd ../vcb/bin
rm vcbcomp vcbcut vcbhead vcbrange vcbrcast

rm ../lib/*.a

cd ../src
make clean

cd ../libtestsrc
make clean

