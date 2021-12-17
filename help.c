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

/*   Given a long path ie: home/hi/something/whatever////
**   will return the last element, ie: whatever
*/
char * trimmer(char * str)
{
    char * end = str + strlen(str) - 1;
    if (*end == '/')
    {
        while (*end == '/' && end >= str)
            --end;
        *(end+1) = 0;

        while (*end != '/' && end >= str)
            --end;
        ++end;

        return end;
    }
    else
    {
        while (*end != '/' && end >= str)
            --end;
        ++end;

        return end;
    }
}