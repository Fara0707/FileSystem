//
// Created by Kolia on 30.11.2019.
//
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cmath>

#include "fileSystem.h"

bool FileSystem::init() {
     return fread(&_super, sizeof(SuperBlock), 1, _file) == sizeof(SuperBlock) && strcmp(_super.id, "SB") == 0;
}


bool FileSystem::deleteFile(char *buf) {
    fseek(_file, _super.inodeMapOffset * _super.blockSize, SEEK_SET);

    char *inodeMap = (char *)malloc(_super.inodeCount);
    fread(inodeMap, _super.inodeCount, 1, _file);

    Inode inode;
    for (int i = 0; i < _super.inodeCount; ++i) {
        if (inodeMap[i] != '0') {
            fseek(_file, (_super.inodeTableOffset + i) * _super.blockSize, SEEK_SET);
            fread(&inode, sizeof(Inode), 1, _file);
            if (!strcmp(inode.name, buf)){

                fseek(_file, _super.inodeMapOffset * _super.blockSize + i , SEEK_SET);
                char *data = (char *)calloc(1, 1);
                memcpy(data, "0", 1);
                fwrite(data, 1, 1, _file);


                if(inode.offsetStart != -1) {
                    for (int j = 0; j <= inode.offsetFinish - inode.offsetStart; ++j) {
                        fseek(_file, _super.dataBlocksMapOffset * _super.blockSize + inode.offsetStart + j, SEEK_SET);
                        fwrite(data, 1, 1, _file);
                    }
                }
                free(data);
                return true;
            }
        }
    }
    free(inodeMap);
    return false;
}

bool FileSystem::writeFile(char *names, char *buf, int size) {
    fseek(_file, _super.inodeMapOffset * _super.blockSize, SEEK_SET);

    char *inodeMap = (char *) malloc(_super.inodeCount);
    fread(inodeMap, _super.inodeCount, 1, _file);

    Inode inode;
    for (int i = 0; i < _super.inodeCount; ++i) {
        if (inodeMap[i] != '0') {
            fseek(_file, (_super.inodeTableOffset + i) * _super.blockSize, SEEK_SET);
            fread(&inode, sizeof(Inode), 1, _file);
            if (!strcmp(inode.name, names)) {
                int needBlock = ceil(size / _super.blockSize + 0.5);

                inode.offsetStart = findfreeBlocks(needBlock);
                cout<<inode.offsetStart<<endl;
                inode.offsetFinish = inode.offsetStart + needBlock  - 1;
                inode.size = size - 1;

                char *ind = (char *)calloc(1, _super.blockSize);

                memcpy(ind, &inode, sizeof(Inode));
                fseek(_file, (_super.inodeTableOffset + i) * _super.blockSize, SEEK_SET);
                fwrite(ind, _super.blockSize, 1, _file);
                free(ind);

                //записать файл в фс отметить на карте занятость

                fseek(_file, _super.dataBlocksMapOffset * _super.blockSize, SEEK_SET);
                char *dataBlocksMap = (char *) malloc(_super.dataBlocksCount * _super.blockSize);
                fread(dataBlocksMap, _super.dataBlocksCount * _super.blockSize, 1, _file);

                char *data = (char *)calloc(1, 1);
                memcpy(data, "1", 1);

                for (int j = 0; j <= inode.offsetFinish - inode.offsetStart; ++j) {
                    fseek(_file, _super.dataBlocksMapOffset * _super.blockSize + inode.offsetStart + j, SEEK_SET);
                    fwrite(data, 1, 1, _file);
                }
                free(data);

                fseek(_file, (_super.dataBlocksTableOffset + inode.offsetStart) * _super.blockSize, SEEK_SET);

                fwrite(buf, size, 1, _file);

                return true;
            }
        }
    }

    free(inodeMap);
    cout<< "File not create"<<endl;
    return false;
}

int FileSystem::findfreeBlocks(int block) {
    fseek(_file, _super.dataBlocksMapOffset * _super.blockSize, SEEK_SET);

    char *dataBlocksMap = (char *) malloc(_super.dataBlocksCount);
    fread(dataBlocksMap, _super.dataBlocksCount, 1, _file);

//    int count = 0;
//    int start = 0;
    for (int i = 0; i < _super.dataBlocksCount; ++i) {
        if (dataBlocksMap[i] != '1') {
            return i;
        }
    }
   // return 0;
}

bool FileSystem::readFile(char *buf) {
    fseek(_file, _super.inodeMapOffset * _super.blockSize, SEEK_SET);

    char *inodeMap = (char *)malloc(_super.inodeCount);
    fread(inodeMap, _super.inodeCount, 1, _file);

    for (int i = 0; i < _super.inodeCount; ++i) {
        if (inodeMap[i] == '1') {
            Inode inode;
            fseek(_file, (_super.inodeTableOffset + i)  * _super.blockSize, SEEK_SET);
            fread(&inode, sizeof(Inode), 1, _file);
            if (!strcmp(inode.name, buf)){
                fseek(_file, (_super.dataBlocksTableOffset + inode.offsetStart) * _super.blockSize, SEEK_SET);

                char a;
                for(int j = 0; j < inode.size; ++j){
                    fread(&a, sizeof(char), 1, _file);
                    cout<<a;
                }
                cout<<endl;
                free(inodeMap);
                return true;
            }
        }
    }

    free(inodeMap);
    return false;
}

void FileSystem::listFiles() {
    fseek(_file, _super.inodeMapOffset * _super.blockSize, SEEK_SET);

    char *inodeMap = (char *)malloc(_super.inodeCount);
    fread(inodeMap, _super.inodeCount, 1, _file);


    for (int i = 0; i < _super.inodeCount; ++i) {
        if (inodeMap[i] == '1' ) {
            Inode inode;
            fseek(_file, (_super.inodeTableOffset + i)  * _super.blockSize, SEEK_SET);
            fread(&inode, sizeof(Inode), 1, _file);
            cout<< "File number " << i <<": " << inode.name<<endl;
        }
    }

    free(inodeMap);
}

int FileSystem::getFreeInodeNum() {
    int count = 0;
    fseek(_file, _super.inodeMapOffset * _super.blockSize, SEEK_SET);

    char *inodeMap = (char *)malloc(_super.inodeCount);
    fread(inodeMap, _super.inodeCount, 1, _file);

    for (int i = 0; i < _super.inodeCount; ++i) {
        if (inodeMap[i] != '1') {
            return count;
        }
        count++;
    }

    return -1;
}

Inode FileSystem::createInode(char* name) {
    Inode inode;
    strcpy(inode.name, name);
 //   strcpy(inode.owner, "owner");
    inode.size = 0;
    inode.offsetStart = -1;
    inode.offsetFinish = -1;
    return inode;
}

bool FileSystem::createFile(char *name) {
    int freeInode = getFreeInodeNum();
    if(freeInode == -1){
        return false;
    }
    else {
        fseek(_file, (_super.inodeTableOffset + freeInode) *_super.blockSize, SEEK_SET);
        Inode i = createInode(name);
        char *inode = (char *)calloc(1, _super.blockSize);

        memcpy(inode, &i, sizeof(Inode));
        fwrite(inode, _super.blockSize, 1, _file);
        free(inode);

        fseek(_file, _super.inodeMapOffset * _super.blockSize + freeInode , SEEK_SET);
        char *data = (char *)calloc(1, 1);
        memcpy(data, "1", 1);
        fwrite(data, 1, 1, _file);
        free(data);
        return true;
    }
}


