#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    void initializeFilesystem(std::string& filename);
    void loadFilesystem(std::string& filename);
    void endProgram();
}

#endif // UTILS_H