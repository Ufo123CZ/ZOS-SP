#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace Utils {
    void initializeFilesystem(std::string& filename);
    void loadFilesystem(std::string& filename);
    void endProgram();
    std::pair<std::string, int> splitPath(std::string& filename, std::string& path, std::string& currentPath, int currentCluster);
}

#endif // UTILS_H