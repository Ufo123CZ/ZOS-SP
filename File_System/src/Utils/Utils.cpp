#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Items.h"

namespace Utils {

void endProgram() {
    std::cout << "Exiting..." << std::endl;
    exit(0);
}

void loadFilesystem(std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }
}


    std::pair<std::string, int> splitPath(std::string& filename, std::string& path, std::string& currentPath, int currentCluster) {
    std::vector<std::string> directories;
    std::string tempPath;
    int cluster = currentCluster;

    // Open the file
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return {"", -1};
    }

    // Read the filesystem description
    Description desc{};
    ifs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
    if (!ifs) {
        std::cerr << "Error: Could not read filesystem description" << std::endl;
        return {"", -1};
    }

    // Split the path into directories
    std::istringstream pathStream(path);
    std::string dir;
    while (std::getline(pathStream, dir, '/')) {
        if (!dir.empty()) {
            directories.push_back(dir);
        }
    }

    // Determine if the path is absolute or relative
    if (path[0] == '/') {
        tempPath = "/";
        cluster = 0; // Start from root
    } else {
        tempPath = currentPath;
    }

    // Traverse the directories
    for (const auto& directory : directories) {
        bool found = false;
        ifs.seekg(desc.data_start_address, std::ios::beg);
        DirectoryItem dirItem{};
        while (ifs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
            if (dirItem.name == directory && !dirItem.isFile && dirItem.parent_cluster == cluster) {
                tempPath += (tempPath == "/" ? "" : std::string("/")) + dirItem.name;
                cluster = dirItem.start_cluster;
                found = true;
                break;
            }
        }
        if (!found) {
            return {"", -1};
        }
    }

    return {tempPath, cluster};
}

} // namespace Utils