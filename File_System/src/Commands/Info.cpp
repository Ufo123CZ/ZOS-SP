#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include "../Utils/FAT.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <regex>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Info {
    /**
     * @brief Display information about a file or directory
     * @param path - path to the file or directory
     * @return - message with information about the file or directory
     */
    std::string fileInfo(std::string& path) {
        // Open the filesystem
        std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!fs) {
            return "Cannot open filesystem";
        }

        // Read the filesystem description
        Description desc{};
        fs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
        if (!fs) {
            // Close file
            fs.close();
            return "Can't read filesystem description";
        }

        // Item name to search
        std::string itemName = path.substr(path.find_last_of('/') + 1);

        // Regular expression to match suffix of any kind of file
        // if there is no suffix, it will be a folder
        std::regex suffixRegex("\\.[a-zA-Z0-9]+");
        std::smatch match;
        std::regex_search(itemName, match, suffixRegex);
        bool isFile = !match.empty();

        // Traverse the directory structure to find the directory item
        int32_t cluster;
        std::pair<std::string, int32_t> result;
        if (isFile) {
            std::string pathWithoutItem = path.substr(0, path.find_last_of('/'));
            if (pathWithoutItem == itemName) {
                result = {currentPath, currentCluster};
            } else {
                result = Utils::splitPath(pathWithoutItem);
            }
        } else {
            result = Utils::splitPath(path);
        }
        if (result.first.empty() && result.second == -1) {
            fs.close();
            return "Path does not exist";
        }
        cluster = result.second;

        // If item is a File find it first
        if (isFile) {
            fs.seekg(desc.data_start_address + cluster * desc.cluster_size, std::ios::beg);
            DirectoryItem dirItem{};
            bool found = false;
            for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
                fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
                if (!fs) {
                    fs.close();
                    return "Error: Could not read directory items";
                }
                if (dirItem.name == itemName && dirItem.isFile) {
                    found = true;
                    cluster = dirItem.start_cluster;
                    break;
                }
            }
            if (!found) {
                fs.close();
                return "File does not exist";
            }
        }

        // Initialize FAT
        FAT fat;
        fat.readFromFile(filename);

        // Read the cluster chain of the file
        std::vector<int32_t> clusters = fat.getClusterChain(cluster);

        // Close the filesystem
        fs.close();

        // Print the list of clusters
        std::string resultStr = "File: " + itemName + "\nFile is in cluster: ";
        for (const auto& cl : clusters) {
            resultStr += std::to_string(cl) + " ";
        }

        return resultStr;
   }
}