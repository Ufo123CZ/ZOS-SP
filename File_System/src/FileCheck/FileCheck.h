#ifndef FILECHECK_H
#define FILECHECK_H

#include <string>

class FileCheck {
public:
    static bool checkArgcSize(int argc, int expectedSize);
    static bool isDatFile(const std::string &filename);
};

#endif //FILECHECK_H
