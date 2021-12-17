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


int breakDir ( char dirname []) {

	struct header header;
	float curr_offset = 0;
	struct stat st;
	// header length: sizeof(int)

	struct meta metaRecords[20];
	DIR * dir_ptr ;
	struct dirent *direntp ;
	char const* mytest = "mytest";
	FILE * fp;
	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;

    // Create header
	// meta_offset: meta开始的地方
    header.meta_offset = sizeof(header);                  // is curr_offset viable right now -- no it is not
	header.num_elts = 0;

	// write the header
    fp = fopen(mytest, "w+");
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);

	//update current pointer 
	curr_offset += sizeof(header);
	// data_offset = sizeof(header);

	if ((dir_ptr = opendir (dirname)) == NULL)
		fprintf (stderr , " cannot open %s \n",dirname);
	else {
		int count=0, i=0, fileCnt = 0;		
		while ((direntp = readdir (dir_ptr)) != NULL ) {
            //get the inode number and file name
			count++;
			if (count < 3) continue;
			if (count == 3) fileCnt = 1;
			printf ("inode %d of the entry %s \n", (int) direntp ->d_ino , direntp -> d_name );

			// name to be found
			char source[1024] = "testDir/";
			strcat(source ,direntp -> d_name);

			// new record
			struct meta* curr = (struct meta*)malloc(sizeof(struct meta));
			metaRecords[i] = *curr;
			strcpy(curr->name, direntp -> d_name);
			strcpy(curr->parent_folder, dirname);
			if (count !=3 ) metaRecords[i-1].next = &metaRecords[i];
			

			// get the stat information for current directory
			stat(mytest, &st);


			// if file or dir
			// if file
			if ((st.st_mode & S_IFMT) == S_IFREG){
				curr_offset += copyAndWrite(source, mytest, *curr);
				fileCnt++;
			}
    
			else if ((st.st_mode & S_IFMT) == S_IFDIR)
        	{
			// if dir -> breakDir : count+=fileCnt

        	}

		updateMeta(curr_offset, fileCnt);
		closedir (dir_ptr);
	}

	int to;
	if ((to = open(mytest , O_WRONLY|O_CREAT|O_APPEND, fdmode)) < 0)
	{
		perror("open2");
		exit(1);
	}
	write(to, metaRecords, sizeof(struct meta)*20);

	return count-2;
}

void updateHeader(int curr_offset, int numOfEle){
    struct header h;
    int i;
    int old_offset, old_next;
    struct meta * m;
    char path[300];
    FILE *fp = fopen("mytest", "r+");
    m = (struct meta*) malloc(sizeof(struct meta));

	// Update current header
	fread (&h, sizeof(struct header), 1, fp);
	old_offset = h.meta_offset;
	h.meta_offset = curr_offset;
	int size = curr_offset - old_offset;
	fseek (fp, (-1)*(sizeof(struct header)), SEEK_CUR);
	fwrite (&h, sizeof(struct header), 1, fp);

	// update header's metadata
	fseek (fp, old_offset, SEEK_SET);
	m = realloc(m, sizeof(struct meta*) * h.num_elts);
	fread (m, sizeof(struct meta*), h.num_elts, fp);
	for (i = 0; i < h.num_elts; ++i)
	{
		m[i].offset += size;
	}
	fseek (fp, old_offset, SEEK_SET);
	fwrite (m, sizeof(struct meta*), h.num_elts, fp);

    // clean up
    fclose(fp);
    free(m);
}

int main () {
	char dirName[] = "testDir";	
	breakDir (dirName);
	

		
}