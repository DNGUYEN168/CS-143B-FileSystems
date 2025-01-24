#include "FileSystem.hpp"
#include "Helpers.hpp"
#include <cstring>
#include <iostream>

FileSystem::FileSystem()
{
    // initailze all required data, and possibly sort disk here in the constructor 
    virtualDisk = new Disk();

    localChache = new unsigned char* [7]; // 1 bitmap block + 6 fd array blocks

    for (int x =0; x < 7; x++) // initialize an empty cache 
    {
        localChache[x] = new unsigned char [512]; 
        std::memset(localChache[x], 0, sizeof(localChache[x]));
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

    // bit map uses at most Disk[0][0-7] (64 bits = 8 bytes) 
    M[0] = 0xFF; // sets the first 8 bits to be 1 ( those blocks are used)     
    
    virtualDisk->write_block(0, M); // wrtie to our bitmap the new bitmap 

    // writing file descriptors into the disk 
    // struct fd = 16 bytes (no padding) 
    // store structs contiguously 

    fileDescriptors DirectoryFD = {0,7,-1,-1}; // only for directory
    fileDescriptors DefaultFD = {-1, -1,- 1, -1}; // normal fd

    for (int i = 1; i < 7; i++) // blocks 1-7
    {
        for (int j=0; j < 512; j += 16) // array 0-512 (16 byte fds)
        {
            if (i == 1 && j == 0) // very first fileDescriptor
            {
                memcpy(&M[j], &DirectoryFD, sizeof(DirectoryFD)); // copy directory into first fd 
            }
            if (i == 3 && j == 272) // test run use numbers 3 and 272
            {   
                fileDescriptors test = {12, -10,- 13, -10}; // normal fd
                memcpy(&M[j], &test, sizeof(test));
            }
            else
            {
                memcpy(&M[j], &DefaultFD, sizeof(DefaultFD)); // copy default for the rest 
            }
        }
 
        virtualDisk->write_block(i, M); // write to disk the new block of fd's
    }
    // initilize the OFT 
    for (int i = 0; i < 4; i++)
    {
        std::memset(OFT[i].buffer, 0, sizeof(OFT[i].buffer)); // OFT buffer = '\0'
        // the rest are -1 
        OFT[i].current_position = 0;
        OFT[i].descriptor_index = 81;
        OFT[i].file_size = 600;
    }

    // OFT[0] is all 0 and holds directory information 
    OFT[0].current_position = 0;
    OFT[0].descriptor_index = 0;
    OFT[0].file_size = 0;
    virtualDisk->read_block(7, OFT[0].buffer); // store directory inside OFT[0]

    for (int i = 0; i < 7; i++) // store 0-6 for quick access 
    {
        virtualDisk->read_block(i, localChache[i]);
    }

    // //Clear M once again (was holding fd info) 
    std::memset(M, 0, sizeof(M));
}

fileDescriptors FileSystem::getFileDescriptor(int i)
{
    // get fd info if we need to get more blocks later on 
    // int fdBlockIndex = (OFT[i].descriptor_index / 32) + 1; 
    // int fdIndex = (OFT[i].descriptor_index % 32) * 16; // starting point of fd at OFT[i] 
    int fdBlockIndex = 3;
    int fdIndex = 272;
    unsigned char* cacheFDBlock = localChache[fdBlockIndex]; 

    fileDescriptors fdData; 
    // copy file descriptors for ease of use later 
    memcpy(&fdData.fileSize, &cacheFDBlock[fdIndex], 4); 
    memcpy(&fdData.b1, &cacheFDBlock[fdIndex+4], 4);         
    memcpy(&fdData.b2, &cacheFDBlock[fdIndex +8], 4);         
    memcpy(&fdData.b3, &cacheFDBlock[fdIndex + 12], 4); 

    return fdData;

}

// The directory has an initial size of 0 and expands in fixed increments of 
// 8 bytes since each new entry consists of 4 characters followed by an integer


int FileSystem::read(int i, int m, int n)
{
    if (i < 0 || i > 3) // not inside OFT range
    {
        throw "Error: Invalid OFT index";
    }
    if (OFT[i].file_size == -1)
    {
        throw "Error: File not opened" ;
    }
    if (n < 0 || n >= 512) // n = 0 - 511 
    {
        throw "Error: Invalid byte size";
    }
    if (m < 0 || m >= 512) {
        throw "Error: Invalid memory index";
    }

    int bufferPosition = OFT[i].current_position % 512;
    // assume 0 - 1535, the buffer inside OFT[i] will be some block, we mod to get the starting point at THAT block     

    fileDescriptors fdData = getFileDescriptor(i); // get file descriptor and store for ease of use 

    for (n; n > 0; n--, m++, OFT[i].current_position++) // decr n (num bytes), inc m (memory position) , inc oft.curr
    {
        if (m > 512) {break;} // not enough room in main memory 
        if (OFT[i].current_position > OFT[i].file_size) {break;} // file has no more data 
        if (OFT[i].current_position == 512) // b1 (0-511) move to b2 if curr_pos goes past 
        {
             //move block 1 back to disk
            virtualDisk->write_block(fdData.b1, OFT[i].buffer);

            // get block 2 for more reading 
            virtualDisk->read_block(fdData.b2, OFT[i].buffer);
        }
        else if (OFT[i].current_position == 1024) // blocks 2 (512 - 1023) move to block 3 if curr_pos goes past b2
        {
            //move block 2 back to disk
            virtualDisk->write_block(fdData.b2, OFT[i].buffer);

            // get block3 for more reading 
            virtualDisk->read_block(fdData.b3, OFT[i].buffer);
        }
        M[m] = OFT[i].buffer[bufferPosition]; // copy data
        bufferPosition = OFT[i].current_position % 512; // make sure it stays wiwhtin 0 - 511
    }

    return n;
    
}

int FileSystem::seek(int i, int p)
{
    // i = index at OFT
    // p = new pos we want to move at 
    if (i < 0 || i > 3) // not in range of OFT
    {
        throw "Error: Invalid OFT index";
    }
    if (OFT[i].file_size == -1)
    {
        throw "Error: File at index not opened";
    }
    if (p < 0 || p > OFT[i].file_size) // desired position is negative, or bigger than current file 
    {
        throw  "error: current position is past the end of file";
    }

    // main seek 
    // pos > 0
    // p and current_pos are in the same block (i.e. divided by 512 equals the same number) 
    // we subtract 1 to avoid is curr_pos = 512 and p = 510, they are the same block but might lead to different numbers 
    if ( OFT[i].current_position > 0 && (OFT[i].current_position -1 / 512 == p / 512) || (OFT[i].current_position / 512 == p / 512))
    {
        OFT[i].current_position = p; 
    }
    else // we need to retrieve the new block and switch it with the old block  
    {
        int fdBlockIndex = (OFT[i].descriptor_index / 32) + 1; // fd 0-191 / 32 = 0 - 5 add 1 to get it 1 - 6
        int fdIndex = (OFT[i].descriptor_index % 32) * 16; // mod 32 to get the index of fd, multiply 16 to get to the correct starting place in array 
         
        // location of OFT[i] file descriptor within the D[1-6] 
        // retrieving file data 
        unsigned char* cacheFDBlock = localChache[fdBlockIndex]; 


    }

    return p;




}