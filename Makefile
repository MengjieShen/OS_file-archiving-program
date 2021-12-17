# Makefile for executable adjust

# *****************************************************
# Parameters to control Makefile operation
CC = gcc
CFLAGS = -pedantic -Wall -g
LIBS = -lm

# ****************************************************
# Entries to bring the executable up to date

test: test.o help.o 
	$(CC) $(CFLAGS) -o test test.o help.o 
	
test.o: test.c zip.h
	$(CC) $(CFLAGS) -c test.c

help.o: help.c zip.h
	$(CC) $(CFLAGS) -c help.c

clean: 
	rm -f *~ *.o test a.out core main
# readwriteclose.o: readwriteclose.c zip.h
# 	$(CC) $(CFLAGS) -c readwriteclose.c

