#include "filesystem.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <QDateTime>

using namespace std;

FileSystem::FileSystem() {
    m_blocks = (char*)malloc(BLOCK_SIZE * BLOCK_NUM);  //Allocate the space.
    format();
}

void FileSystem::format()
{
    cout << "format" << endl;
    memset(m_blocks, 0, BLOCK_SIZE * BLOCK_NUM);
    m_current_dir = (FCB*)m_blocks;
    m_current_dir->use_count = 0;
    m_current_dir->location = 0;
    strcpy(m_current_dir->name, "root");
    m_current_dir->type = DIR;
    m_current_dir->parent = -1;
    m_current_dir->first_block = 0;
    memset(m_FAT, 0, sizeof(m_FAT));    //Init FAT
    memset(m_bitmap, 0, sizeof(m_bitmap));
    m_FAT[0] = BLOCK_NUM - 1;    //FAT[0] is used to record the free blocks.
    m_bitmap[0] = 1;
}

//Return the number of the free blocks. If all the block have been used, return -1.
int FileSystem::findFreeBlock() {
    if(m_FAT[0] == 0)
        return -1;
    for(int i = 0; i < BLOCK_NUM; i++)
        if(m_bitmap[i] == 0)
            return i;
    return -1;
}



//Create normal file, success return 0, fail return -1.
int FileSystem::createFile(char name[15]) {
    FCB* fcb = NULL;
    for(const auto& file_block_num : m_current_dir->children) { //If a file in current dir has common name, return -2
        fcb = (FCB*)(m_blocks + file_block_num * BLOCK_SIZE);
        if(strcmp(fcb->name, name) == 0 && fcb->type == NORMAL)
            return -2;
    }
    int free_block_num_fcb = findFreeBlock();
    int free_block_num_first = findFreeBlock();
    if(free_block_num_fcb == -1 || free_block_num_first == -1) {
        cout << "Storage has been used up." << endl;
        return -1;
    }
    FCB* new_file = (FCB*)(m_blocks + free_block_num_fcb * BLOCK_SIZE);
    new_file->children.clear();
    new_file->parent = m_current_dir->location;
    strcpy(new_file->name, name);
    new_file->use_count = 0;
    m_current_dir->children.push_back(free_block_num_fcb);
    new_file->location = free_block_num_fcb;
    new_file->size = 0;
    m_bitmap[free_block_num_fcb] = 1;
    m_FAT[0]--;
    free_block_num_first = findFreeBlock();
    new_file->first_block = free_block_num_first;

    QDateTime current_date_time = QDateTime::currentDateTime();
    strcpy(new_file->time, current_date_time.toString("yy/MM/dd hh:mm:ss").toLatin1().data());

    m_bitmap[free_block_num_first] = 1;
    m_FAT[0]--;
    return 0;
}

//Write into the file, success return 0, fail return -1.
int FileSystem::write(std::string fileName, char* content, WRITE_MODE mode) {
    bool fileExist = false;
    FCB* fcb = NULL;
    for(const auto& block_num : m_current_dir->children) {
        fcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
        if(fcb->name == fileName && fcb->type == NORMAL) {   //If the file already exists.
            fileExist = true;
            break;
        }
    }
    if(fileExist == false)
        return -1;
    int current_block_num = fcb->first_block;
    char* data = (char*)(m_blocks + fcb->first_block * BLOCK_SIZE);
    if(mode == COVER) {    //Cover
        current_block_num = m_FAT[current_block_num];
        while(current_block_num != 0) {   //Free the file blocks except its first block
            m_bitmap[current_block_num] = 0;
            m_FAT[0]++;
            int next_block_num = m_FAT[current_block_num];
            m_FAT[current_block_num] = 0;
            current_block_num = next_block_num;
        }
        current_block_num = fcb->first_block;
        fcb->size = 0;
        int writed_num = 0;
        int content_index = 0;
        while (char c = content[content_index++]) {
            if(writed_num >= BLOCK_SIZE) {  //Current block has been used up.
                int free_block_num = findFreeBlock();
                if(free_block_num == -1) {
                    cout << "Storage has been used up." << endl;
                    return -1;
                }
                m_FAT[current_block_num] = free_block_num;
                current_block_num = free_block_num;
                m_bitmap[current_block_num] = 1;
                m_FAT[0]--;
                writed_num = 0;
                data = (char*)(m_blocks + current_block_num * BLOCK_SIZE);   //Update the start addr of current block
            }
            data[writed_num++] = c;
            if(c == '\0')
                break;
            fcb->size++;

        }
    }
    else if(mode == APPEND) {   //append
        int file_size = fcb->size;
        //The current block is the block_offset + 1 th block
        int block_offset = file_size % BLOCK_SIZE;
        //The bytes the block has been used.
        int writed_num = fcb->size - block_offset * BLOCK_SIZE;
        //Find the Last block of this file
        while(m_FAT[current_block_num] != 0)
            current_block_num = m_FAT[current_block_num];
        data = (char*)(m_blocks + current_block_num * BLOCK_SIZE);    //The start addr of current block
        while(char c = getchar()) {
            if(c == '~')  {
                data[writed_num] = '\0';
                break;
            }
            if(writed_num >= BLOCK_SIZE) {    //The block has been used up
                int free_block_num = findFreeBlock();
                if(free_block_num == -1) {
                    cout << "Storage has been used up." << endl;
                    return -1;
                }
                m_FAT[current_block_num] = free_block_num;
                current_block_num = free_block_num;
                m_bitmap[current_block_num] = 1;
                m_FAT[0]--;
                writed_num = 0;
                data = (char*)(m_blocks + current_block_num * BLOCK_SIZE);
            }
            data[writed_num++] = c;
            fcb->size++;
        }

    }
    return 0;
}

