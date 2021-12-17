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
	// write the header
	write_ptr = fopen("test.bin","wb");  // w for write, b for binary
    fwrite(&header, sizeof(struct header), 1, write_ptr);
	fclose(write_ptr);
}

void addMeta() {
	write_ptr = fopen("test.bin","r+b");  // w for write, b for binary
	fseek(write_ptr, 17, SEEK_SET);
	fwrite (&metaRecords, sizeof(struct meta)*20, 1, write_ptr);
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
			// get the stat information for current directory
			stat(source, &st);

			// if file
			if ((st.st_mode & S_IFMT) == S_IFREG){
				struct meta* curr = (struct meta*)malloc(sizeof(struct meta));
				strcpy(metaRecords[fileCnt].name, direntp -> d_name);
				strcpy(metaRecords[fileCnt].parent, dirname);
				copyAndWrite(source, "test.bin", fileCnt);
				fileCnt++;
			}
			// if dir
			else if ((st.st_mode & S_IFMT) == S_IFDIR) breakDir(source);

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
	// printf("update header meta offset check : %d\n", h.meta_offset);
	old_offset = h.meta_offset;
	h.meta_offset = offset;
	int size = offset - old_offset;
	// fseek (write_ptr, (-1)*(sizeof(struct header)), SEEK_CUR);
	fseek(write_ptr, 0, SEEK_SET);
	fwrite (&h, sizeof(struct header), 1, write_ptr);

	// Update current meta records
	// fseek (write_ptr, old_offset, SEEK_SET);
	// h.num_elts = 0;
	// int ele = h.num_elts;
	// h.num_elts = ele + numOfEle;
	// m = realloc(m, sizeof(struct meta*) * h.num_elts);
	// fread (m, sizeof(struct meta*), h.num_elts, write_ptr);
	// for (i = 0; i < h.num_elts; ++i)
	// {
	// 	m[i].offset += size;
	// }
	// fseek (write_ptr, old_offset, SEEK_SET);
	// fwrite (m, sizeof(struct meta*), h.num_elts, write_ptr);

    // clean up
	fclose(write_ptr);
}

void read_metadata(){
    struct meta m;
	struct meta metas[20];
	struct header h;
	write_ptr = fopen("test.bin","rb");  // w for write, b for binary
    // m = (struct meta*) malloc(sizeof(struct meta));
	// Update current header
	fread (&h, sizeof(struct header), 1, write_ptr);
	printf("meta offset from reading header: %d \n", h.meta_offset);
	fseek (write_ptr, 17, SEEK_SET);
	// // fread(&m, sizeof(struct meta), 1, write_ptr);
	fread(&metas, sizeof(struct meta)*20, 1, write_ptr);

	printf("meta file name : %s\n", metas[0].name);
	printf("meta file name : %d\n", metas[0].size);
	// printf("meta file test : %s\n", m.parent);
	// printf("meta file test : %d\n", m.size);
	// printf("meta file test : %d\n", m.offset);
	fclose(write_ptr);

}

int main () {
	addHeader();
	char dirName[] = "testDir/A";	
	breakDir (dirName);
	addMeta();
	read_metadata();
}