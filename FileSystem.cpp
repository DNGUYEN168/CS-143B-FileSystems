#include "FileSystem.hpp"
#include <cstring>
#include <iostream>

// TEMP HElpesr 
void printFileDescriptor(const unsigned char* buffer) {
    // Create a fileDescriptors struct to hold the extracted data
    fileDescriptors fd;
    
    // Copy the 16 bytes from the buffer into the fileDescriptors struct
    std::memcpy(&fd, buffer, sizeof(fileDescriptors));

    // Print the fields of the file descriptor
    std::cout << "Field 1: " << fd.fileSize << ", ";
    std::cout << "Field 2: " << fd.b1 << ", ";
    std::cout << "Field 3: " << fd.b2 << ", ";
    std::cout << "Field 4: " << fd.b3 << std::endl;
}

void checkContents(unsigned char* M, int size) {
    // Loop through every 16 bytes in M and treat it as a file descriptor
    for (int i = 0; i < size; i += 16) {
        std::cout << "FD at offset " << i/16 << ": ";
        printFileDescriptor(&M[i]);
    }
}

void print_bits(unsigned char M[512]) {
    // Loop through the first 64 bits (8 bytes)
    for (int i = 0; i < 8; i++) {
        unsigned char byte = M[i];
        
        // Loop through each bit in the byte (8 bits)
        for (int j = 0; j < 8; j++) {
            // Extract and print the bit (1 or 0), printing from LSB to MSB
            printf("%d", (byte >> j) & 1);
        }
    }
    printf("\n");
}

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
            else
            {
                memcpy(&M[j], &DefaultFD, sizeof(DefaultFD)); // copy default for the rest 
            }
        }
        virtualDisk->write_block(i, M); // write to disk the new block of fd's
    }


    
    

}
