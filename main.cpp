#include <stdio.h>
#include <iostream>
#include "FileSystem.hpp"

int main()
{
    // Disk* teast= new Disk();
    FileSystem FS = FileSystem();
    int test = 81;
    int currBlock = (test / 32) + 1; // fd 0-191 / 32 = 0 - 5 add 1 to get it 1 - 6
    int fdIndex = (test % 32) * 16; // mod 32 to get the index of fd, multiply 16 to get to the correct starting place in array 
    std::cout << currBlock << " " << fdIndex << std::endl;

    return 0;
}
