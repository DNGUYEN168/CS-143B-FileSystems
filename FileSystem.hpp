#ifndef FILESYSTEM_H

#define FILESYSTEM_H

#include "Disk.hpp"

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

    unsigned char* I[512]; // input_buffer
    unsigned char* O[512]; // output_buffer
    unsigned char M[512]; // main memory ( can be used to help set up the fds)


    // using a psuedo cache to save fds later 
    unsigned char** localChache;


    public:
    FileSystem();

    void create();
    void destroy();
    void open();
    void close();
    void read();
    void write();
    int seek(int i, int p);
    void directory();

    void quit(); // end program

    // auxiliory function 
    void init();

};


#endif