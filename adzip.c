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
# include <limits.h>
# include "zip.h"
# define BUFFSIZE 1024

FILE * write_ptr;

tmp* copyAndWrite(char* fromFile,char* toFile, tmp* tmp, meta metaRecords[20])
{	
	int n, from , to, index, dataOffset;
	char buf[BUFFSIZE];
	memset(buf, 0, BUFFSIZE);
	mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	struct stat statbuf;
	index = tmp->index;
	dataOffset = tmp->dataOffset;
	// printf("\ndataoffset: %d\n", dataOffset);
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
		metaRecords[index].st_gid = statbuf.st_gid;
		metaRecords[index].st_uid = statbuf.st_uid;
		metaRecords[index].offset = dataOffset;
		dataOffset += statbuf.st_size;
		metaRecords[index].permissions = statbuf.st_mode;
	}
	close(from);
	close(to);
	tmp->dataOffset =  dataOffset;
	tmp->index ++;
	return tmp;
}

//add header data to the archive file
void addHeader(char* archive_file) {
	struct header header;
	header.meta_offset = sizeof(struct header);                   
	header.num_elts = 0;
	// write the header
	write_ptr = fopen(archive_file,"wb");  // w for write, b for binary
    fwrite(&header, sizeof(struct header), 1, write_ptr);
	fclose(write_ptr);
}

//add meta data to the archive file
void addMeta(char* archive_file, meta metaRecords[20]) {
	struct header h;
	write_ptr = fopen(archive_file,"r+b");  // w for write, b for binary
	fread (&h, sizeof(struct header), 1, write_ptr);
	fseek(write_ptr, h.meta_offset, SEEK_SET);
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
	h.meta_offset = offset;
	fseek(write_ptr, 0, SEEK_SET);
	fwrite (&h, sizeof(struct header), 1, write_ptr);
	fclose(write_ptr);
}

void read_metadata(char* archive_file){
	struct meta metas[20];
	struct header h;
	write_ptr = fopen(archive_file,"rb");  // w for write, b for binary
	// Update current header
	fread (&h, sizeof(struct header), 1, write_ptr);
	// printf("meta offset from reading header: %d \n", h.meta_offset);
	fseek (write_ptr, h.meta_offset, SEEK_SET);
	// // fread(&m, sizeof(struct meta), 1, write_ptr);
	fread(metas, sizeof(struct meta)*20, 1, write_ptr);
	printf("=======================================================\n");
    printf("=======================================================\n");
	for (int i = 0; i< h.num_elts; i++){
		if(metas[i].isFile){
			printf("\nFile: ");
			// printf("index : %d", i);
			printf("file name : %s ", metas[i].name);
			printf("size : %d ", metas[i].size);
			printf("offset : %d ", metas[i].offset);
			printf("parent : %s ", metas[i].parent);
			printf("user id : %d ", metas[i].st_uid);
			printf("group id : %d ", metas[i].st_gid);
			printf("permission: %d ", metas[i].permissions);
		}else{
			printf("\nDirectory: ");
			// printf("index : %d", i);
			printf("Directory name : %s ", metas[i].name);
			printf("parent : %s ", metas[i].parent);
			printf("user id : %d ", metas[i].st_uid);
			printf("group id : %d ", metas[i].st_gid);
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
	// printf("path %s", path);
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
	char tmppath[1024];
	strcpy(tmppath, path);
	strcat(tmppath, "/");
	strcat(tmppath, trimmer(root));
	if(mkdir(tmppath, 0777) == -1){
		fprintf(stderr, "Error making dir: %s\n", path);
		// free(m);
		perror("Error: ");
		exit(1);
	}
	chmod(path, permissions);
	struct meta metas[20];
	struct header h;
	write_ptr = fopen(archive_file,"rb");  
	fread (&h, sizeof(struct header), 1, write_ptr);
	fseek (write_ptr, h.meta_offset, SEEK_SET);
	fread(&metas, sizeof(struct meta)*20, 1, write_ptr);
	fclose(write_ptr);
	
	for(int i = 0; i<h.num_elts; i++){
		if(strcmp(metas[i].parent,trimmer(root))== 0){
				//file
				if(metas[i].isFile){
					char tmppath1[1024];
					strcpy(tmppath1, tmppath);
					strcat(tmppath1, "/");
					strcat(tmppath1, metas[i].name);
					extractfile(tmppath1, archive_file, metas[i].offset, metas[i].size, metas[i].permissions);
				}
				//directory
				else{
					char tmppath1[1024];
					strcpy(tmppath1, tmppath);
					extract(metas[i].name, tmppath1, archive_file, metas[i].permissions);
			}
		}
	}
}

void traverse(char* archive_file, char* dirname, int count){
	struct meta metas[20];
	struct header h;
	write_ptr = fopen(archive_file,"rb");  // w for write, b for binary
	fread (&h, sizeof(struct header), 1, write_ptr);
	fseek (write_ptr, h.meta_offset, SEEK_SET);
	fread(&metas, sizeof(struct meta)*20, 1, write_ptr);
	fclose(write_ptr);
	char* root = trimmer(dirname);
	bool find = 0;
	for(int i=0; i< h.num_elts; i++){
		if(strcmp(metas[i].name, root) == 0){
			find = 1;
			break;
		}
	}
	if(!find){
		strcpy(root,"..");
	}
	for(int j = 0; j<count; j++){
		printf("%s","-");
	}
	printf("%s\n", root);
	count +=2;

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
	dataOffset = tmp->dataOffset;
	struct stat st;
	DIR * dir_ptr ;
	struct dirent *direntp ;
	int count=0;	
	// get the stat information for current directory
	stat(dirname, &st);
	if((st.st_mode & S_IFMT) == S_IFDIR){
		metaRecords[indexCount].isFile = false;
		strcpy(metaRecords[indexCount].name, trimmer(dirname));
		strcpy(metaRecords[indexCount].parent, trimmer(parent));
		metaRecords[indexCount].permissions =  st.st_mode;
		metaRecords[indexCount].st_gid = st.st_gid;
		metaRecords[indexCount].st_uid = st.st_uid;
		indexCount++;
		tmp->index = indexCount;
		if ((dir_ptr = opendir (dirname)) == NULL){
			fprintf (stderr , " cannot open %s \n",dirname);
		}else {
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
					indexCount = tmp->index;
					strcpy(metaRecords[indexCount].name, direntp -> d_name);
					strcpy(metaRecords[indexCount].parent, trimmer(dirname));
					metaRecords[indexCount].isFile = true;
					tmp = copyAndWrite(source, archive_file, tmp, metaRecords);
					indexCount = tmp->index;
				}
				// if dir
				else if ((st.st_mode & S_IFMT) == S_IFDIR){
					tmp->index = indexCount;
					tmp = recursiveDir(source, archive_file, dirname, tmp, metaRecords);
				} 
			}
			closedir (dir_ptr);
			return tmp;
		}
	}else if((st.st_mode & S_IFMT) == S_IFREG){
		strcpy(metaRecords[indexCount].name, trimmer(dirname));
		strcpy(metaRecords[indexCount].parent, trimmer(parent));
		metaRecords[indexCount].isFile = true;
		printf("here");
		printf(" %s", dirname);
		tmp = copyAndWrite(dirname, archive_file, tmp, metaRecords);
		return tmp;
	}	
	
}

