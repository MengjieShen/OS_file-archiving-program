//program that appends the content of a file at the very end of the 
//content of another file

//usage : ./output from-file to-file

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "zip.h"
#define BUFFSIZE 1024
int dataOffset = 0;
// 1. create file
// 2. directory -> file: append every file
// 3. directory -> dir: recursively, append all files in the lowest level, return a file to the upper level,

int copyAndWrite(char fromFile[],char* toFile, struct meta record)
{
	int n, from , to;
	char buf[BUFFSIZE];
	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	struct stat statbuf ;

	//open the "from" source file
	if ((from = open(fromFile , O_RDONLY)) < 0)
	{
		perror("open1");
		// printf("Vivi");
		exit(1);
	}

	//open the "to" destiantion file
	if ((to = open(toFile , O_WRONLY|O_CREAT|O_APPEND, fdmode)) < 0)
	{
		perror("open2");
		exit(1);
	}

	//read from the "from" file and write into the "to" file
	while((n=read(from, buf, sizeof(buf)))>0) 
		write(to,buf,n);
	
	if (stat (fromFile , &statbuf ) == -1) perror("stat");
	else {
		record.size = statbuf.st_size;
		record.offset = dataOffset;
		dataOffset+=statbuf.st_size;
		header.meta_offset += statbuf.st_size;
		header.num_elts += 1;
		// printf("name:%s\n", record.name);
		// printf("parent_folder:%s\n", record.parent_folder);
		// printf("size:%d\n", record.size);
		// printf("offset:%d\n", record.offset);
	}

	close(from);
	close(to);
	return(1);

}