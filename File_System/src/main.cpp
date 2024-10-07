#include <iostream>
#include <string>

#include "FileCheck/FileCheck.h"

using namespace std;

int main(const int argc, char* argv[]) {

    // Check if the number of arguments is correct
    if (!FileCheck::checkArgcSize(argc, 2)) {
        cout << "Need to provide a file" << endl;
        return 1;
    }

    // Check if the file is a .dat file
    if (!FileCheck::isDatFile(argv[1])) {
        cout << "File must have .dat extension" << endl;
        return 1;
    }
    cout << "File is a .dat file" << endl;


    return 0;
}
