#include "Commands.h"
#include "../Utils/Items.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#define ROOT_CLUSTER (-2)

namespace Ls {
    std::string listDirectory(std::string& filename, std::string& path) {
        std::fstream fs(filename, std::ios::in | std::ios::binary);
        if (!fs) {
            return "Cannot open filesystem";
        }

        // Read the filesystem description
        Description desc{};
        fs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
        if (!fs) {
            return "Can't read filesystem description";
        }

        // go from root to the end of the path and find the cluster at the end of the path
        int32_t currentCluster = ROOT_CLUSTER;

        // Navigate to the specified directory
        if (!path.empty() && path != "/") {
            std::istringstream iss(path);
            std::string dir;
            while (std::getline(iss, dir, '/')) {
                if (dir.empty()) continue;

                fs.seekg(desc.data_start_address, std::ios::beg);
                DirectoryItem dirItem{};
                bool found = false;
                while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
                    if (dirItem.name == dir && !dirItem.isFile) {
                        currentCluster = dirItem.start_cluster;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return "Directory not found: " + dir;
                }
            }
        }

        // List all items with the same parentCluster
        fs.seekg(desc.data_start_address, std::ios::beg);
        DirectoryItem dirItem{};
        std::string result;
        while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
            if (dirItem.parent_cluster == currentCluster) {
                result += (dirItem.isFile ? "File: " : "Dir: ") + std::string(dirItem.name) + "\n";
            }
        }

        return result.empty() ? "Directory is empty" : result;
    }
}