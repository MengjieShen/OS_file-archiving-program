// int copyAndWrite(char fromFile[], char*toFile, int index);
# include <stdbool.h>
typedef struct meta {
  char name[20];
  int size;
  int offset;
  char parent[20];
  bool isFile;
  mode_t permissions;
} meta;

struct header {
  int num_elts; //total elements including directories
  int meta_offset;
  int next;
} header;

struct meta metaRecords[20];

int copyAndWrite(char* fromFile,char* toFile, int index);
void updateHeader(int offset, int numOfEle, char* archive_file);
char * trimmer(char * str);
void addHeader(char* archive_file);
void addMeta(char* archive_file);
void breakDir ( char* dirname, char* archive_file, char* parent);
void read_metadata(char* archive_file);
void extract(char root[1024], char path[1024], char* archive_file);
void extractfile(char path[1024], char* archive_file, int offset, int size, int permissions);
