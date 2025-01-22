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




class FileSystem
{

    private:
    Disk* virtualDisk; // disk that will hold all file information 

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
    void seek();
    void directory();

    void quit(); // end program

    // auxiliory function 
    void init();

};


#endif