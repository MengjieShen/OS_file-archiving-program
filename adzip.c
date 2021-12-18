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

FILE * write_ptr;
// int dataOffset = sizeof(header);
// int fileCnt = 0;

tmp* copyAndWrite(char* fromFile,char* toFile, tmp* tmp, meta metaRecords[20])
{	
	int n, from , to, index, dataOffset;
	char buf[BUFFSIZE];
	memset(buf, 0, BUFFSIZE);
	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	struct stat statbuf;
	index = tmp->index;
	// printf("index: %d", index);
	dataOffset = tmp->dataOffset;
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
		dataOffset += statbuf.st_size;
		metaRecords[index].offset = dataOffset;
		// metaRecords[index].offset = metaRecords[index-1].offset + statbuf.st_size;
		metaRecords[index].permissions = statbuf.st_mode;
	}
	close(from);
	close(to);
	tmp->dataOffset =  dataOffset;
	// printf("tmp -> index %d\n" , tmp->index);
	tmp->index ++;
	// printf("tmp -> index %d\n" , tmp->index);
	return tmp;
}

void addHeader(char* archive_file) {
	struct header header;
	header.meta_offset = sizeof(struct header);                   
	header.num_elts = 0;
	// write the header
	write_ptr = fopen(archive_file,"wb");  // w for write, b for binary
    fwrite(&header, sizeof(struct header), 1, write_ptr);
	fclose(write_ptr);
}

void addMeta(char* archive_file, meta metaRecords[20]) {
	struct header h;
	write_ptr = fopen(archive_file,"r+b");  // w for write, b for binary
	fread (&h, sizeof(struct header), 1, write_ptr);
	fseek(write_ptr, h.meta_offset, SEEK_SET);
	// printf("meta offset %d", h.meta_offset);
	fwrite (metaRecords, sizeof(struct meta), 20, write_ptr);

	fclose(write_ptr);
}

void updateHeader(int offset, int numOfEle, char* archive_file) {
    int old_offset;
	struct header h;
	write_ptr = fopen(archive_file,"r+b");  // w for write, b for binary

	// Update current header
	fread (&h, sizeof(struct header), 1, write_ptr);
	h.num_elts = numOfEle;
	old_offset = h.meta_offset;
	h.meta_offset = offset;
	int size = offset - old_offset;
	fseek(write_ptr, 0, SEEK_SET);
	fwrite (&h, sizeof(struct header), 1, write_ptr);
	fclose(write_ptr);
}

void read_metadata(char* archive_file){
	// printf("archive file %s", archive_file);
	struct meta metas[20];
	struct header h;
	write_ptr = fopen(archive_file,"rb");  // w for write, b for binary
	// Update current header
	fread (&h, sizeof(struct header), 1, write_ptr);
	// printf("meta offset from reading header: %d \n", h.meta_offset);
	fseek (write_ptr, h.meta_offset, SEEK_SET);
	// // fread(&m, sizeof(struct meta), 1, write_ptr);
	fread(&metas, sizeof(struct meta)*20, 1, write_ptr);
	printf("=======================================================\n");
    printf("=======================================================\n");
	for (int i = 0; i< h.num_elts; i++){
		if(metas[i].isFile){
			printf("\nFile: ");
			printf("file name : %s ", metas[i].name);
			printf("size : %d ", metas[i].size);
			printf("offset : %d ", metas[i].offset);
			printf("parent : %s ", metas[i].parent);
			printf("permission: %d ", metas[i].permissions);
		}else{
			printf("\nDirectory: ");
			printf("Directory name : %s ", metas[i].name);
			printf("parent : %s ", metas[i].parent);
			printf("permission: %d ", metas[i].permissions);
		}
		printf("\n=======================================================\n");
	}
	fclose(write_ptr);
	// return metas;
}

void extractfile(char path[1024], char* archive_file, int offset, int size, int permissions){
	write_ptr = fopen(archive_file,"r+b");
	FILE * extract_ptr;
	printf("path %s", path);
	extract_ptr = fopen(path, "w");
	char* buf;
	fseek(write_ptr, offset, SEEK_SET);
	//read from the "from" file and write into the "to" file
	fread (&buf, size, 1, write_ptr);
	fwrite (&buf, size, 1, extract_ptr);
	chmod(path, permissions);
	fclose(write_ptr);
	fclose(extract_ptr);
}

