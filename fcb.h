#ifndef FCB_H
#define FCB_H

#include <string>
#include <time.h>
#include <vector>

enum FILE_TYPE { NORMAL, DIR};  //File type

struct FCB {
    char name[15];
    FILE_TYPE type;
    int location;
    int permission[9];
    int use_count;
    char time[20];
    int first_block;  //The first block pointed to.
    int parent; //Parent dir
    int size;
    std::vector<int> children;
};

#endif // FCB_H
