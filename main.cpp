
#include "FileSystem.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <cstring>
#include <string>
int main(int argc, char* argv[])
{
    // Disk* teast= new Disk();
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string s;

    std::ofstream outputFile;
    outputFile.open("output.txt");


    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
        return 1; // Exit with an error code
    }


    unsigned char fileName[4] = {'\0','\0','\0','\0'};
    FileSystem FS;

    if (inputFile.is_open())
    {
        while (getline(inputFile,s))
        {
            if (s.empty() || s.find_first_not_of(" \t\n\r") == std::string::npos) 
            {
                outputFile << '\n';
                continue;
            }
            std::vector<std::string> commands; 
            std::istringstream iss(s); 
            std::string part;
            
            
            // Split the string by spaces and add each part to the vector
            while (iss >> part) {
                commands.push_back(part);
            }
            

            if (commands[0].compare("in") == 0)
            {
                FS = FileSystem();
                outputFile << "system initialized" << std::endl;
            }
            else if (commands[0].compare("cr") == 0)
            {
                try
                {
                    for (int i = 0; i < 4; i++)
                    {
                        fileName[i] = commands[1][i];
                    }
                    outputFile << FS.create(fileName) << " created" << std::endl;
                }
                catch(...)
                {
                    outputFile << "error\n";
                }
            }
            else if (commands[0].compare("de") == 0)
            {
                try
                {
                    for (int i = 0; i < 4; i++)
                    {
                        fileName[i] = commands[1][i];
                    }
                    outputFile << FS.destroy(fileName) << " destroyed" << std::endl;
                }
                catch(...)
                {
                    outputFile << "error\n";
                }
            }
            else if (commands[0].compare("op") == 0)
            {
                try
                {
                    for (int i = 0; i < 4; i++)
                    {
                        fileName[i] = commands[1][i];
                    }
                    int fdNum = FS.open(fileName);
                    outputFile << fileName << " opened " << fdNum << std::endl;
                }
                catch(...)
                {
                    outputFile << "error\n";
                }
            }
            else if (commands[0].compare("cl") == 0)
            {
                try
                {
                    int oftIndex = FS.close(std::stoi(commands[1]));
                    outputFile << oftIndex << " closed" << std::endl;
                }
                catch(...)
                {
                    outputFile << "error\n";
                }
            }
            else if (commands[0].compare("rd") == 0)
            {
                try
                {
                    int numBytes = FS.read(std::stoi(commands[1]),std::stoi(commands[2]),std::stoi(commands[3]));
                    outputFile << numBytes << " bytes read from " << commands[1] << std::endl;
                }
                catch(...)
                {
                    outputFile << "error\n";
                }
            
            }
            else if (commands[0].compare("wr") == 0)
            {
                try
                {
                    int numBytes = FS.write(std::stoi(commands[1]),std::stoi(commands[2]),std::stoi(commands[3]));
                    outputFile << numBytes << " bytes written to " << commands[1] << std::endl;
                }
                catch(...)
                {
                    outputFile << "error\n";
                }
                
            }
            else if (commands[0].compare("sk") == 0)
            {
                try
                {
                    int pos = FS.seek(std::stoi(commands[1]),std::stoi(commands[2]));
                    outputFile << "position is " << pos << std::endl;
                }
                catch(...)
                {
                    outputFile << "error\n";
                }
            }
            else if (commands[0].compare("dr") == 0)
            {
                outputFile << FS.directory();
                outputFile << '\n';
            }
            else if (commands[0].compare("rm") == 0)
            {
                try
                {
                    outputFile << FS.read_memory(std::stoi(commands[1]), std::stoi(commands[2])) << std::endl;
                }
                catch(...)
                {
                    outputFile << "error\n";
                }
                
            }
            else if (commands[0].compare("wm") == 0)
            {
                try
                {
                    unsigned char buffer[512];
                    int size; 
                    for (size = 0; size < commands[2].length(); size++)
                    {
                        buffer[size] = commands[2][size];
                    }
                    buffer[commands[2].length()] = '\0';
                    int totalBytes = FS.write_memory(std::stoi(commands[1]), buffer, size);
                    outputFile << totalBytes << " bytes written to M" << std::endl;

                }
                catch(...)
                {
                    outputFile << "error\n";
                }
                
            }

        }
    }




    FS.quit(); // clear any meme issues 
    return 0;
}
