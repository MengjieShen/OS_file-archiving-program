# Makefile for executable adjust

# *****************************************************
# Parameters to control Makefile operation
CC = gcc
CFLAGS = -pedantic -Wall -g
LIBS = -lm

# ****************************************************
# Entries to bring the executable up to date

adzip: adzip.o help.o 
	$(CC) $(CFLAGS) -o adzip adzip.o help.o 
	
adzip.o: adzip.c zip.h
	$(CC) $(CFLAGS) -c adzip.c

help.o: help.c zip.h
	$(CC) $(CFLAGS) -c help.c
	
run1: adzip
	./adzip -c "test.bin" "tmp/testDir/"

run2: adzip
	./adzip -a "test.bin" "tmp/testDir/k.txt"

run3: adzip
	./adzip -x "test.bin" "tmp/testDir/A/dirB/g.txt"

run4: adzip
	./adzip -m "test.bin" "testDir/"

run5: adzip
	./adzip -p "test.bin" "tmp/testDir/"


clean: 
	rm -f *~ *.o test a.out core test.bin adzip main

# readwriteclose.o: readwriteclose.c zip.h
# 	$(CC) $(CFLAGS) -c readwriteclose.c

