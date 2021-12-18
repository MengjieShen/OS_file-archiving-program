# File archiver
### Vivi Wei, Mengjie Shen

#### This application is built with C language. 

### 1. Starting commands:

The application is an archiving tool that can zip a given directory into a file  with postfix `.ad`. It supports flattening the hierarchies in the directory, unzipping the file, and selectively retrieve files or directories from the zipped file. For fast start, type `make run1`, which will archive a default file or directory. For manual testing, type `./adzip -c "test.ad" <File/Directory>`. 



### 2. Code Structures:

#### 1. Structs

The struct `meta` keeps the record of a entry. An entry stores either the information of a file or directory. All the meta entries are stored in an array `metaRecords` defined in the main file, and will be added to the end of the `.ad` file after all the data has been written. The struct is:

```c
typedef struct meta {
  char name[20];
  int size;
  int offset;
  char parent[20];
  bool isFile;
  mode_t permissions;
} meta;
```


The struct `tmp` is a storing unit that keeps track of the current location in and the number of files written to the  `.ad` file. The struct is:

```c
typedef struct tmp {
  int dataOffset;
  int index;
} tmp;
```

The struct `header` is the header in the `.ad` file. It keeps the number of all the elements including directories that are currently in the `.ad` file. It also stores the starting place of the `metaRecords`. It is added to the file at the start.

```c
typedef struct header {
  int num_elts;
  int meta_offset;
} header;
```

#### 2. Functions

When user wants to archive a file or directory, function `archive()` is invoked. It first adds a header to the file, then recursively flattens the directory given. In function `recursiveDir`, it opens the directory and checks all the inner children one by one. If it is a directory, `recursiveDir` will be called again. If it is a file, it will be directly written to the target file. They will both be entered in the `metaRecords`.

When user wants to append a random file or directory into an existing `.ad` file, function `append()` will be invoked. It will first read from the `.ad` file and store the header and meta data in the short memory, then operates similarly to `archive()` and then update the metadata and header information to the `.ad` file.

When user wants to recreate the directory or file from a `.ad` file, function `extract()` is invoked. It will extract a file or directory from the `.ad` file. It first reads header and metadata in the `.ad` file. If an entry is a file, it will invoke `extractfile()` to output the file. If an entry is a directory, `extract()` will be called again. 

When user wants to view the hierarchy of the directory or file stored in `.ad` file, function `traverse()` will be invoked.  It first reads header and metadata in the `.ad` file. If an entry is a file, it will print the name to the terminal. If an entry is a directory, `traverse()` will be called again. 

### 3. How to run:

The application can be run from the console.

1. To archive a file or directory:

```C
./adzip -c <archive-file> <file/directory list>
```
or  
```C
make run1
```

\
2. To append file or directory to an existing `.ad` file (You can both append the sub-directory or sub-file of the existing directory, or a director or file that is apart from the existing directory):

```C
./adzip -a <archive-file> <file/directory list>
```
or 
```C
make run2
```
\
3. To unzip an existing `.ad` file:
```C
./adzip -x <archive-file> <file/directory list>
```
or
```C
make run3
```
\
4. To display metadata:
```C
./adzip -m <archive-file> <file/directory list>
```
or
```C
make run4
```
\
5. To display the hierarchy in an existing `.ad` file:

```C

./adzip -p <archive-file> <file/directory list>
```

or
```C
make run5
```

To clean the executables:

```C
make clean
```

### 4. Acknowledgement:

Inspired by https://github.com/RishiSadhir/Pittar.