//path is current path
void extract(char root[1024], char path[1024], char* archive_file, int permissions){
	if (strcmp(root, "..") != 0){
		if(mkdir(path, 0777) == -1)
		{
			fprintf(stderr, "Error making dir: %s\n", path);
			// free(m);
			perror("Error: ");
			exit(1);
		}
		printf("path here!%s", path);
		// sleep(100);
		chmod(path, permissions);
		printf("permissions %d", permissions);
		sleep(1);

	}
	struct meta metas[20];
	struct header h;
	write_ptr = fopen(archive_file,"rb");  
	fread (&h, sizeof(struct header), 1, write_ptr);
	fseek (write_ptr, h.meta_offset, SEEK_SET);
	fread(&metas, sizeof(struct meta)*20, 1, write_ptr);
	fclose(write_ptr);
	for(int i = 0; i<h.num_elts; i++){
		if(strcmp(metas[i].parent,root)== 0){
			char tmppath[1024];
			strcpy(tmppath, path);
			strcat(tmppath, metas[i].name);
				//file
				if(metas[i].isFile){
					printf("tmppath file: %s\n", tmppath);
					extractfile(tmppath, archive_file, metas[i].offset, metas[i].size, metas[i].permissions);
				}
				//directory
				else{
					printf("test test test %s\n", tmppath);
					strcat(tmppath, "/");
					extract(metas[i].name, tmppath, archive_file, metas[i].permissions);
			}
		}
	}
}

void traverse(char* archive_file, char* dirname, int count){
	char* root = trimmer(dirname);
	for(int j = 0; j<count; j++){
		printf("%s","-");
	}
	printf("%s\n", root);
	count +=2;
	struct meta metas[20];
	struct header h;
	write_ptr = fopen(archive_file,"rb");  // w for write, b for binary
	fread (&h, sizeof(struct header), 1, write_ptr);
	fseek (write_ptr, h.meta_offset, SEEK_SET);
	fread(&metas, sizeof(struct meta)*20, 1, write_ptr);
	fclose(write_ptr);
	for(int i = 0; i<h.num_elts; i++){
		if(strcmp(metas[i].parent,root)== 0){
			if(metas[i].isFile){
				for(int j = 0; j<count; j++){
					printf("%s","-");
				}
				printf("%s\n", metas[i].name);
			}else{
				traverse(archive_file, metas[i].name, count);
			}

		}
	}
}

tmp* recursiveDir ( char* dirname, char* archive_file, char* parent, tmp* tmp, meta metaRecords[20]) {
	int indexCount, dataOffset;
	indexCount = tmp->index;
	// printf("indexCount%d\n" , indexCount);
	dataOffset = tmp->dataOffset;
	struct stat st;
	DIR * dir_ptr ;
	struct dirent *direntp ;
	// mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	int count=0;		
	if ((dir_ptr = opendir (dirname)) == NULL)
		fprintf (stderr , " cannot open %s \n",dirname);
	else {
	// get the stat information for current directory
		stat(dirname, &st);
		if((st.st_mode & S_IFMT) == S_IFDIR){
			// printf("this is directory, %s, index: %d", trimmer(dirname), indexCount);
			metaRecords[indexCount].isFile = false;
			strcpy(metaRecords[indexCount].name, trimmer(dirname));
			strcpy(metaRecords[indexCount].parent, trimmer(parent));
			metaRecords[indexCount].permissions =  st.st_mode;
			indexCount++;
			tmp->index = indexCount;
		}
		while ((direntp = readdir (dir_ptr)) != NULL ) {
			count++;
			if (count < 3) continue;
			// printf ("inode %d of the entry %s index %d\n", (int) direntp ->d_ino , direntp -> d_name, indexCount );
			char source[1024];
			strcpy(source, dirname);
			strcat(source, "/");
			strcat(source, direntp -> d_name);
			stat(source, &st);
			// if file
			if ((st.st_mode & S_IFMT) == S_IFREG){
				strcpy(metaRecords[indexCount].name, direntp -> d_name);
				strcpy(metaRecords[indexCount].parent, trimmer(dirname));
				metaRecords[indexCount].isFile = true;
				tmp = copyAndWrite(source, archive_file, tmp, metaRecords);
				printf("test!!!!!!!%d", tmp ->dataOffset);
				indexCount = tmp->index;
			}
			// if dir
			else if ((st.st_mode & S_IFMT) == S_IFDIR){
				tmp->index = indexCount;
				tmp = recursiveDir(source, archive_file, dirname, tmp, metaRecords);
			} 
		}
		closedir (dir_ptr);
	}
	return tmp;
}

