#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/FAT.h"
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

namespace MkDir {
    std::string makeDirectory(std::string& filename, std::string& dirname) {
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

        // Check if directory already exists
        fs.seekg(desc.data_start_address, std::ios::beg);
        DirectoryItem dirItem{};
        while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
            if (dirItem.name == dirname && !dirItem.isFile) {
                return "Directory already exists";
            }
        }

        // Initialize FAT and read clusters from file
        FAT fat;
        fat.readFromFile(filename);

        // Find the first free cluster
        int freeCluster = fat.findFreeCluster();
        if (freeCluster == -1) {
            return "No free clusters available";
        }

        // Mark the cluster as used
        fat.Clusters[freeCluster] = FAT_FILE_END;
        fat.writeToFile(filename);

        DirectoryItem newDirItem{};
        strncpy(newDirItem.name, dirname.c_str(), ITEM_MAX_NAME - 1);
        newDirItem.name[sizeof(newDirItem.name) - 1] = '\0';
        newDirItem.isFile = false;
        newDirItem.size = 0;
        newDirItem.start_cluster = freeCluster;

        // Write the new directory item to the filesystem
        fs.clear(); // Clear EOF flag
        fs.seekp(0, std::ios::end);
        fs.write(reinterpret_cast<char*>(&newDirItem), sizeof(newDirItem));
        if (!fs) {
            return "Cannot write directory item";
        }

        return "Directory created successfully";
    }
}