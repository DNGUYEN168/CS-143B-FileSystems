#include "Disk.hpp"
#include <string.h>
#include <iostream>
Disk::Disk()
{
    // initialie the 64x512 array 

    disk = new unsigned char* [64]; // make array of pointer 64 
    for (int i = 0; i <64; i++)
    {
        disk[i] = new unsigned char[512];
    }
}

void Disk::clearDisk()
{
    for (int x = 0; x < 64; x++)
    {
        delete[] disk[x]; // Free the memory pointed to by each pointer in localChache
        disk[x] = nullptr; // Optional: set the pointer to nullptr for safety
    }
    // Step 2: Deallocate the array of pointers itself
    delete[] disk; // Free the array of pointers
    disk = nullptr;
}

void Disk::read_block(int B, unsigned char* input_buffer)
{
   
    memcpy(input_buffer, disk[B], 512); // memcpy data in input onto disk 

}

void Disk::write_block(int B, unsigned char *output_buffer)
{
    memcpy(disk[B], output_buffer, 512); // copy 
}
