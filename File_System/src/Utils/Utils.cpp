#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Items.h"

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

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


std::pair<std::string, int32_t> splitPath(std::string& path) {
    std::string tempPath;
    std::string finalPath = "/";
    int32_t cluster = currentCluster;

    // absolute path
    if (path[0] == '/') {
        tempPath = path.substr(0);
    } else {
        //relative path
        tempPath = currentPath;
        if (tempPath[tempPath.size() - 1] != '/') {
            tempPath += "/";
        }

        std::istringstream iss(path);
        std::string dir;
        while (std::getline(iss, dir, '/')) {
            if (dir.empty()) continue;

            if (dir == "..") {
                tempPath = tempPath.substr(0, tempPath.size() - 1);
                tempPath = tempPath.substr(0, tempPath.find_last_of('/'));
            } else {
                tempPath += dir + "/";
            }
        }
    }

    // Read the filesystem description
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }
    Description desc{};
    ifs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
    if (!ifs) {
        std::cerr << "Error: Could not read filesystem description" << std::endl;
        exit(1);
    }

    DirectoryItem dirItem{};
    std::istringstream iss(tempPath);
    std::string dir;
    cluster = 0; // Start from the root cluster
    while (std::getline(iss, dir, '/')) {
        if (dir.empty()) continue;

        ifs.seekg(desc.data_start_address, std::ios::beg);
        bool found = false;
        while (ifs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
            if (dirItem.name == dir && !dirItem.isFile && dirItem.parent_cluster == cluster) {
                cluster = dirItem.start_cluster; // Update cluster
                finalPath += dir + "/"; // Update finalPath
                found = true;
                break;
            }
        }
        if (!found) {
            return {"", -1};
        }
    }
    return {finalPath, cluster};
}

} // namespace Utils