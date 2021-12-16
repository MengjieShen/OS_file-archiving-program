//program to do ls showing inode number of all files in current directory
# include <stdio.h>
# include <sys/types.h>
# include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "zip.h"
#define BUFFSIZE 1024


// 1. create file
// 2. directory -> file: append every file
// 3. directory -> dir: recursively, append all files in the lowest level, return a file to the upper level,

// int copyAndWrite(char fromFile[],char* toFile, int index)
// {
// 	int n, from , to;
// 	char buf[BUFFSIZE];
// 	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;

// 	//open the "from" source file
// 	if ((from = open(fromFile , O_RDONLY)) < 0)
// 	{
// 		perror("open1");
// 		printf("Vivi");
// 		exit(1);
// 	}

// 	//open the "to" destiantion file
// 	if ((to = open(toFile , O_WRONLY|O_CREAT|O_APPEND, fdmode)) < 0)
// 	{
// 		perror("open2");
// 		exit(1);
// 	}

// 	//read from the "from" file and write into the "to" file
// 	while((n=read(from, buf, sizeof(buf)))>0)
// 		write(to,buf,n);

// 	metaRecords[index].size = buf.st_size;
// 	metaRecords[index].offset = dataOffset;
// 	dataOffset+=buf.st_size;

// 	close(from);

// 	write(to, metaRecords, sizeof(struct meta)*20);

// 	close(to);
// 	return(1);

// }

void do_ls ( char dirname []) {
	struct meta metaRecords[20];
	DIR * dir_ptr ;
	struct dirent *direntp ;
	char const* mytest = "mytest";
	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;

	if ((dir_ptr = opendir (dirname)) == NULL)
		fprintf (stderr , " cannot open %s \n",dirname);
	else {
		int count=0, i=0;		
		while ((direntp = readdir (dir_ptr)) != NULL ) {
            //get the inode number and file name
			count++;
			if (count < 3) continue;
			printf ("inode %d of the entry %s \n", (int) direntp ->d_ino , direntp -> d_name );
			char source[1024] = "testDir/";
			strcat(source ,direntp -> d_name);

			struct meta* curr = (struct meta*)malloc(sizeof(meta));
			metaRecords[i] = *curr;
			strcpy(curr->name, direntp -> d_name);
			// printf("test:%s", curr->name);
			strcpy(curr->parent_folder, dirname);

			if (count !=3 ) metaRecords[i-1].next = &metaRecords[i];
			
            copyAndWrite(source, mytest, *curr);
        }
		
		closedir (dir_ptr);
	}

	int to;
	if ((to = open(mytest , O_WRONLY|O_CREAT|O_APPEND, fdmode)) < 0)
	{
		perror("open2");
		exit(1);
	}
	write(to, metaRecords, sizeof(struct meta)*20);
}

int main () {
	char dirName[] = "testDir";
	do_ls (dirName);
	
}