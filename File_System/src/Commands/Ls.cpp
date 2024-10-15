#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include <string>
#include <fstream>
#include <iostream>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Ls {
    std::string listDirectory(std::string& path) {
        std::fstream fs(filename, std::ios::in | std::ios::binary);
        if (!fs) {
            return "Cannot open filesystem";
        }

        // Read the filesystem description
        Description desc{};
        fs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
        if (!fs) {
            // Close filesystem
            fs.close();
            return "Can't read filesystem description";
        }

        int32_t clusterToRead = currentCluster;
        // Check if the path is not empty
        if (!path.empty()) {
            auto result = Utils::splitPath(path);
            if (result.first.empty() && result.second == -1) {
                // Close filesystem
                fs.close();
                return "Path does not exist";
            }
            clusterToRead = result.second;
        }

        // List all items with the same parentCluster
        fs.seekg(desc.data_start_address + clusterToRead * desc.cluster_size, std::ios::beg);
        DirectoryItem dirItems[desc.cluster_size / sizeof(DirectoryItem)];
        DirectoryItem dirItem{};
        std::string result;
        while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
            if (dirItem.parent_cluster == dirItem.start_cluster) { // root directory
                continue;
            }
            if (dirItem.parent_cluster == clusterToRead) {
                result += (dirItem.isFile ? "File: " : "Dir: ") + std::string(dirItem.name) + "\n";
            }
        }

        // Close filesystem
        fs.close();

        return result.empty() ? "Directory is empty" : result;
    }
}