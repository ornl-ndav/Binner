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

INCLUDE = -I. -I$(TOP)/include -I$(TOP)/vcb/include  

LIB_PATH = -L$(TOP)/vcb/lib  -L/sw/lib

LIBS =  -lpthread -lvcb  -lm

OBJS = binner.o binnerio.o bmeshrebin.o bslicer.o cell.o clip.o clip3d.o collector.o compvol.o gencell.o pix2mesh.o geometry.o getfrom.o giveto.o netrebin.o rebin.o rot3d.o scale3d.o serve.o socketfun.o sorter.o tran3d.o volume.o voxelize.o

EXES = bmeshrebin bslicer clip3d  compvol gencell pix2mesh rebin rot3d scale3d tran3d collector getfrom giveto serve netrebin sorter

.SUFFIXES: .cc .c .cpp

.cc.o:
	$(C++) $(CCFLAGS) $(INCLUDE) -c $<

.cpp.o:
	$(C++) $(CCFLAGS) $(INCLUDE) -c $<

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDE) -c $<

all: $(EXES)

sorter: sorter.o socketfun.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

netrebin: netrebin.o socketfun.o binner.o voxelize.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

getfrom: getfrom.o socketfun.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

giveto: giveto.o socketfun.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

serve: serve.o socketfun.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

collector: collector.o socketfun.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

pix2mesh: pix2mesh.o cell.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)
	
gencell: gencell.o cell.o binnerio.o
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

bmeshrebin: bmeshrebin.o binner.o voxelize.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

bslicer: bslicer.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)
	
install: all
	mv $(EXES) $(TOP)/bin

clean:
	rm -f *.o $(EXES) *.a *~
