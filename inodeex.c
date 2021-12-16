//aceesing information from inode
# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/stat.h>

int main ( int argc , char * argv []){
	struct stat statbuf ;

	if (stat (argv [1] , &statbuf ) == -1)
		perror ("Failed to get file status ");
	else {
		//atime : accessed time
		printf ("inode number: %llu\n",statbuf.st_ino) ;
		printf ("Time/Date: %s", ctime(&statbuf.st_atime ));
		printf ("---------------------------------\n");
		printf ("entity name: %s\n",argv [1]) ;
		//mtime: modified time
		printf ("modified: %s", ctime(&statbuf.st_mtime));
}
	return (1) ;
}