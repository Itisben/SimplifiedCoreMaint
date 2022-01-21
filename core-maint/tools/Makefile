GM_TOP= ~/code/hong/Green-Marl-mod_base_1
GM_GRAPH= ${GM_TOP}/apps/output_cpp/gm_graph
CC= g++
NAME=convert
CFLAGS = -g -O3 -fopenmp -I${GM_GRAPH}/inc -I. 
LFLAGS = -L${GM_GRAPH}/lib -lgmgraph 
include ${GM_TOP}/setup.mk
include ${GM_TOP}/apps/output_cpp/common.mk

$(NAME) : convert.cc convert.h
	${CC} ${CFLAGS} $< ${LFLAGS} -o $@

clean: 
	\rm $(NAME)
