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
    /**
     * @brief List all items in the current directory
     * @param path - path to the directory
     * @return - list of items in the directory
     */
    std::string listDirectory(std::string& path) {
        // Open the filesystem
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

        // Check if the path is not empty
        int32_t clusterToRead = currentCluster;
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
        DirectoryItem dirItem{};
        std::string result;
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
            if (!fs) {
                // Close filesystem
                fs.close();
                return "Can't read directory items";
            }
            if (dirItem.parent_cluster == dirItem.start_cluster) { // root directory
                continue;
            }
            if (dirItem.parent_cluster == clusterToRead && dirItem.name[0] != '\0') {
                result += (dirItem.isFile ? "File: " : "Dir: ") + std::string(dirItem.name) + "\n";
            }
        }

        // Close filesystem
        fs.close();

        // Remove the last newline character
        result = result.substr(0, result.size() - 1);

        return result.empty() ? "Directory is empty" : result;
    }
}