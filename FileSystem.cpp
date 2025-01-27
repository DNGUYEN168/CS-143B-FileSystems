#include "FileSystem.hpp"
#include "Helpers.hpp"
#include <cstring>
#include <iostream>


fileDescriptors FileSystem::getFileDescriptor(int i)
{
    // get fd info if we need to get more blocks later on 
    int fdBlockIndex = (OFT[i].descriptor_index / 32) + 1; 
    int fdIndex = (OFT[i].descriptor_index % 32) * 16; // starting point of fd at OFT[i] 
    unsigned char* cacheFDBlock = localChache[fdBlockIndex]; 

    fileDescriptors fdData; 
    // copy file descriptors for ease of use later 
    memcpy(&fdData.fileSize, &cacheFDBlock[fdIndex], 4); 
    memcpy(&fdData.b1, &cacheFDBlock[fdIndex+4], 4);         
    memcpy(&fdData.b2, &cacheFDBlock[fdIndex +8], 4);         
    memcpy(&fdData.b3, &cacheFDBlock[fdIndex + 12], 4); 
    return fdData;
}

void FileSystem::UpdateFD(fileDescriptors fd, int fdNum)
{
    // get fd info if we need to get more blocks later on 
    int fdBlockIndex = (fdNum / 32) + 1; 
    int fdIndex = (fdNum % 32) * 16; // starting point of fd at OFT[i] 
    unsigned char* cacheFDBlock = localChache[fdBlockIndex]; 

    // copy file descriptors for ease of use later 
    memcpy(&cacheFDBlock[fdIndex], &fd.fileSize,  4); 
    memcpy(&cacheFDBlock[fdIndex+4], &fd.b1,  4);         
    memcpy(&cacheFDBlock[fdIndex +8], &fd.b2,  4);         
    memcpy(&cacheFDBlock[fdIndex + 12], &fd.b3,  4); 

}

