#include "FileSystem.hpp"
#include <cstring>
#include <iostream>

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