void archive(char* archive_file, char* dirname){
	addHeader(archive_file);
	struct meta metaRecords[20];
	struct tmp* tmp;
	tmp = (struct tmp*) malloc(sizeof(struct tmp));
	tmp -> dataOffset = sizeof(header);
	// printf("tmp off set %d", tmp->dataOffset);
	tmp -> index = 0;
	tmp = recursiveDir(dirname, archive_file, "..", tmp, metaRecords);
	updateHeader(tmp->dataOffset, tmp->index, archive_file);
	addMeta(archive_file, metaRecords);
	free(tmp);
}

void append(char* archive_file, char* dirname){
	int iscontained = 0;
	struct meta metas[20];
	struct header h;
	char tmp_name[100];
	strcpy(tmp_name, dirname);
	write_ptr = fopen(archive_file,"r+b"); 
	fread (&h, sizeof(struct header), 1, write_ptr);
	fseek (write_ptr, h.meta_offset, SEEK_SET);
	fread(&metas, sizeof(struct meta)*20, 1, write_ptr);
	struct tmp* tmp;
	tmp = (struct tmp*) malloc(sizeof(struct tmp));
	tmp -> dataOffset = h.meta_offset;
	tmp -> index = h.num_elts;
	char* name = trimmer(tmp_name);
	char* parent = trimmer2(tmp_name, name);
	// printf("\n\n 299 %s \n\n", dirname);
	// printf("%s", trimmer2(dirname, name));
	for(int i = 0; i<h.num_elts; i++){
		// printf("metas name %s", metas[i].name);
		if (strcmp(parent,metas[i].name) == 0){		
			iscontained =1;
			break;
		}
	}
	if(iscontained == 1){
		printf("here");
		printf("tmp test index: %d, offset: %d", tmp->index, tmp->dataOffset);
		printf("\ndirectory: %s\n", dirname);
		tmp = recursiveDir(dirname, archive_file, parent, tmp, metas);
	}
	else{
		// printf("here");
		tmp = recursiveDir(dirname, archive_file, "..", tmp, metas);
	}
	// printf("tmp : dataoffset: %d", tmp->dataOffset);
	// printf("tmp: index", tmp->index);
	updateHeader(tmp->dataOffset, tmp->index, archive_file);
	addMeta(archive_file, metas);
	free(tmp);
	fclose(write_ptr);
}

int main(int argc, char *argv[]){
	char * archive_file;
	char * dirName;
	for (int q = 0; q < argc; q++)
	{
		//archive file or directory
		if (strcmp(argv[q], "-c") == 0){
            archive_file = argv[q + 1];
			dirName = argv[q+2];
			archive(archive_file, dirName);
        }
		//append file or directory
		else if (strcmp(argv[q], "-a") == 0){
            archive_file = argv[q + 1];
			dirName = argv[q+2];
			append(archive_file, dirName);
		}
		//extract file
		else if (strcmp(argv[q], "-x") == 0){
            archive_file = argv[q + 1];
			dirName = argv[q+2];
			extract("..", dirName, archive_file, 0777);
		}
		//display metadata
		else if (strcmp(argv[q], "-m") == 0){
            archive_file = argv[q + 1];
			dirName = argv[q+2];
			read_metadata(archive_file);
		}
		else if (strcmp(argv[q], "-p") == 0){
            archive_file = argv[q + 1];
			dirName = argv[q+2];
			traverse(archive_file, dirName, 0);
	}
	}
}
