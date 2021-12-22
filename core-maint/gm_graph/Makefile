CC= g++
AR= ar
LIB = lib
BIN = ../bin
INC = inc
SRC = src
OBJ = obj
CFLAGS = -O3 -g -I$(INC) -fopenmp -Wall 
#CFLAGS = -g -I$(INC) -fopenmp -Wall 
OUT =$(LIB)/libgmgraph.a 

# where to find source codes
vpath %.cc $(SRC)
vpath %.h $(INC)

SOURCES := $(wildcard $(SRC)/*.cc)
OBJECTS := $(subst $(SRC),$(OBJ),$(SOURCES:.cc=.o)) 

# include common define flags for gm_graph and application
# (32/64bit environment), (bitwidth of node/edge structure), (hdfs setting)
include common.mk

all: $(BUILD_DIRS) $(OUT)

$(LIB)/libgmgraph.a: $(OBJECTS)
	$(AR) r $@  $^  

$(BUILD_DIRS): 
	mkdir $(JBIN)

$(OBJ)/%.o: %.cc .
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)/*.o $(OUT) $(JAVACLASSES)

.PHONY: echocflags echolflags echoflags
echocflags:
	@echo "$(CFLAGS)"
echolflags:
	@echo "$(LFLAGS)"
echoflags:
	@echo "$(CFLAGS) $(LFLAGS)"
