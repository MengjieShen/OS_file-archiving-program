//program to do ls showing inode number of all files in current directory
# include <stdio.h>
# include <sys/types.h>
# include <dirent.h>
# include <string.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include "zip.h"
# define BUFFSIZE 1024

// 1. create file
// 2. directory -> file: append every file
// 3. directory -> dir: recursively, append all files in the lowest level, return a file to the upper level,
FILE * write_ptr;
struct meta metaRecords[20];
int dataOffset = sizeof(header);
int fileCnt = 0;

int copyAndWrite(char fromFile[],char* toFile, int index)
{	
	int n, from , to;
	char buf[BUFFSIZE];
	memset(buf, 0, BUFFSIZE);
	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	struct stat statbuf ;

	//open the "from" source file
	if ((from = open(fromFile , O_RDONLY)) < 0)
	{
		perror("open1");
		exit(1);
	}

	//open the "to" destiantion file
	if ((to = open(toFile , O_WRONLY|O_CREAT|O_APPEND, fdmode)) < 0)
	{
		perror("open2");
		exit(1);
	}

	//read from the "from" file and write into the "to" file
	while((n=read(from, buf, sizeof(buf)))>0) {
		write(to,buf,n);
	}
		
	
	if (stat (fromFile , &statbuf ) == -1) perror("stat");
	else {
		metaRecords[index].size = statbuf.st_size;
		metaRecords[index].offset = dataOffset;
		dataOffset+=statbuf.st_size;
		header.meta_offset += statbuf.st_size;
		header.num_elts += 1;
	}
	
	close(from);
	close(to);
	return statbuf.st_size;

}

void addHeader() {
	struct header header;
	header.meta_offset = sizeof(struct header);                  // is curr_offset viable right now -- no it is not
	header.num_elts = 0;
	// printf("header meta offset : %d", header.meta_offset); //12
	// write the header
	write_ptr = fopen("test.bin","wb");  // w for write, b for binary
    fwrite(&header, sizeof(struct header), 1, write_ptr);
	fclose(write_ptr);
}

void breakDir ( char dirname []) {

	float curr_offset = 0;
	struct stat st;
	DIR * dir_ptr ;
	struct dirent *direntp ;
	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	int count=0, i=0;		
    // Create header
	// meta_offset: meta开始的地方

	if ((dir_ptr = opendir (dirname)) == NULL)
		fprintf (stderr , " cannot open %s \n",dirname);
	else {
		while ((direntp = readdir (dir_ptr)) != NULL ) {
            //get the inode number and file name
			count++;
			if (count < 3) continue;
			printf ("inode %d of the entry %s \n", (int) direntp ->d_ino , direntp -> d_name );

			// name to be found
			char source[1024];
			strcpy(source, dirname);
			strcat(source, "/");
			strcat(source, direntp -> d_name);

			// new record
			// printf("parent test %s\n", metaRecords[fileCnt].parent);

			// get the stat information for current directory
			stat(source, &st);

			// if file
			if ((st.st_mode & S_IFMT) == S_IFREG){
				struct meta* curr = (struct meta*)malloc(sizeof(struct meta));
				strcpy(metaRecords[fileCnt].name, direntp -> d_name);
				strcpy(metaRecords[fileCnt].parent, dirname);
				copyAndWrite(source, "test.bin", fileCnt);
				// printf("current file offset after adding f.txt: %f\n", curr_offset);
				printf("\n\n\nmetacheck\n\n\n");
				printf("meta index: %d\n", fileCnt);
				printf("meta name: %s\n", metaRecords[fileCnt].name);
				printf("meta parent: %s\n", metaRecords[fileCnt].parent);
				printf("meta size: %d\n", metaRecords[fileCnt].size);
				printf("meta offset: %d\n", metaRecords[fileCnt].offset);
				fileCnt++;
			}
			// if dir
			else if ((st.st_mode & S_IFMT) == S_IFDIR)
        	{
				breakDir(source);
        	}				

		}
		
		updateHeader(dataOffset, fileCnt);
		closedir (dir_ptr);
	}

}

void updateHeader(int offset, int numOfEle) {
    int i;
    int old_offset, old_next;
    struct meta * m;
	struct header h;
    char path[300];
	write_ptr = fopen("test.bin","r+b");  // w for write, b for binary
    m = (struct meta*) malloc(sizeof(struct meta));

	// Update current header
	fread (&h, sizeof(struct header), 1, write_ptr);
	old_offset = h.meta_offset;
	h.meta_offset = offset;
	int size = offset - old_offset;
	fseek (write_ptr, (-1)*(sizeof(struct header)), SEEK_CUR);
	fwrite (&h, sizeof(struct header), 1, write_ptr);
	// Update current meta records
	fseek (write_ptr, old_offset, SEEK_SET);
	h.num_elts = 0;
	int ele = h.num_elts;
	h.num_elts = ele + numOfEle;
	m = realloc(m, sizeof(struct meta*) * h.num_elts);
	fread (m, sizeof(struct meta*), h.num_elts, write_ptr);
	for (i = 0; i < h.num_elts; ++i)
	{
		m[i].offset += size;
	}
	fseek (write_ptr, old_offset, SEEK_SET);
	fwrite (m, sizeof(struct meta*), h.num_elts, write_ptr);

	// printf("Current header.meta_offset: %d\n", h.meta_offset);
	// printf("Current header.num_elts: %d\n", h.num_elts);
	fclose(write_ptr);

	write_ptr = fopen("test.bin","rb");
	// read from the beginning
	fseek(write_ptr, 0, SEEK_SET);
	char buffer[2048];
	int j;
	// for (j=0; j<20; j++) {
	// 	printf("j: %d\n", j);
	// 	printf("name:%s\n", metaRecords[j].name);
	// 	printf("parent_folder:%s\n", metaRecords[j].parent_folder);
	// 	printf("size:%d\n", metaRecords[j].size);
	// 	printf("offset:%d\n", metaRecords[j].offset);
	// }
	// while (fread(buffer, sizeof *buffer, 1, write_ptr) != 0) {
	// 	printf("%s\n", buffer);
	// }

    // clean up
	fclose(write_ptr);
    free(m);
}

void read_metadata(){
    struct meta * m;
	struct header h;
	write_ptr = fopen("test.bin","rb");  // w for write, b for binary
    m = (struct meta*) malloc(sizeof(struct meta));
	// Update current header
	fread (&h, sizeof(struct header), 1, write_ptr);
	int meta_offset = h.meta_offset;
	printf("meta offset from reading header: %d \n", h.meta_offset);
	fseek (write_ptr, meta_offset, SEEK_SET);
	fread(&meta, sizeof(struct meta), 1, write_ptr);
	// printf("size of meta: %lu", sizeof(meta));
	printf("meta file test : %s\n", meta.name);
	printf("meta file test : %s\n", meta.parent);
	printf("meta file test : %d\n", meta.size);
	printf("meta file test : %d\n", meta.offset);
	fclose(write_ptr);
	free(m);
}

int main () {
	addHeader();
	char dirName[] = "testDir/A";	
	breakDir (dirName);
	read_metadata();
}