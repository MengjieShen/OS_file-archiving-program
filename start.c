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

int main()
{
    FILE* fp = fopen("test.bin", "w+b");
    fwrite("Header is one", 100, 1, fp);
    fclose(fp);
    return 0;
}
