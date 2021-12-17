//program to do ls showing inode number of all files in current directory
# include <stdio.h>
# include <sys/types.h>
# include <dirent.h>
# include <string.h>
# include <stdio.h>
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

int breakDir ( char dirname []) {

	struct header header;
	float curr_offset = 0;
	struct stat st;

	struct meta metaRecords[20];
	DIR * dir_ptr ;
	struct dirent *direntp ;
	char const* mytest = "mytest";
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
    printf("num_of_eles:%d\n", header.num_elts);

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
			metaRecords[i] = *curr;
			strcpy(curr->name, direntp -> d_name);
			strcpy(curr->parent_folder, dirname);
			// if (count !=3 ) metaRecords[i-1].next = &metaRecords[i];

			// get the stat information for current directory
			stat(source, &st);

			// if file or dir
			// if file
			if ((st.st_mode & S_IFMT) == S_IFREG){
				curr_offset += copyAndWrite(source, "test.bin", *curr);
				fileCnt++;
			}
    
			else if ((st.st_mode & S_IFMT) == S_IFDIR)
        	{
			// if dir -> breakDir : count+=fileCnt
				fileCnt += breakDir(source);
				printf("File count counting dir A: %d\n", fileCnt);
        	}
				printf("\n\nfile count test: %d\n\n", fileCnt);
				

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
    // FILE *fp = fopen("mytest", "w+");
	write_ptr = fopen("test.bin","w+b");  // w for write, b for binary
    m = (struct meta*) malloc(sizeof(struct meta));

	// Update current header
	// fread (&h, sizeof(struct header), 1, fp);
	fread (&h, sizeof(struct header), 1, write_ptr);
	// printf("num_of_eles: %d\n", h.num_elts);
	old_offset = h.meta_offset;
	h.meta_offset = curr_offset;
	int size = curr_offset - old_offset;
	// fseek (fp, (-1)*(sizeof(struct header)), SEEK_CUR);
	fseek (write_ptr, (-1)*(sizeof(struct header)), SEEK_CUR);
	// fwrite (&h, sizeof(struct header), 1, fp);
	fwrite (&h, sizeof(struct header), 1, write_ptr);
	printf("meta_offset:%d\n", h.meta_offset);
	printf("old_offset:%d\n", old_offset);

	// update header's metadata
	// fseek (fp, old_offset, SEEK_SET);
	fseek (write_ptr, old_offset, SEEK_SET);
	// printf("\n\n header num elements:%d", h.num_elts);
	h.num_elts = 0;
	int ele = h.num_elts;
	// printf("\n\n old num elements:%d", ele);
	h.num_elts = ele + numOfEle;
	// printf("\n\n new num elements:%d", h.num_elts);
	
	m = realloc(m, sizeof(struct meta*) * h.num_elts);
	// fread (m, sizeof(struct meta*), h.num_elts, fp);
	fread (m, sizeof(struct meta*), h.num_elts, write_ptr);
	for (i = 0; i < h.num_elts; ++i)
	{
		m[i].offset += size;
	}
	// fseek (fp, old_offset, SEEK_SET);
	fseek (write_ptr, old_offset, SEEK_SET);
	// fwrite (m, sizeof(struct meta*), h.num_elts, fp);
	fwrite (m, sizeof(struct meta*), h.num_elts, write_ptr);

	char buffer[2048];


		/* File was opened successfully. */
		
		/* Attempt to read */
	while (fread(buffer, sizeof *buffer, 4, write_ptr) == 4) {
		/* byte swap here */
		printf("%s\n", buffer);
	}


    // clean up
	fclose(write_ptr);
    // fclose(fp);
    free(m);
}

int main () {
	char dirName[] = "testDir";	
	breakDir (dirName);
	

		
}