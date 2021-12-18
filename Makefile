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
	./adzip -c "test.ad" "tmp/testDir/"

run2: adzip
	./adzip -a "test.ad" "tmp/M"

run3: adzip
	./adzip -x "test.ad" "tmp/testDir/"

run4: adzip
	./adzip -m "test.ad" "testDir/"

run5: adzip
	./adzip -p "test.ad" "tmp/M"


clean: 
	rm -f *~ *.o test a.out core test.ad adzip main

# readwriteclose.o: readwriteclose.c zip.h
# 	$(CC) $(CFLAGS) -c readwriteclose.c

