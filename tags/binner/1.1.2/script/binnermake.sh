#!/bin/sh

# README:
#
# RUN IN A CLEAN AND SAFE DIRECTORY ONLY.
#
# THIS SCRIPT GRABS THE LATEST FILES IN SVN AND BUILDS
# THE ENTIRE BINNER DIRECTORY

# THIS SCRIPT OVERWRITES FILES AND SUB-DIECTORIES
# BEFORE BUILDING BINNER, VCB AND LDFR LIBRIRIES.
#
# AT PRESENT, VCB AND LDFR ARE BUILT AS ONE PACKAGE
# ALTHOUGH THEY ARE INDEPENDENT OF EACH OTHER.
#
# THIS SCRIPT IS MODIFIED FROM A SIMILAR SCRIPT FOR
# VCBLIB. THAT SCRIPT WAS CALL LIB_MAKE, FIRST WRITTEN 
# BY CHAD JONES AND LATER MAINTAINED BY COLIN MOLLENHOUR.
#
# END OF README.

#default values
build="release"
checkout="yes"
opengl="OpenGL"

#check arguments
for arg ; do
  if [ $arg = "debug" ] ; then
    build="debug"
  elif [ $arg = "noco" ] ; then
    checkout="no"
  elif [ $arg = "noogl" ] ; then
    opengl="no-OpenGL"
  else
    echo "Usage: lib_make [debug] [noco] [noogl]" >&2
    echo "The \"noco\" option will prevent checking code out again." >&2
    echo "The \"debug\" option will cause the library to be compiled with -g."
    echo "The \"noogl\" option will cause opengl related part to be excluded."
	echo "Options can be added in any order."
    exit 1
  fi
done

#report settings to be used
echo "Build mode: $build, $checkout, $opengl" >&2
echo "SVN Checkout: $checkout" >&2


#build
cd vcb/src
if [ opengl = "OpenGL" ] ; then
  make -f makefile_withgl
else
  make
fi

cd ../../src
make all
make install
make clean

exit 0
