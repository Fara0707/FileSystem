//
// Created by Kolia on 30.11.2019.
//

#ifndef FILESYSTEM_FILESYSTEM_H
#define FILESYSTEM_FILESYSTEM_H
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace std;

struct SuperBlock {
    char id[8];

    int blockSize;
    int blockCount;

    int inodeCount;
    int inodeMapOffset;
    int inodeTableOffset;

    int dataBlocksCount;
    int dataBlocksMapOffset;
    int dataBlocksTableOffset;
};

struct Inode {
    char name[64];

    int size;

 //   int accessType;
 //   char owner[64];

 //   int createTimestamp;
//    int modifyTimestamp;
 //   int lastAccessTimestamp;

    int offsetStart;
    int offsetFinish;
};

struct DataBlock {
//    DataBlock(SuperBlock sb) : sb(sb) {}
//
//    SuperBlock sb;
//    bool*bite = new bool[8*sb.blockSize];
//
//    virtual ~DataBlock() {
//        delete[](bite);
//    }
};



class FileSystem {
public:

    bool readFile(char *buf);
    bool writeFile(char *names, char *buf, int size);
    void listFiles();
    bool deleteFile(char *buf);
    bool createFile(char* name);

    int getFreeInodeNum();
    Inode createInode(char* name);
    int findfreeBlocks(int block);

    FileSystem(FILE *file) : _file(file) {
        init();
    };

    bool init();

    SuperBlock _super;
    FILE *_file;
private:
  //  SuperBlock _super;

};


#endif //FILESYSTEM_FILESYSTEM_H
