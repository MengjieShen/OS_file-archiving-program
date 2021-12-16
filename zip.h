// int copyAndWrite(char fromFile[], char*toFile, int index);
int copyAndWrite(char fromFile[],char* toFile, meta record)
struct {
  char name[200];
//   char name_trim[200];
  int size;
  int offset;
  int dir;
//   int root;
  char parent_folder[200];
  struct meta* next;
  struct meta* child;
//   char parent_folder_trim[200];
} meta;

struct {
  int num_elts;
  int meta_offset;
  int next;
} header;

// struct meta metaRecords[20];