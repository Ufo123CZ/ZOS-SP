#include "Commands.h"
#include <iostream>

namespace Help {
    /**
     * @brief Print help menu in console
     */
    void writeHelpInConsole() {
        std::cout << "Help menu: " << std::endl
                << "   cp s1 s2: Copy s1 to s2" << std::endl
                << "   mv s1 s2: Move s1 to s2 or rename s1 to s2" << std::endl
                << "   rm s1: Remove file s1" << std::endl
                << "   mkdir a1: Create directory a1" << std::endl
                << "   rmdir a1: Remove directory a1" << std::endl
                << "   ls a1: List files in directory a1" << std::endl
                << "   ls: List files in current directory" << std::endl
                << "   cat s1: Display contents of file s1" << std::endl
                << "   cd a1: Change directory to a1" << std::endl
                << "   pwd: Display current directory" << std::endl
                << "   info a1/s1: Display information about a1/s1" << std::endl
                << "   incp s1 s2: Copy s1 to s2 in the current directory" << std::endl
                << "   outcp s1 s2: Copy s1 to s2 outside the current directory" << std::endl
                << "   load s1: Load a file into the program" << std::endl
                << "   format 600MB: Format a 600MB file. Size must be in MB. If there are any information in file it will be erased. If file does not exist it will be created" << std::endl
                << "   exit: Exit the program" << std::endl
                << "   help: Display this help menu" << std::endl
                << "   check: Check for bad clusters in the filesystem" << std::endl
                << "   bug: create a bad cluster in FAT on starting cluster of s1" << std::endl;
    }
}
