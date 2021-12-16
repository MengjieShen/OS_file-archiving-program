#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define PERMS 0644

char *workfile = "mytest";

main(){
	
	int filedes;

	if ((filedes = open(workfile, O_CREAT|O_RDWR, PERMS)) == -1 )
	{
		perror("creating..");
		exit(1);
	}
	else
	{
		printf("Managed to get to the file successfully\n");
	}

	exit(0);

}