#----------------------------------------------------
#                                    ahu, 24-JUN-1995
# Makefile for Pentomino
# uses default rules

# Modified:
# 07-AUG-1995, ahu: added tree
# 10-AUG-1995, ahu: added disp
# 14-JAN-1996, ahu: reorganised everything

#----------------------------------------------------
# C Compiler

# GNU C
CC = gcc
#CFLAGS += -g

# optimize
CFLAGS += -O3 

# get all kinds of warnings from GCC
CFLAGS += -Wall 
#CFLAGS += -Wstrict-prototypes
#CFLAGS += -Wmissing-prototypes
#CFLAGS += -Wcast-align
#CFLAGS += -Wconversion 
#CFLAGS += -Wpointer-arith

#----------------------------------------------------
# HP 9000
#CC = cc
#CFLAGS = -Aa +O3

#CFLAGS += -D_HPUX_SOURCE

#----------------------------------------------------
# DEC
#CC = cc
#CFLAGS = -O3

# produce call graph profile data
#CFLAGS += -pg
#LDFLAGS = -pg

#----------------------------------------------------
# pentomino specific flags

# for scalable architecture support define SCARCH
CFLAGS += -DSCARCH

# for a lot of debug output define DEBUG 
#CFLAGS += -DDEBUG

#----------------------------------------------------

RM = rm -f

#----------------------------------------------------

AOUT  = pentomino
OBJS  = print.o play.o data.o setup.o read.o scarch.o pentomino.o
HEADS = data.h pentomino.h

#----------------------------------------------------
# dependency lines and construction commands:

all: $(AOUT)

$(AOUT): $(OBJS)

pentomino.o: setup.h read.h print.h play.h $(HEADS)

data.o:      $(HEADS)

setup.o:     setup.h play.h $(HEADS)

read.o:      setup.h read.h print.h $(HEADS)

print.o:     setup.h print.h $(HEADS)

play.o:      setup.h play.h print.h $(HEADS)

scarch.o:    play.h scarch.h ${HEADS}

.PHONY : clean
clean:
	-$(RM) $(AOUT) *.o *~ .*~