void archive(char* archive_file, char* dirname){
	addHeader(archive_file);
	struct meta metaRecords[20];
	struct tmp* tmp;
	tmp = (struct tmp*) malloc(sizeof(struct tmp));
	tmp -> dataOffset = sizeof(header);
	tmp -> index = 0;
	tmp = recursiveDir(dirname, archive_file, "..", tmp, metaRecords);
	// updateHeader(tmp->dataOffset, tmp->index, archive_file);
	// addMeta(archive_file, metaRecords);
	free(tmp);
	tmp = NULL;
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
	for(int i = 0; i<h.num_elts; i++){
		if (strcmp(parent,metas[i].name) == 0){		
			iscontained =1;
			break;
		}
	}
	struct stat st;
	stat(dirname, &st);
	if(iscontained == 1){
		tmp = recursiveDir(dirname, archive_file, parent, tmp, metas);
	}
	else{
		tmp = recursiveDir(dirname, archive_file, "..", tmp, metas);
	}
	updateHeader(tmp->dataOffset, tmp->index, archive_file);
	addMeta(archive_file, metas);
	fclose(write_ptr);
	free(tmp);
	tmp = NULL;
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
			//get current directory
		   	char cwd[1024];
			if (getcwd(cwd, sizeof(cwd)) != NULL) {
				printf("Current working dir: %s\n", cwd);
			} else {
				perror("getcwd() error");
				return 1;
			}
            archive_file = argv[q + 1];
			dirName = argv[q+2];
			int isFile = 0;
			int index = 0;
			struct meta metas[20];
			struct header h;
			write_ptr = fopen(archive_file,"rb");  
			fread (&h, sizeof(struct header), 1, write_ptr);
			fseek (write_ptr, h.meta_offset, SEEK_SET);
			fread(&metas, sizeof(struct meta)*20, 1, write_ptr);
			fclose(write_ptr);

			for(int i =0; i< h.num_elts; i++){
				if (strcmp(trimmer(dirName), metas[i].name) == 0){
					if(metas[i].isFile){
						isFile = 1;
						index = i;
					}
					break;
				}
			}
			
			if(isFile){
				char tmppath1[1024];
				strcpy(tmppath1, cwd);
				strcat(tmppath1, "/");
				strcat(tmppath1, trimmer(dirName));
				extractfile(tmppath1, archive_file, metas[index].offset, metas[index].size, metas[index].permissions);
			}else{
				extract(dirName, cwd, archive_file, 0777);
			}
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
