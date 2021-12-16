# Makefile for executable adjust

# *****************************************************
# Parameters to control Makefile operation
CC = gcc
CFLAGS = -pedantic -Wall -g
LIBS = -lm

# ****************************************************
# Entries to bring the executable up to date

test: test.o readwriteclose.o 
	$(CC) $(CFLAGS) -o test test.o readwriteclose.o 
	
test.o: test.c zip.h
	$(CC) $(CFLAGS) -c test.c

readwriteclose.o: readwriteclose.c zip.h
	$(CC) $(CFLAGS) -c readwriteclose.c

