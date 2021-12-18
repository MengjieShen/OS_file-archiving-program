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

/*   Given a long path ie: home/hi/something/whatever////
**   will return the last element, ie: something
*/
char* trimmer2(char * str, char* end) {
//    char string[50] = "Hello! We are learning about strtok";
   // Extract the first token
   char * token = strtok(str, "/");
   char* last;
   // loop through the string to extract all other tokens
   while( true ) {
    //   printf( " %s\n", token ); //printing each token
      last = token;
      token = strtok(NULL, "/");
      if(token == NULL){
          last = "..";
          break;
      }
      if(strcmp(token,end) ==0){
          break;
      }
   }
   return last;
}