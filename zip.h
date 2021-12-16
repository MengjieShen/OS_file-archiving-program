// int copyAndWrite(char fromFile[], char*toFile, int index);
struct meta {
  char name[200];
  int size;
  int offset;
  int dir;
  char parent_folder[200];
  struct meta* next;
  struct meta* child;
} meta;

struct header {
  int num_elts;
  int meta_offset;
  int next;
} header;

struct meta metaRecords[20];

int copyAndWrite(char fromFile[],char* toFile, struct meta record);
