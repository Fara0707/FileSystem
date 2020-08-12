#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include "fileSystem.h"

using namespace std;

SuperBlock createSb(int size, int count) {
    SuperBlock block{};
    block.blockSize = size;
    block.blockCount = count;
    strcpy(block.id, "SB");

    block.inodeCount = ceil(block.blockCount * 0.1 + 0.5);
    block.inodeMapOffset = 0;
    block.inodeTableOffset = 0;

    int inodeMapBlocksCount = block.inodeCount % block.blockSize == 0 ?
                              block.inodeCount / block.blockSize : block.inodeCount / block.blockSize + 1;

    block.dataBlocksCount = block.blockCount - 1 - block.inodeCount - inodeMapBlocksCount;

    block.dataBlocksMapOffset = 0;
    block.dataBlocksTableOffset = 0;

    return block;
}

void writeSB(SuperBlock &block, FILE* name) {
    fseek(name, 0, SEEK_SET);
    char *data = (char *)calloc(1, block.blockSize);
    memcpy(data, &block, sizeof(SuperBlock));
    fwrite(data, block.blockSize, 1, name);
    free(data);
}

void writeBMID(SuperBlock &block, FILE* name) {
    block.inodeMapOffset = ftell(name) / block.blockSize;
    int inodeMapBlocksCount = block.inodeCount % block.blockSize == 0 ?
                              block.inodeCount / block.blockSize : block.inodeCount / block.blockSize + 1;

    char *data = (char *)calloc(1, block.blockSize);
    for (int i = 0; i < inodeMapBlocksCount; ++i) {
        fwrite(data, block.blockSize, 1, name);
    }
    free(data);
}

void writeMID(SuperBlock &block, FILE *name) {

    block.inodeTableOffset = ftell(name) / block.blockSize;

    char *data = (char *)calloc(1, block.blockSize);
    for (int i = 0; i < block.inodeCount; ++i) {
        fwrite(data, block.blockSize, 1, name);
    }

    free(data);
}

void writeBMBD(SuperBlock &block, FILE *name) {

    block.dataBlocksMapOffset = ftell(name) / block.blockSize;

    int dataMapBlocksCount = block.dataBlocksCount % block.blockSize == 0 ?
                             block.dataBlocksCount / block.blockSize : block.dataBlocksCount / block.blockSize + 1;

    char *dataBlock = (char *)calloc(1, block.blockSize);
    for (int i = 0; i < dataMapBlocksCount; ++i) {
        fwrite(dataBlock, block.blockSize, 1, name);
    }

    free(dataBlock);
}

void writeMBD(SuperBlock &block, FILE *name) {
    block.dataBlocksTableOffset = ftell(name) / block.blockSize;

    char *dataBlock = (char *)calloc(1, block.blockSize);

    for (int i = 0; i < block.dataBlocksCount; ++i) {
        fwrite(dataBlock, block.blockSize, 1, name);
    }
    free(dataBlock);
}

void writeFileSystem(FILE *name, int size, int count) {
    SuperBlock sb = createSb(size, count);
    writeSB(sb, name);
    writeBMID(sb, name);
    writeMID(sb, name);
    writeBMBD(sb, name);
    writeMBD(sb, name);
    writeSB(sb, name);
}


int main() {
    char name[] = "FS";
    int blockSize = 256;
    int  blockCount = 100;
    //двепрограммы в одной, создание фс и работа с ней

    FILE * fs = fopen(name, "wb");
    writeFileSystem(fs, blockSize, blockCount);

    fclose(fs);
    fs = fopen(name, "r+b");
  //  FILE * fs = fopen(name, "r+b");
    FileSystem fileSystem(fs);


    bool go = true;
    while(go) {
        int cs;
        cout << "1. readFile\n";
        cout << "2. writeFile\n";
        cout << "3. deleteFile\n";
        cout << "4. listFiles\n";
        cout << "5. createFile\n";
        cout << "6. Exit\n";
        cin >> cs;
        switch ( cs ) {
            case 1: {
                cout << "Enter file name" << endl;
                string s1;
                cin>>s1;
                int size1 = s1.length();
                char arr1[size1 + 1];
                strcpy(arr1, s1.c_str());
                fileSystem.readFile(arr1);
                break;
            }
            case 2: {
                cout<< "Enter file name" << endl;
                string name;
                cin>>name;
                int sizeName = name.length();
                char names[sizeName + 1];
                strcpy(names, name.c_str());

                cout << "Enter line" << endl;
                string s2;
                cin>>s2;
                int size2 = s2.length();
                char arr2[size2 + 1];
                strcpy(arr2, s2.c_str());
                fileSystem.writeFile(names, arr2, size2 + 1);
                break;
            }
            case 3: {
                cout << "Enter file name" << endl;
                string s1;
                cin>>s1;
                int size1 = s1.length();
                char arr1[size1 + 1];
                strcpy(arr1, s1.c_str());
                fileSystem.deleteFile(arr1);
                break;
            }
            case 4: {
                fileSystem.listFiles();
                break;
            }
            case 5: {
                cout << "Enter file name" << endl;
                string s1;
                cin>>s1;
                int size1 = s1.length();
                char name[size1 + 1];
                strcpy(name, s1.c_str());
                fileSystem.createFile(name);
                break;
            }
            case 6:
                go = false;
                break;
        }
    }

    fclose(fs);

    return 0;
}