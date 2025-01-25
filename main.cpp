#include <stdio.h>
#include <iostream>
#include "FileSystem.hpp"

int main()
{
    // Disk* teast= new Disk();
    FileSystem FS = FileSystem();
    unsigned char test[4] = {'W', 'X', 'Y', '\0'};
    FS.create(test);

    return 0;
}
