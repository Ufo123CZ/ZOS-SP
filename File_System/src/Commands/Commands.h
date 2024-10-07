#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Format {
public:
    static string formatFile(FILE* file, const string &size);
};


#endif //COMMANDS_H
