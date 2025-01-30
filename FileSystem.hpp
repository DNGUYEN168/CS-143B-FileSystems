#ifndef FILESYSTEM_H

#define FILESYSTEM_H

#include "Disk.hpp"
#include <string>
typedef struct
{
    int fileSize;
    int b1;
    int b2;
    int b3;
} fileDescriptors;


typedef struct 
{
    unsigned char buffer[512]; 
    int current_position; // current position inside file 
    int file_size; // size of currently opened file 
    int descriptor_index; // where its index is inside disk 
} oft_entry;

class FileSystem
{

    private:
    Disk* virtualDisk; // disk that will hold all file information 

    oft_entry OFT[4]; // open file table 
    unsigned char M[512]; // main memory ( can be used to help set up the fds)
    unsigned char** localChache;
    // using a psuedo cache to save fds later 
    
    public:
    FileSystem();
    ~FileSystem();

    void create(unsigned char* name);
    unsigned char* destroy(unsigned char* name);
    int open(unsigned char* name);
    int close(int i);
    int read(int i, int m, int n); // get OFT[i], read n bytes from its buffer (moving to next blocks if neccessary), into M starting at m
    int write(int i, int m, int n); // copy n bytes from main memory M[m] to OFT[i]
    int seek(int i, int p);
    void directory();

    // auxiliory function 
    void init();
    int write_memory(int m, unsigned char* s, int bytes);
    std::string read_memory(int m, int n);

    // helpers 
    fileDescriptors getFileDescriptor(int i);
    void UpdateFD(fileDescriptors fd, int fdNum); // given fdIndex update with new info
    int findFreeBlock();

};


#endif