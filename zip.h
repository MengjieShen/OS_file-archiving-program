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

int copyAndWrite(char fromFile[],char* toFile, int index);
void updateHeader(int curr_offset, int numOfEle);
char * trimmer(char * str);
void addHeader();
void addMeta();
void breakDir ( char dirname []);
void updateHeader(int offset, int numOfEle);
void read_metadata();
void extract(char* root, char * relative_path);
