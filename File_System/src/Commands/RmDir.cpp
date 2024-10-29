#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/FAT.h"
#include "../Utils/Utils.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace RmDir {
    std::string removeDirectory(std::string& path) {
        // Open the filesystem
        std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!fs) {
            return "Cannot open filesystem";
        }

        // Read the filesystem description
        Description desc{};
        fs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
        if (!fs) {
            return "Can't read filesystem description";
        }

        // Directory name to search
        std::string dirname = path.substr(path.find_last_of('/') + 1);

        // Navigate to the specified directory
        std::pair<std::string, int32_t> result = Utils::splitPath(path);
        if (result.first.empty() && result.second == -1) {
            fs.close();
            return "Path does not exist";
        }
        int32_t cluster = result.second;

        // Fill the directory items with the content of the directory
        DirectoryItem dirItems[desc.cluster_size / sizeof(DirectoryItem)];
        fs.seekg(desc.data_start_address + cluster * desc.cluster_size, std::ios::beg);
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.read(reinterpret_cast<char*>(&dirItems[i]), sizeof(DirectoryItem));
        }

        // Check if the directory is empty
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (dirItems[i].name[0] != '\0') {
                fs.close();
                return "Directory is not empty";
            }
        }

        // Remove the directoryItem that references the directory from the parent directory
        std::string parentPath = path.substr(0, path.find_last_of('/'));
        int32_t parentCluster;
        if (parentPath == path) {
            parentCluster = currentCluster;
        } else {
            std::pair<std::string, int32_t> parent = Utils::splitPath(parentPath);
            if (parent.first.empty() && parent.second == -1) {
                fs.close();
                return "Parent directory does not exist";
            }
            parentCluster = parent.second;
        }

        // Fill the parent directory items with the content of the parent directory
        DirectoryItem parentDirItems[desc.cluster_size / sizeof(DirectoryItem)];
        fs.seekg(desc.data_start_address + parentCluster * desc.cluster_size, std::ios::beg);
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.read(reinterpret_cast<char*>(&parentDirItems[i]), sizeof(DirectoryItem));
        }
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (dirname == parentDirItems[i].name) {
                parentDirItems[i].name[0] = '\0';
                fs.seekp(desc.data_start_address + parentCluster * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
                fs.write(parentDirItems[i].name, sizeof(DirectoryItem));
                break;
            }
        }

        // Initialize the FAT
        FAT fat;
        fat.readFromFile(filename);
        // Mark the clusters as unused
        fat.Clusters[cluster] = FAT_UNUSED;
        fat.writeToFile(filename);

        // Close the filesystem
        fs.close();

        return "Directory removed";
    }
}