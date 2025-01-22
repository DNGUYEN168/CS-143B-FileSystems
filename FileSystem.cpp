#include "FileSystem.hpp"
#include <cstring>
#include <iostream>

FileSystem::FileSystem()
{
    // initailze all required data, and possibly sort disk here in the constructor 
    virtualDisk = new Disk();

    localChache = new unsigned char* [6]; // 6 fd arrays


    for (int x =0; x < 6; x++) // initialize an empty cache 
    {
        localChache[x] = new unsigned char [512]; 
        for (int y = 0; y < 512; y++) // fill the rest with '\0'
        {
            localChache[x][y] = '\0';
        }
    }

    init(); // call to init to start, will be called mutliple in shell 



}

void FileSystem::init()
{
    std::memset(M, 0, sizeof(M)); // initailze main memory to be all empty 
    std::memset(I, 0, sizeof(I)); // initialize input_buffer
    std::memset(O, 0, sizeof(O)); // initialize output_buffer 

    for (int block = 0; block < 64; block++)
    {
        virtualDisk->write_block(block, M); // initalize disk to be empty 
    }

    // bit map: 00000 . . . 000 11111111 <-- first 8 bits are 1 because they take up the bitmap, fds, directory 
    // D[0] = bitmap
    // D[7] = directory 

    M[0] = 0xFF; // sets the first 8 bits to be 1 

    // 
    virtualDisk->write_block(0, M); // wrtie to our bitmap the new bitmap 



}
