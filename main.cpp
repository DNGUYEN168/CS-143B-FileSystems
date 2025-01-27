#include <stdio.h>
#include <iostream>
#include "FileSystem.hpp"

int main()
{
    // Disk* teast= new Disk();
    FileSystem FS = FileSystem();
    unsigned char test[4] = {'W', 'X', 'Y', '\0'};
    unsigned char name[4] =  {'Z', 'X', 'Y', '\0'};
    
    FS.create(test);
    FS.create(test);

    // std::cout << FS.M[0] << FS.M[1] << FS.M[2] << std::endl;

    

    return 0;
}
