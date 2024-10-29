#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include <string>
#include <fstream>
#include <iostream>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Cd {
    void changeDirectory(std::string& path) {
        std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!fs) {
            std::cerr << "Cannot open filesystem" << std::endl;
            return;
        }

        // Read the filesystem description
        Description desc{};
        fs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
        if (!fs) {
            std::cerr << "Can't read filesystem description" << std::endl;
            // Close filesystem
            fs.close();
            return;
        }

        if (path.empty()) {
            currentCluster = ROOT_CLUSTER;
            currentPath = ROOT_DIRECTORY;
        } else {
            // Split the path into directories
            std::pair<std::string, int32_t> result = Utils::splitPath(path);
            if (result.first.empty() && result.second == -1) {
                std::cout << "Path does not exist" << std::endl;
            } else {
                //set result.first to currentPath and result.second to currentCluster
                currentPath = result.first;
                currentCluster = result.second;
            }
        }

        // Close filesystem
        fs.close();
    }
}