//Read the file, success return 0, fail return -1
int FileSystem::read(std::string fileName, std::string& content) {
    bool fileExist = false;
    FCB* fcb = NULL;

    for(const auto& block_num : m_current_dir->children) {
        fcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
        if(strcmp(fcb->name, fileName.c_str()) == 0) {   //If the file exist
            fileExist = true;
            break;
        }
    }
    if(fileExist == false)
        return -1;
    char* data = (char*)(m_blocks + fcb->first_block * BLOCK_SIZE);
    int current_block_num = fcb->first_block;
    int writed_num = 0;
    while(data[writed_num] != '\0') {
        content += data[writed_num++];
        if(writed_num >= BLOCK_SIZE) {    //Have finished reading the block
            if(m_FAT[current_block_num] == 0)    //If the current block is the last block
                return 0;
            writed_num = 0;
            current_block_num = m_FAT[current_block_num];
            data = (char*)(m_blocks + current_block_num * BLOCK_SIZE);
        }
    }
    return 0;
}

//Create dir，success return 0, fail return -1
int FileSystem::mkdir(char dirName[15]) {
    FCB* fcb = NULL;
    for(const auto& block_num : m_current_dir->children) {
        fcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
        if(strcmp(fcb->name, dirName) == 0 && fcb->type == DIR)    //If there is a dir has common name, return -1
            return -2;
    }
    int free_block_num = findFreeBlock();
    if(free_block_num == -1)
        return -1;
    m_bitmap[free_block_num] = 1;
    m_FAT[0]--;
    //Init FCB
    FCB* new_dir = (FCB*)(m_blocks + free_block_num * BLOCK_SIZE);
    new_dir->type = DIR;
    strcpy(new_dir->name, dirName);
    new_dir->first_block = -1;
    new_dir->use_count = 0;
    new_dir->parent = m_current_dir->location;
    new_dir->location = free_block_num;
    m_current_dir->children.push_back(free_block_num);
    new_dir->parent = m_current_dir->location;
    return 0;
}

