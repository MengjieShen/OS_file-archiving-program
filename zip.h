// int copyAndWrite(char fromFile[], char*toFile, int index);
struct meta {
  char name[20];
  int size;
  int offset;
  char parent[20];
} meta;

struct header {
  int num_elts;
  int meta_offset;
  int next;
} header;

struct meta metaRecords[20];

int copyAndWrite(char fromFile[],char* toFile, int index);
void updateHeader(int curr_offset, int numOfEle);
