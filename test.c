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
int dataOffset = 0;

int copyAndWrite(char fromFile[],char* toFile, int index)
{	
	int n, from , to;
	char buf[BUFFSIZE];
	memset(buf, 0, BUFFSIZE);
	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	struct stat statbuf ;

	// printf("1. name:%s\n", metaRecords[index].name);
	// printf("1. parent_folder:%s\n", metaRecords[index].parent_folder);
	// printf("1. size:%d\n", metaRecords[index].size);
	// printf("1. offset:%d\n", metaRecords[index].offset);
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

int breakDir ( char dirname []) {

	struct header header;
	float curr_offset = 0;
	struct stat st;

	// struct meta metaRecords[20];
	DIR * dir_ptr ;
	struct dirent *direntp ;
	FILE * fp;
	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	int count=0, i=0, fileCnt = 0;		
    // Create header
	// meta_offset: meta开始的地方
    header.meta_offset = sizeof(header);                  // is curr_offset viable right now -- no it is not
	header.num_elts = 0;

	// write the header
	write_ptr = fopen("test.bin","wb");  // w for write, b for binary
    fwrite(&header, sizeof(struct header), 1, write_ptr);
	fclose(write_ptr);
	//update current pointer 
	curr_offset += sizeof(header);

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
			struct meta* curr = (struct meta*)malloc(sizeof(struct meta));
			strcpy(metaRecords[fileCnt].name, direntp -> d_name);
			strcpy(metaRecords[fileCnt].parent_folder, dirname);

			// get the stat information for current directory
			stat(source, &st);

			// if file or dir
			// if file
			if ((st.st_mode & S_IFMT) == S_IFREG){
				curr_offset += copyAndWrite(source, "test.bin", fileCnt);
				fileCnt++;
			}
    
			else if ((st.st_mode & S_IFMT) == S_IFDIR)
        	{
			// if dir -> breakDir : count+=fileCnt
				fileCnt += breakDir(source);
        	}				

		}
		updateHeader(curr_offset, fileCnt);
		closedir (dir_ptr);
	}


	return fileCnt;
}

void updateHeader(int curr_offset, int numOfEle) {
    
    int i;
    int old_offset, old_next;
    struct meta * m;
	struct header h;
    char path[300];
	write_ptr = fopen("test.bin","a+b");  // w for write, b for binary
    m = (struct meta*) malloc(sizeof(struct meta));

	// Update current header
	fread (&h, sizeof(struct header), 1, write_ptr);
	old_offset = h.meta_offset;
	h.meta_offset = curr_offset;
	int size = curr_offset - old_offset;
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

	printf("Current header.meta_offset: %d\n", h.meta_offset);
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
    // FILE *fp = fopen("mytest", "w+");
	write_ptr = fopen("test.bin","rb");  // w for write, b for binary
    m = (struct meta*) malloc(sizeof(struct meta));
	// Update current header
	// fread (&h, sizeof(struct header), 1, fp);
	fread (&h, sizeof(struct header), 1, write_ptr);
	int meta_offset = h.meta_offset;
	printf("meta_offset %d \n", h.meta_offset);
	fseek (write_ptr, meta_offset, SEEK_SET);
	fread(&meta, sizeof(struct meta), 1, write_ptr);
	printf("size of meta: %lu", sizeof(meta));
	printf("meta file test 12 : %s\n", meta.name);

	fclose(write_ptr);
	free(m);
}

int main () {
	char dirName[] = "testDir";	
	breakDir (dirName);
	read_metadata();
}