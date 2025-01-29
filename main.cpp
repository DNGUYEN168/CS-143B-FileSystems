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
    // // std::cout << "calll!" << std::endl;
    FS.create(name2);
    // // std::cout << "calll!" << std::endl;
    FS.create(name3);

    FS.open(name);
    FS.open(name1);
    FS.open(name2);

    // unsigned char tIn[12] = {'a','b','c','d','e','f','g','h','i','j','k','l'};
    // FS.write_memory(0,tIn, sizeof(tIn));
    std::cout << "write call here" << std::endl;
    FS.write(1, 0,512); // 8
    FS.write(2, 0,512); // 9
    FS.write(3, 0,512); // 10

    FS.write(1, 0,512); // 11
    FS.write(1, 0,512); // 12
    FS.write(3, 0,512); // 13
    FS.write(2, 0,512); // 14
    FS.write(2, 0,512); // 15
    FS.write(3, 0,512); // 16


    FS.directory();

    // std::cout << FS.M[0] << FS.M[1] << FS.M[2] << std::endl;

    

    return 0;
}