FileSystem::FileSystem()
{
    // initailze all required data, and possibly sort disk here in the constructor 
    virtualDisk = new Disk();

    localChache = new unsigned char* [7]; // 1 bitmap block + 6 fd array blocks

    for (int x =0; x < 7; x++) // initialize an empty cache 
    {
        localChache[x] = new unsigned char [512]; 
        std::memset(localChache[x], 0, 512);
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

    // TEST SECTION 

    // unsigned char test[4] = {'W', 'X', 'Y', '\0'};
    // int fdIndex = 61;
    // memcpy(&M[0], &test, sizeof(test));
    // memcpy(&M[4], &fdIndex, sizeof(fdIndex));
    // virtualDisk->write_block(7, M); // write to direcoty 
    // std::memset(M, 0, sizeof(M));

    // END OF TEST SECTION 


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
    // initilize the OFT 
    for (int i = 0; i < 4; i++)
    {
        std::memset(OFT[i].buffer, 0, sizeof(OFT[i].buffer)); // OFT buffer = '\0'
        // the rest are -1 
        OFT[i].current_position = -1;
        OFT[i].descriptor_index = -1;
        OFT[i].file_size = -1;
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

int FileSystem::write_memory(int m, unsigned char *s)
{
    int S_len = strlen((char*)s) + m; // total legnth 
    for (int i=m; i < S_len; i++)
    {
        M[i] = s[i];
    }
    return S_len;
}

std::string FileSystem::read_memory(int m, int n)
{
    std::string retVal(n, '\0');
    for (int i=0; i< n; i++)
    {
        retVal[i] = M[m+i]; // copy info
        if (M[i] == '\0') {break;}
    }
    return retVal;
}

// The directory has an initial size of 0 and expands in fixed increments of 
// 8 bytes since each new entry consists of 4 characters followed by an integer
void FileSystem::create(unsigned char *name)
{
    seek(0,0);
    unsigned char test[8] = {'\0', '\0','\0','\0','\0', '\0','\0','\0'};
    // read to main memory starting at curr_pos 0 in directory 
    while (OFT[0].current_position < OFT[0].file_size)
    {
        read(0,0,8); // read 8 bytes store into M --> move curr_pos + 8 
        std::cout << "M: "<<M[0] << M[1] << M[2] << std::endl; 
        std::cout << "name: "<< name[0] << name[1] << name[2] << std::endl;

        if (name[0] == M[0] && name[1] == M[1] && name[2] == M[2] && name[3] == M[3]) {throw "Error: Duplicate name"; return;} // name exists 
    }

    int freeFD = -1;
    for (int i =1; i < 7; i++) // 1-6 fd
    {
        unsigned char* currBlock = localChache[i];
        for (int j = 0; j < 512; j += 16) // 0 - 511
        {
            fileDescriptors fd;
            // Copy the 16 bytes from the buffer into the fileDescriptors struct
            std::memcpy(&fd, &currBlock[j], 16); // copy 16 bytes (4 ints)
            if (fd.fileSize == -1) // free filedesc
            {
                
                freeFD = ((i - 1) * 32) + (j / 16); // convert i and j into an index for later storage 

                fileDescriptors newFD = {0,-1,-1,-1};
                UpdateFD(newFD, freeFD); // update the freeFD with 0 
                break;
            }
        }

        if (freeFD != -1) {break;} // leave double for loop 
    }

    if (freeFD == -1) { throw "Error not space";} // direcotyr full 

    memcpy(test, name, 4); // copy name onto test 
    memcpy(test+4, &freeFD, 4); 
    
    seek(0,0); // start at beginning of directory

    if (OFT[0].current_position == OFT[0].file_size) // dir is empty 
    {
        std::cout << "first call " << name << " to " << OFT[0].current_position << " using FD "<< freeFD << std::endl;
        memcpy(&M[OFT[0].current_position], test, 8);
        OFT[0].file_size += 8;
        fileDescriptors newFD = {OFT[0].file_size,7,-1,-1};
        UpdateFD(newFD,0); // update fd 0 ( directory fd) 
    }
    else 
    {
        while(OFT[0].current_position < OFT[0].file_size)
        {
            read(0,0,8); // read 8 bytes --> move curr_pos + 8 
            if ('\0' == M[0] && '\0' == M[1] && '\0' == M[2] && '\0' == M[3]) 
            {
                // std::cout << name << " to " << OFT[0].current_position << " using FD "<< freeFD << std::endl;
                memcpy(&M[OFT[0].current_position - 8], test, 8); // move back 8 to make space for name + int

                OFT[0].file_size += 8;
                fileDescriptors newFD = {OFT[0].file_size,7,-1,-1};
                UpdateFD(newFD,0); // update fd 0 ( directory fd) 
                break;
            }
        }

    }
    
    // std::cout << FS.M[0] << FS.M[1] << FS.M[2] << std::endl;
    if (OFT[0].current_position == 1536)
    {
        throw "Error: No free directory entry found";
    }

}

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

    int bufferPosition;
    // assume 0 - 1535, the buffer inside OFT[i] will be some block, we mod to get the starting point at THAT block     
    fileDescriptors fdData = getFileDescriptor(i); // get file descriptor and store for ease of use 
    
    for (n; n > 0; n--, m++, OFT[i].current_position++) // decr n (num bytes), inc m (memory position) , inc oft.curr
    {
        bufferPosition = OFT[i].current_position % 512; // make sure it stays wiwhtin 0 - 511
        if (m >= 512) {break;} // not enough room in main memory 
        if (OFT[i].current_position > OFT[i].file_size) {break;} // file has no more data (excluding dir)
        
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
        fileDescriptors fdData = getFileDescriptor(i); // get file data; 

        int currBlock;
        if (OFT[i].current_position < 512) { currBlock = fdData.b1;} // pos < 512 we are in block1 (0 - 511)
        else if (OFT[i].current_position < 1024) { currBlock = fdData.b2;} // 512 - 1023
        else {currBlock = fdData.b3;} // 1024 - 1535

        int new_block;
        if (p < 512) { new_block = fdData.b1;} // in b1
        else if (p < 1024) { new_block = fdData.b2;}  //b2
        else {new_block = fdData.b3;} //b3

        // return old block
        virtualDisk->write_block(currBlock, OFT[i].buffer);
        // get new block
        virtualDisk->read_block(new_block, OFT[i].buffer);
        // update position 
        OFT[i].current_position = p;
    }
    return p;
}