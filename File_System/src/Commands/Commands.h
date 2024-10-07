#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>

class Format {
public:
    static std::string formatFile(FILE* file, const std::string &size);
};

#endif //COMMANDS_H