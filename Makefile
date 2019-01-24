# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

CC = g++

CFLAGS = -std=c++11 -O3 -Wall -Wno-write-strings -static

INCLUDES = -IsymComp/bliss-0.72/ -IsymComp -IcliqueComp/ -IsymComp/nauty25r9/ -IcliqueComp/mcqd_para

LFLAGS = -LsymComp/bliss-0.72/

LIBS = -lbliss symComp/nauty25r9/nauty.a 

SRCS = gtc.cc gtcGraph.cc cliqueComp/cliqueComp.cc gtcSolver.cc symComp/DisjointForrest.cc symComp/symCompBliss.cc symComp/symCompNauty.cc

OBJS = $(SRCS:.c=.o)

MAIN = gtc

.PHONY: depend clean

all:    $(MAIN)
	@echo  gtc is ready to use.

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

