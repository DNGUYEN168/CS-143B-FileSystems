#include <stdio.h>
#include <iostream>
#include "FileSystem.hpp"
#include "Helpers.hpp"
int main()
{
    // Disk* teast= new Disk();
    FileSystem FS = FileSystem();
    unsigned char name[4] = {'\0','\0','\0','\0'};
    int num = 2;
    int num1 = 31;
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j < num1; j++)
        {
            name[0] = 'A' + i;  // Convert `i` into a printable character
            name[1] = 'B' + j;  // Convert `j` into a printable character
            name[2] = 'C' + i;
            FS.create(name);
        }
    }

    FS.directory();
    return 0;
}
