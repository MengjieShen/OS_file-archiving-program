//program to do ls showing inode number of all files in current directory
# include <stdio.h>
# include <sys/types.h>
# include <dirent.h>
#include <string.h>
# include "zip.h"

void do_ls ( char dirname []) {
	DIR * dir_ptr ;
	struct dirent *direntp ;
	char const* mytest = "mytest";

	if ((dir_ptr = opendir (dirname)) == NULL)
		fprintf (stderr , " cannot open %s \n",dirname);
	else {
		int i=0;
		while ((direntp = readdir (dir_ptr)) != NULL ) {
            //get the inode number and file name
			i++;
			if (i < 3) continue;
			printf ("inode %d of the entry %s \n", (int) direntp ->d_ino , direntp -> d_name );
			char source[1024] = "testDir/";
			strcat(source ,direntp -> d_name);
            copyAndWrite(source, mytest);
        }
		
		closedir (dir_ptr);
	}
}

int main () {
	char dirName[] = "testDir";
	do_ls (dirName);
	
}