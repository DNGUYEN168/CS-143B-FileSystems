#include <stdio.h>
#include <iostream>
#include "FileSystem.hpp"

int main()
{
    // Disk* teast= new Disk();
    FileSystem FS = FileSystem();
    FS.read(0,100,50);

    return 0;
}
