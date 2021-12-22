#CC=g++ -march=native -O2 -D DEBUG2
#CC=g++ -march=native -g -D DEBUG
#CC=g++ -march=native -g -D DEBUG2
#CC=g++ -march=native -O2
#CC=g++ -march=native
CC=g++

INCDIR=./gm_graph/inc
LIBDIR=./gm_graph/lib
LIB=gmgraph
LFLAGS=-L$(LIBDIR) -l$(LIB)

#CFLAGS=-c -I. -std=c++0x -Wfatal-errors
#CFLAGS=-c -I. -I$(INCDIR) -std=c++0x -Wfatal-errors
CFLAGS=-c -I. -I$(INCDIR) -fopenmp  -std=gnu++0x -fpermissive
CFLAGS2=-I. -I$(INCDIR) -fopenmp  -std=gnu++0x -fpermissive
INCFILES = $(shell ls $(INCDIR))
H_FILES = $(addprefix $(INCDIR)/, $(INCFILES))

OUTDIR=./

all: debug

debug: CC+= -g -D DEBUG
core: CC+= -O3
core2: CC+= -O3 -D WITH_SUBTAG
dcore: CC+= -g
prof: CC+= -pg -Wall -O2 -g



core: maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o ./gm_graph/lib/libgmgraph.a
	$(CC) $(CFLAGS2) maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o $(LFLAGS) -o core
	
core2: maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o ./gm_graph/lib/libgmgraph.a
	$(CC) $(CFLAGS2) maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o $(LFLAGS) -o core2

dcore: maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o ./gm_graph/lib/libgmgraph.a
	$(CC) $(CFLAGS2) maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o $(LFLAGS) -o dcore

debug: maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o ./gm_graph/lib/libgmgraph.a
	$(CC) $(CFLAGS2) maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o $(LFLAGS) -o debug

prof: maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o ./gm_graph/lib/libgmgraph.a
	$(CC) $(CFLAGS2) maindebug.o gadget.o heap.o treap.o traversal.o glist.o ours.o $(LFLAGS) -o prof


maindebug.o: maindebug.cc
	$(CC) $(CFLAGS) maindebug.cc -o maindebug.o


gadget.o: gadget/gadget.cc
	$(CC) $(CFLAGS) gadget/gadget.cc -o gadget.o

heap.o: gadget/heap.cc
	$(CC) $(CFLAGS) gadget/heap.cc -o heap.o

treap.o: gadget/treap.cc
	$(CC) $(CFLAGS) gadget/treap.cc -o treap.o

traversal.o: traversal/traversal.cc
	$(CC) $(CFLAGS) traversal/traversal.cc -o traversal.o

glist.o: glist/glist.cc
	$(CC) $(CFLAGS) glist/glist.cc -o glist.o

ours.o: ours8/core-maint.cpp
	$(CC) $(CFLAGS) ours8/core-maint.cpp -o ours.o

clean: 
	rm -f ./*.o

