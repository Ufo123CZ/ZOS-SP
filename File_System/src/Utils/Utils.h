#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace Utils {
    void initializeFilesystem(std::string& filename);
    void loadFilesystem(std::string& filename);
    void endProgram();
    std::pair<std::string, int32_t> splitPath(std::string& path);
}

#endif // UTILS_H