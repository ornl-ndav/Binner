SHELL = /bin/sh
                                                                                          
C++ = g++
CC  = gcc

ifeq ($(BUILD),debug)
CCFLAGS := -g
else
CCFLAGS := -O3
endif
INCLUDE = -I. -I../include -I/usr/include/GL

LIB_PATH = -L../lib -L/usr/lib

LIBS = -lvcb -lm

EXECUTABLES = vcbcomp vcbrcast vcbhead vcbrange vcbcut

.SUFFIXES: .cc .c .cpp

.cc.o:
	$(C++) $(CCFLAGS) $(INCLUDE) -c $<

.cpp.o:
	$(C++) $(CCFLAGS) $(INCLUDE) -c $<

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDE) -c $<

all: $(EXECUTABLES)

vcbcomp: vcbcomp.o
	$(CC) -o $@ $(CFLAGS) vcbcomp.o $(LIB_PATH) $(LIBS)

vcbrcast: vcbrcast.o
	$(CC) -o $@ $(CCFLAGS) vcbrcast.o $(LIB_PATH) $(LIBS)

vcbhead: vcbhead.o
	$(CC) -o $@ $(CCFLAGS) vcbhead.o $(LIB_PATH) $(LIBS)

vcbrange: vcbrange.o
	$(CC) -o $@ $(CCFLAGS) vcbrange.o $(LIB_PATH) $(LIBS)

vcbcut: vcbcut.o
	$(CC) -o $@ $(CCFLAGS) vcbcut.o $(LIB_PATH) $(LIBS)

clean:
	rm -f $(EXECUTABLES) *.o core *~

install: all
	mv -f $(EXECUTABLES) ../bin/
