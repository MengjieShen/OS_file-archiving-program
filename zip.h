# include <stdbool.h>
typedef struct meta {
  char name[20];
  int size;
  int offset;
  char parent[20];
  bool isFile;
  mode_t permissions;
} meta;

typedef struct tmp{
  int dataOffset;
  int index;
}tmp;

typedef struct header {
  int num_elts; //total elements including directories
  int meta_offset;
} header;

// struct meta metaRecords[20];


tmp* copyAndWrite(char* fromFile,char* toFile, tmp* tmp, meta metaRecords[20]);
void updateHeader(int offset, int numOfEle, char* archive_file);
char * trimmer(char * str);
char* trimmer2(char * str, char* end);
void addHeader(char* archive_file);
void addMeta(char* archive_file, meta metaRecords[20]) ;
void breakDir ( char* dirname, char* archive_file, char* parent);
void read_metadata(char* archive_file);
void extract(char root[1024], char path[1024], char* archive_file, int permissions);
void extractfile(char path[1024], char* archive_file, int offset, int size, int permissions);
void traverse(char* archive_file, char* dirname, int count);
tmp* recursiveDir ( char* dirname, char* archive_file, char* parent, tmp* tmp, meta metaRecords[20]);
void archive(char* archive_file, char* dirname);
