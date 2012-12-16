# Copyright 2012 Shan An <anshan.tju@gmail.com>
#
# Copying and distribution of this file, with or without
# modification, are permitted in any medium without royalty provided
# the copyright notice and this notice are preserved.  This file is
# offered as-is, without any warranty.
# 
# Modified Dec. 2012 by Shan An

CC = gcc
CXX = g++

# C source code
CSRC = image_io/io_jpeg.c image_io/io_png.c 
CPPSRC = SLIC.cpp SLICsegmentation.cpp

# all source code
SRC	= $(CSRC)
SRC	+= $(CPPSRC)

# C objects
COBJ = $(CSRC:.c=.o)
CPPOBJ = $(CPPSRC:.cpp=.o)
# all objects
OBJ	= $(COBJ) 
OBJ	+= $(CPPOBJ)

# binary target
BIN	= SLICsegmentation

default	: $(BIN)

# C optimization flags
COPT = -O3 -ftree-vectorize -funroll-loops #delete for gdb debugging 

# C compilation flags 
CFLAGS	= $(COPT) -Wall -Wextra -Werror \
	-Wno-write-strings -ansi 

# link flags
LDFLAGS	= -ljpeg -lpng -lstdc++

# use openMP with `make OMP=1`
ifdef OMP
CFLAGS	+= -fopenmp
LDFLAGS += -lgomp
else
CFLAGS	+= -Wno-unknown-pragmas
endif

# partial compilation of C source code
%.o: %.c %.cpp %.h
	$(CC) -c -o $@  $< $(CFLAGS) -I/opt/local/include/ -I/usr/local/include/   

# link all the opject code
$(BIN): $(OBJ) $(LIBDEPS)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) -L/opt/local/lib/ -L/usr/local/lib/ -ljpeg -lpng


# housekeeping
.PHONY	: clean distclean
clean	:
	$(RM) $(OBJ)
	$(RM) $(BIN)
distclean	: clean
	$(RM) $(BIN)

