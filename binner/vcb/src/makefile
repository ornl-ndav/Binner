SHELL = /bin/sh
                                                                                          
LIBNAME = vcb
RM = /bin/rm
AR = ar cq
                                                                                          
TOP = ..
C++ = g++
CC  = gcc


ifeq ($(BUILD),debug)
CCFLAGS := -g
else
CCFLAGS := -O3
#CCFLAGS := -pg
endif
                                                                                          
INCLUDE = -I. -I../include -I../Task -I/usr/include/GL
                                                                                          
LIBS = -lm

OBJS = jval.o jrb.o dmem.o exhash.o fields.o hilbert.o intersect.o kernel.o nr.o nrcomplex.o nrutil.o raycasting.o vcbdebug.o vcbfitting.o vcbimage.o vcblinalg.o vcbmath.o vcbmcube.o vcbops.o vcbspcurve.o vcbtrimesh.o vcbutils.o vcbvolren.o zcurve.o vcbcolor.o vbutrcast.o vcblic.o Lic.o slic.o slic_aux.o

.SUFFIXES: .cc .c .cpp
                                                                                          
.cc.o:
	$(C++) $(CCFLAGS) $(INCLUDE) -c $<
                                                                                          
.cpp.o:
	$(C++) $(CCFLAGS) $(INCLUDE) -c $<
                                                                                          
.c.o:
	$(CC) $(CCFLAGS) $(INCLUDE) -c $<
                                                                                          
all: lib$(LIBNAME).a
dynamic: lib$(LIBNAME).so
                                                                                          
lib$(LIBNAME).a : $(OBJS)
	$(RM) -f $@
	libtool -static -o $@ $(OBJS)
#	$(AR) $@ $(OBJS)
	mv $@ ../lib

lib$(LIBNAME).so : $(OBJS)
	$(RM) -f $@
	ld -G $(OBJS) -o $@

clean:
	rm -f *.o *.a *~
