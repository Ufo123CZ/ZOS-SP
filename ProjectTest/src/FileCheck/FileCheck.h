#ifndef FILECHECK_H
#define FILECHECK_H

#include <string>

namespace FileCheck {
    bool checkArgcSize(int argc, int expectedSize);
    bool isDatFile(const std::string &filename);
}

#endif //FILECHECK_H