//program to do ls showing inode number of all files in current directory
# include <stdio.h>
# include <sys/types.h>
# include <dirent.h>
# include <string.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <stdbool.h>
# include "zip.h"
# define BUFFSIZE 1024

// 1. create file
// 2. directory -> file: append every file
// 3. directory -> dir: recursively, append all files in the lowest level, return a file to the upper level,

FILE * write_ptr;
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
		// printf("n: %d", n);
		write(to,buf,n);
	}
		
	
	if (stat (fromFile , &statbuf ) == -1) perror("stat");
	else {
		metaRecords[index].size = statbuf.st_size;
		metaRecords[index].offset = dataOffset;
		metaRecords[index].permissions = statbuf.st_mode;
		dataOffset += statbuf.st_size;
		// header.meta_offset += statbuf.st_size;
		// header.num_elts += 1;
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
				printf("filecnt: %d", fileCnt);
				// struct meta* curr = (struct meta*)malloc(sizeof(struct meta));
				strcpy(metaRecords[fileCnt].name, direntp -> d_name);
				strcpy(metaRecords[fileCnt].parent, trimmer(dirname));
				metaRecords[fileCnt].isFile = true;
				copyAndWrite(source, "test.bin", fileCnt);
				fileCnt++;
				
			}
			// if dir
			else if ((st.st_mode & S_IFMT) == S_IFDIR){
				printf("filecnt: %d", fileCnt);
				// struct meta* curr = (struct meta*)malloc(sizeof(struct meta));
				metaRecords[fileCnt].isFile = false;
				strcpy(metaRecords[fileCnt].name, direntp -> d_name);
				strcpy(metaRecords[fileCnt].parent, trimmer(dirname));
				metaRecords[fileCnt].permissions =  st.st_mode;
				// header.num_elts += 1;
				fileCnt++;
				breakDir(source);
				// updateHeader(dataOffset, fileCnt);
				// return;
			} 
		}
		// updateHeader(dataOffset, fileCnt);
		
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
	h.num_elts += numOfEle;
	// printf("update header meta offset check : %d\n", h.meta_offset);
	old_offset = h.meta_offset;
	h.meta_offset = offset;
	// printf("update header meta offset check : %d\n", h.meta_offset);
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
	free(m);
	fclose(write_ptr);
}

void read_metadata(){
	struct meta metas[20];
	struct header h;
	write_ptr = fopen("test.bin","rb");  // w for write, b for binary
	// Update current header
	fread (&h, sizeof(struct header), 1, write_ptr);
	printf("meta offset from reading header: %d \n", h.meta_offset);
	fseek (write_ptr, h.meta_offset, SEEK_SET);
	// // fread(&m, sizeof(struct meta), 1, write_ptr);
	fread(&metas, sizeof(struct meta)*20, 1, write_ptr);
	printf("meta file name : %s\n", metas[0].name);
	printf("meta isfile  : %d\n", metas[0].isFile);
	printf("meta file size : %d\n", metas[2].size);
	printf("meta file offset : %d\n", metas[2].offset);
	printf("meta file parent : %s\n", metas[2].parent);
	fclose(write_ptr);
	// return metas;
}

void extractfile(char path[1024], int offset, int size, int permissions){
	int n;
	write_ptr = fopen("test.bin","r+b");
	FILE * extract_ptr;
	printf("path %s", path);
	extract_ptr = fopen(path, "w");
	char* buf;
// 	printf("size %d", size);
	fseek(write_ptr, offset, SEEK_SET);
// // 	//read from the "from" file and write into the "to" file
	fread (&buf, size, 1, write_ptr);
	// printf("buffer %s", &buf);
	fwrite (&buf, size, 1, extract_ptr);
	// chmod(path, permissions);
	fclose(write_ptr);
	fclose(extract_ptr);
}


//dircectory chmod 还没加
void extract(char root[1024], char path[1024]){

	char tmppath[1024];
	strcpy(tmppath, path);
	// strcat(tmppath, "/");
	strcat(tmppath, root);
	strcat(tmppath, "/");
	// printf("tmppath %s", tmppath);
	path = tmppath;
	if(mkdir(path, 0777) == -1)
	{
		fprintf(stderr, "Error making dir: %s\n", path);
		// free(m);
		perror("Error: ");
		exit(1);
	}
	sleep(1);

	struct meta metas[20];
	struct header h;
	write_ptr = fopen("test.bin","rb");  // w for write, b for binary
	fread (&h, sizeof(struct header), 1, write_ptr);
	fseek (write_ptr, h.meta_offset, SEEK_SET);
	fread(&metas, sizeof(struct meta)*20, 1, write_ptr);
	fclose(write_ptr);
	printf("num elements: %s", metas[0].parent);
	for(int i = 0; i<h.num_elts; i++){
		printf("\nparent%s\n",metas[i].parent);
		if(strcmp(metas[i].parent,root)== 0){
		// 	//file
			if(metas[i].isFile){
				// continue;
				printf("\n\n %s \n\n", metas[i].name);
				// path = "/Users/shenmengjie/Documents/GitHub/OS_file-archiving-program/testDir1";
				char tmppath[1024];
				strcpy(tmppath, path);
				// strcat(tmppath, "/");
				strcat(tmppath, metas[i].name);
				printf("tmppath file: %s\n", tmppath);
				extractfile(tmppath, metas[i].offset, metas[i].size, metas[i].permissions);
			}
		 	//directory
			else{
				// printf("\n\n %s \n\n", metas[i].name);
				// char tmppath[1024];
				// strcpy(tmppath, path);
				// // strcat(tmppath, "/");
				// strcat(tmppath, metas[i].name);
				// strcat(tmppath, "/");
				// printf("tmppath %s", tmppath);
				// if(mkdir(path, metas[i].permissions) == -1)
				// {
				// 	fprintf(stderr, "Error making dir: %s\n", path);
				// 	// free(m);
				// 	perror("Error: ");
				// 	exit(1);
				// }
				extract(metas[i].name, tmppath);
			}
		}
	}
}


int main () {
	addHeader();
	char dirName[] = "testDir/A";	
	breakDir (dirName);
	updateHeader(dataOffset, fileCnt);
	addMeta();
	// read_metadata();
	extract("B", "/Users/shenmengjie/Documents/GitHub/OS_file-archiving-program/");
}