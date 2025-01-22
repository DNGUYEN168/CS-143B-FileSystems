#ifndef DISK_H
#define DISK_H

class Disk
{
private:
    unsigned char **disk; // var name for disk, holds 64x512 array 
    
public:

    Disk(); // initializser for disk 

    void read_block(int B, unsigned char* input_buffer); // copies block disk[B] into input_buffer 
    void write_block(int B, unsigned char* output_buffer); // copies output_buffer to disk[B] 
   
};







#endif