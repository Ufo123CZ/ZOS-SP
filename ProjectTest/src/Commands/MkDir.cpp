#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include "../Utils/FAT.h"
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace MkDir {
    /**
     * @brief Create a new directory in the filesystem
     * @param path - path to the new directory
     * @return - message if the directory was created
     */
    std::string makeDirectory(std::string& path) {
        // Open the filesystem
        std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
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

        // New directory name is last directory in path
        std::string dirname = path.substr(path.find_last_of('/') + 1);
        if (dirname.empty()) {
            // Close filesystem
            fs.close();
            return "Invalid directory name";
        }

        // Find the parent cluster
        int32_t newParentCluster = currentCluster;
        if (path.contains('/')) {
            path = path.substr(0, path.find_last_of('/'));
            std::pair<std::string, int32_t> result = Utils::splitPath(path);
            if (result.first.empty() && result.second == -1) {
                // Close filesystem
                fs.close();
                return "Path does not exist";
            }
            newParentCluster = result.second;
        }

        // Read existing directory items
        fs.seekg(desc.data_start_address + newParentCluster * desc.cluster_size, std::ios::beg);
        DirectoryItem dirItems[desc.cluster_size / sizeof(DirectoryItem)];
        DirectoryItem dirItem{};
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.read(reinterpret_cast<char*>(&dirItem), sizeof(DirectoryItem));
            if (!fs) {
                // Close filesystem
                fs.close();
                return "Can't read directory items";
            }
            dirItems[i] = dirItem;
        }

        // Check if the DirItems array is full
        bool isFull = true;
        for (const auto& item : dirItems) {
            if (item.name[0] == '\0' || item.name[0] == '0') {
                isFull = false;
            }
        }
        if (isFull) {
            // Close filesystem
            fs.close();
            return "Directory is full";
        }

        // Check if the directory already exists
        for (const auto& item : dirItems) {
            if (item.parent_cluster == newParentCluster && strcmp(item.name, dirname.c_str()) == 0) {
                // Close filesystem
                fs.close();
                return "Directory already exists";
            }
        }

        // Initialize FAT
        FAT fat;
        fat.readFromFile(filename);

        // Find the first free cluster
        int freeCluster = fat.findFreeCluster();
        if (freeCluster == -1) {
            // Close filesystem
            fs.close();
            return "No free clusters available";
        }

        // Mark the cluster as used
        fat.Clusters[freeCluster] = FAT_FILE_END;
        fat.writeToFile(filename);

        // Create new directory item
        DirectoryItem newDirItem{};
        strncpy(newDirItem.name, dirname.c_str(), ITEM_MAX_NAME - 1);
        newDirItem.name[sizeof(newDirItem.name) - 1] = '\0';
        newDirItem.isFile = false;
        newDirItem.size = 0;
        newDirItem.start_cluster = freeCluster;
        newDirItem.parent_cluster = newParentCluster;

        // find free place in dirItems
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (dirItems[i].name[0] == '\0' || dirItems[i].name[0] == '0') {
                fs.seekp(desc.data_start_address + i * sizeof(DirectoryItem) + newParentCluster * desc.cluster_size, std::ios::beg);
                fs.write(reinterpret_cast<char*>(&newDirItem), sizeof(DirectoryItem));
                if (!fs) {
                    // Close filesystem
                    fs.close();
                    return "Can't write directory item";
                }
                break;
            }
        }

        // Shorten the directory name if it is too long
        if (dirname.size() > ITEM_MAX_NAME) {
            dirname = dirname.substr(0, ITEM_MAX_NAME - 1);
            std::cout << "Directory name is too long. Shortened to: " << dirname << std::endl;
        }

        // Close the filesystem
        fs.close();

        return  "Directory created";
    }
}