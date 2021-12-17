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
	./adzip -c "test.bin" "testDir/A"

run3: adzip
	./adzip -x "test.bin" "/Users/shenmengjie/Documents/GitHub/OS_file-archiving-program/B/"

run4: adzip
	./adzip -m "test.bin" "testDir/A/B"

run5: adzip
	./adzip -p "test.bin" "testDir/A"


clean: 
	rm -f *~ *.o test a.out core main

# readwriteclose.o: readwriteclose.c zip.h
# 	$(CC) $(CFLAGS) -c readwriteclose.c

