#include "UtilF.h"

void UtilF::writeHeplInConsole() {
       cout << "Help menu" << endl
               << "   exit: Exit the program" << endl
               << "   help: Display this help menu" << endl
               << "   cp s1 s2: Copy s1 to s2" << endl
               << "   mv s1 s2: Move s1 to s2 or rename s1 to s2" << endl
               << "   rm s1: Remove file s1" << endl
               << "   mkdir a1: Create directory a1" << endl
               << "   rmdir a1: Remove directory a1" << endl
               << "   ls a1: List files in directory a1" << endl
               << "   ls: List files in current directory" << endl
               << "   cat s1: Display contents of file s1" << endl
               << "   cd a1: Change directory to a1" << endl
               << "   pwd: Display current directory" << endl
               << "   info a1/s1: Display information about a1/s1" << endl
               << "   incp s1 s2: Copy s1 to s2 in the current directory" << endl
               << "   outcp s1 s2: Copy s1 to s2 outside the current directory" << endl
               << "   load s1: Load a file into the program" << endl
               << "   format 600MB: Format a 600MB file. If there are any information in file it will be erased. If file does not exist it will be created" << endl;
}
