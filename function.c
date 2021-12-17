# include <stdio.h>
# include <sys/types.h>
# include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "zip.h"

//seek to the end of the file and append the current meta data to it
void append_meta(char * out)
{
    int i = 0;
    struct stat st;
    struct header h;

    FILE *fp = fopen(out, "r+");
    fseek(fp, 0, SEEK_SET);
    fread(&h, sizeof(header), 1, fp);
    fseek(fp, 0, SEEK_END);
    fwrite(metaRecords, sizeof(meta), h.num_elts, fp);
    fclose(fp);
}


void update_meta(char * file, int size)
{
    struct header h;
    int i;
    int old_offset, old_next;
    meta * m;
    char path[300];
    FILE *fp = fopen(file, "r+");
    m = (meta *) malloc(sizeof(meta));
    do
    {
        // Update current header
        fread (&h, sizeof(header), 1, fp);
        old_offset = h.meta_offset;
        h.meta_offset += size;
        old_next = h.next;
        if (h.next > 0)
        {
            h.next += size;
        }
        fseek (fp, (-1)*(sizeof(header)), SEEK_CUR);
        fwrite (&h, sizeof(header), 1, fp);

        // update header's metadata
        fseek (fp, old_offset, SEEK_SET);
        m = realloc(m, sizeof(meta) * h.num_elts);
        fread (m, sizeof(meta), h.num_elts, fp);
        for (i = 0; i < h.num_elts; ++i)
        {
            m[i].offset += size;
        }
        fseek (fp, old_offset, SEEK_SET);
        fwrite (m, sizeof(meta), h.num_elts, fp);\

        // get next header
        fseek (fp, old_next, SEEK_SET);

    } while (old_next > 0);

    // clean up
    fclose(fp);
    free(m);
}