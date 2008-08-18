SHELL = /bin/sh
                                                                                          
RM = /bin/rm
AR = ar cq
                                                                                          
TOP = ..
C++ = g++
CC  = gcc

ifeq ($(BUILD),debug)
CCFLAGS := -ggdb -Wall
else
CCFLAGS := -O3
#CCFLAGS := -pg
endif

#INCLUDE = -I. -I../include -I/usr/include/GL
INCLUDE = -I. -I$(TOP)/include -I$(TOP)/vcb/include  

LIB_PATH = -L$(TOP)/vcb/lib  -L/sw/lib

LIBS = -lvcb -lm

OBJS = binner.o binnerio.o bslicer.o cell.o  clip.o clip3d.o  compvol.o gencell.o geometry.o rebin.o rot3d.o scale3d.o tran3d.o volume.o voxelize.o

EXES = bslicer clip3d  compvol gencell rebin rot3d scale3d tran3d

.SUFFIXES: .cc .c .cpp

.cc.o:
	$(C++) $(CCFLAGS) $(INCLUDE) -c $<

.cpp.o:
	$(C++) $(CCFLAGS) $(INCLUDE) -c $<

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDE) -c $<

all: $(EXES)

gencell: gencell.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

rot3d: rot3d.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)
	
tran3d: tran3d.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

scale3d: scale3d.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

clip3d: clip3d.o clip.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

compvol: compvol.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

rebin: rebin.o binner.o voxelize.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

bslicer: bslicer.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)
	
install: all
	mv $(EXES) $(TOP)/bin

clean:
	rm -f *.o $(EXES) *.a *~
