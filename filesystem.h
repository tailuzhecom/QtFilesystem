#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include "fcb.h"

#include <map>
#include <string>
#include <vector>

#define BLOCK_NUM 100
#define BLOCK_SIZE 512

enum WRITE_MODE {COVER, APPEND};

class FileSystem {
public:
    FileSystem();
    void format();
    int createFile(char* name);  //创建文件
    int write(std::string fileName, char* content, WRITE_MODE mode);
    int read(std::string fileName, std::string& content);
    std::vector<std::vector<std::string> > dir();   //显示当前文件夹中的内容
    int mkdir(char* dirName);  //创建文件夹
    int deleteFile(char* fileName, std::vector<int> children_nums, bool all);
    std::vector<int> getChildren();
    int changeDir(char* dirName);
    int findFreeBlock();
    int login();
    int setName(char* newName, char* oldName, FILE_TYPE type);
    int setName(char* newName, int block_num);
    int getBlockNumByName(char* name);
    void pwd();
    ~FileSystem();

private:
    FCB* m_current_dir;
    char* m_blocks;
    int m_start_addr;
    int m_FAT[BLOCK_NUM];
    int m_bitmap[BLOCK_NUM];  //Disk Allocation Table
    std::map<std::string, FCB*> m_open_file;
};
#endif // FILESYSTEM_H
