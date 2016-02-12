# Use the GNU C/C++ compiler: 
CC = gcc
CPP = g++

# COMPILER OPTIONS:

CFLAGS = -c

#OBJECT FILES 
OBJS = dsh.o 


dsh: dsh.o 
		${CC} -lm ${OBJS} -o dsh

clean:
	rm *o 
	rm dsh