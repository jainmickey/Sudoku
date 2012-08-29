#
# Makefile
#
# For Sudoku Game
#


# compiler to use
CC = gcc

# flags to pass compiler
CFLAGS = -ggdb -std=c99 -Wall -Werror -Wformat=0

# name for executable
EXE = sudoku

# space-separated list of header files
HDRS = sudoku.h

# space-separated list of libraries, if any,
# each of which should be prefixed with -l
LIBS = -lncurses

# space-separated list of source files
SRCS = sudoku.c

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# default target
$(EXE): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# dependencies 
$(OBJS): $(HDRS) Makefile

# housekeeping
clean:
	rm -f core $(EXE) *.o
