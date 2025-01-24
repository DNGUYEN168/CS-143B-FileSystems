#include "FileSystem.hpp"
#include "Helpers.hpp"
#include <cstring>
#include <iostream>


void printFileDescriptor(const unsigned char* buffer) {
    // Create a fileDescriptors struct to hold the extracted data
    fileDescriptors fd;
    
    // Copy the 16 bytes from the buffer into the fileDescriptors struct
    std::memcpy(&fd, buffer, sizeof(fileDescriptors));

    // Print the fields of the file descriptor
    std::cout << "{" << fd.fileSize << ", ";
    std::cout <<  fd.b1 << ", ";
    std::cout <<  fd.b2 << ", ";
    std::cout <<  fd.b3 << "}" << std::endl;
}

void checkContents(unsigned char* M, int size) {
    // Loop through every 16 bytes in M and treat it as a file descriptor
    for (int i = 0; i < 512; i += 16) {
        std::cout << "offset "<< i << " FD " << i/16 + size << ": ";
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