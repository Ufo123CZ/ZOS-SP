#include "Utils.h"
#include "Items.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Utils {
    /**
     * @brief Initialize the filesystem
     * @param path - path to the filesystem
     * @result - pair of the final path and the cluster
     */
    std::pair<std::string, int32_t> splitPath(std::string& path) {
        std::string tempPath;
        std::string finalPath = "/";
        int32_t cluster;

        // Absolute path
        if (path[0] == '/') {
            tempPath = path.substr(0);
        } else {
            // Relative path
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

        // Read the directory items from the cluster into the dirItems array
        DirectoryItem dirItem{};
        std::istringstream iss(tempPath);
        std::string dir;
        cluster = 0; // Start from the root cluster
        while (std::getline(iss, dir, '/')) {
            if (dir.empty()) continue;

            // Read the directory items from the cluster into the dirItems array
            ifs.seekg(desc.data_start_address + cluster * desc.cluster_size, std::ios::beg);
            DirectoryItem dirItems[desc.cluster_size / sizeof(DirectoryItem)];
            for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
                ifs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
                if (!ifs) {
                    std::cerr << "Error: Could not read directory items" << std::endl;
                    return {"", -1};
                }
                dirItems[i] = dirItem;
            }

            // Find the directory in the directory items
            bool found = false;
            for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
                if (dirItems[i].name == dir && !dirItems[i].isFile && dirItems[i].parent_cluster == cluster) {
                    cluster = dirItems[i].start_cluster; // Update cluster
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