//Delete dir or normal file, success return 0, fail return -1
int FileSystem::deleteFile(char* fileName, vector<int> children_nums, bool all) {
    FCB* fcb = NULL;
    for(const auto& block_num : children_nums) {
        fcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
        if(all || strcmp(fcb->name, fileName) == 0) {
            int next_block_num = -1;
            if(fcb->type == NORMAL) {    //If the FCB type is NORMAL, free FCB and the first block
                m_bitmap[fcb->location] = 0;
                m_FAT[0]++;
                int current_block_num = fcb->first_block;
                while(current_block_num != 0) {
                    next_block_num = m_FAT[current_block_num];
                    m_FAT[current_block_num] = 0;
                    m_bitmap[current_block_num] = 0;
                    m_FAT[0]++;
                    current_block_num = next_block_num;
                }
                FCB* parent = (FCB*)(m_blocks + fcb->parent * BLOCK_SIZE);
                for(auto iter = parent->children.begin(); iter != parent->children.end(); ++iter)
                    if(*iter == fcb->location) {
                        parent->children.erase(iter);
                        break;
                    }
            }
            else if(fcb->type == DIR) {   //If the FCB type is dir, check if the dir is empty, and free FCB
                if(fcb->location == 0)
                    return -1;

                FCB* child_fcb = NULL;
                for(const auto& children_block_num : fcb->children) {
                    child_fcb = (FCB*)(m_blocks + children_block_num * BLOCK_SIZE);
                    deleteFile(child_fcb->name, fcb->children, true);
                }
                m_bitmap[fcb->location] = 0;
                m_FAT[0]++;
                FCB* parent = (FCB*)(m_blocks + fcb->parent * BLOCK_SIZE);
                for(auto iter = parent->children.begin(); iter != parent->children.end(); ++iter)
                    if(*iter == fcb->location) {
                        parent->children.erase(iter);
                        break;
                    }
            }

        }
    }
    cout << "delete : " << fileName << "remain : " << m_FAT[0] << endl;
    for(int i = 0; i < 50; i++)
        cout << m_bitmap[i] << ' ';
    cout << endl;
    return 0;
}


//Return the items in current dir
std::vector<std::vector<std::string> > FileSystem::dir() {
    vector<vector<string> > nameList;
    if(m_current_dir->children.empty()) {
        cout << "The directory is empty." << endl;
        return nameList;
    }
    FCB* fcb = NULL;
    for(const auto& block_num : m_current_dir->children) {
        fcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
        std::string type = "";
        if(fcb->type == NORMAL)
            type = "FILE";
        else if(fcb->type == DIR)
            type = "DIR";
        printf("%-15s%-15s%-15d\n",fcb->name, type.c_str(), fcb->size);
        vector<string> vec;
        vec.push_back(fcb->name);
        vec.push_back(type);
        vec.push_back(std::to_string(fcb->size));
        vec.push_back(fcb->time);
        nameList.push_back(vec);
    }
    return nameList;
}

std::vector<int> FileSystem::getChildren()
{
    return m_current_dir->children;
}

//Change current dir,success return 0, fail return 0
int FileSystem::changeDir(char *dirName) {
    FCB* fcb = NULL;
    if(strcmp(dirName, "..") == 0) {  //back to parent
        if(m_current_dir->location == 0) {
            cout << "It's root now" << endl;
            return 0;
        }
        m_current_dir = (FCB*)(m_blocks + m_current_dir->parent * BLOCK_SIZE);
        return 0;
    }
    for(const auto& block_num : m_current_dir->children) {
        fcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
        if(fcb->type == DIR && strcmp(dirName, fcb->name) == 0) {
            m_current_dir = fcb;
            return 0;
        }
    }
    return -1;
}

//Print the current path
void FileSystem::pwd() {
    cout << m_current_dir->name << endl;
}

int FileSystem::setName(char *newName, char* oldName, FILE_TYPE type) {
    FCB* fcb = NULL;
    FCB* targetFcb = NULL;
    for(const auto& block_num : m_current_dir->children) {
        fcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
        if(strcmp(oldName, fcb->name) == 0 && type == fcb->type) {
            targetFcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
        }
        if(strcmp(newName, fcb->name) == 0 && type == fcb->type)
            return -1;
    }
    strcpy(targetFcb->name, newName);
    return 0;
}

int FileSystem::setName(char *newName, int block_num) {
    FCB* fcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
    if(strcmp(newName, fcb->name) == 0)   //If name doesn't change
        return 0;
    FCB* current_dir = (FCB*)(m_blocks + fcb->parent * BLOCK_SIZE);
    FCB* child_FCB = NULL;
    for(const auto& child_block_num : current_dir->children) {
        child_FCB = (FCB*)(m_blocks + child_block_num * BLOCK_SIZE);
        if(strcmp(child_FCB->name, newName) == 0 && fcb->type == child_FCB->type)     //If the file already exists
            return -1;
    }
    strcpy(fcb->name, newName);
    return 0;
}


//Return the location of file named name in current dir
int FileSystem::getBlockNumByName(char *name)
{
    FCB* fcb = NULL;
    for(const auto& block_num : m_current_dir->children) {
        fcb = (FCB*)(m_blocks + block_num * BLOCK_SIZE);
        if(strcmp(name, fcb->name) == 0)
            return fcb->location;
    }
    return -1;
}

FileSystem::~FileSystem() {
    free(m_blocks);
}
