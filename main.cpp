#include <stdio.h>
#include <iostream>
#include "FileSystem.hpp"

int main()
{
    // Disk* teast= new Disk();
    FileSystem FS = FileSystem();
    unsigned char name[4] = {'W', 'X', 'Y', '\0'};
    unsigned char name1[4] = {'P', 'Z', 'W', '\0'};
    unsigned char name2[4] = {'X', 'X', 'Y', '\0'};
    unsigned char name3[4] = {'F', 'K', 'W', '\0'};

    FS.create(name);
    // std::cout << "calll!" << std::endl;
    FS.create(name1);
    // std::cout << "calll!" << std::endl;
    FS.create(name2);
    // std::cout << "calll!" << std::endl;
    FS.create(name3);
    

    FS.directory();

    // std::cout << FS.M[0] << FS.M[1] << FS.M[2] << std::endl;

    

    return 0;
}
