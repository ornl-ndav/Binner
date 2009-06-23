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

OBJS = binner.o binnerio.o bmeshrebin.o largebmeshrebin.o bslicer.o cell.o clip.o clip3d.o clipvoxel.o collector.o compvol.o export2vtk.o export2vcb.o gencell.o pix2mesh.o geometry.o getfrom.o giveto.o netrebin.o rebin.o rot3d.o scale3d.o serve.o socketfun.o sorter.o tran3d.o volume.o voxelize.o bounds.o counts.o rebinapp.o gmeshrot.o gmeshrot2.o gmeshrot3.o gmeshb2t.o gmesht2b.o gmeshorderv.o

EXES = gmeshrebin gmeshrot gmeshrot2 gmeshrot3 gmeshb2t gmesht2b gmeshorderv bmeshrebin largebmeshrebin bounds bslicer clip3d clipvoxel compvol export2vtk export2vcb gencell pix2mesh rebin rot3d scale3d tran3d collector getfrom giveto serve netrebin sorter counts

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

gmeshrot: gmeshrot.o binnerio.o
	$(CC) -pg -o $@ $^ $(LIB_PATH) $(LIBS)

gmeshrot2: gmeshrot2.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

gmeshrot3: gmeshrot3.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

gmeshorderv: gmeshorderv.o cell.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

gmesht2b: gmesht2b.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

gmeshb2t: gmeshb2t.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)
	
tran3d: tran3d.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

scale3d: scale3d.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

clip3d: clip3d.o clip.o geometry.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

clipvoxel: clipvoxel.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)
	
compvol: compvol.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

rebin: rebin.o binner.o voxelize.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

bounds: bounds.o cell.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

counts: counts.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)
	
bmeshrebin: bmeshrebin.o rebinapp.o binner.o voxelize.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

gmeshrebin: gmeshrebin.o rebinapp.o binner.o voxelize.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

gmeshrebin2: gmeshrebin2.o rebinapp.o binner.o voxelize.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

largebmeshrebin: largebmeshrebin.o binner.o voxelize.o volume.o clip.o cell.o geometry.o binnerio.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)
	
export2vtk: export2vtk.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

export2vcb: export2vcb.o binnerio.o
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

bslicer: bslicer.o 
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)
	
install: all
	mv $(EXES) $(TOP)/bin

clean:
	rm -f *.o $(EXES) *.a *~
