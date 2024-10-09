#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/FAT.h"
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

namespace MkDir {
    std::string makeDirectory(std::string& filename, std::string& dirname, int32_t currentCluster) {
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

        // Read existing directory items
        fs.seekg(desc.data_start_address, std::ios::beg);
        std::vector<DirectoryItem> dirItems;
        DirectoryItem dirItem{};
        while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
            dirItems.push_back(dirItem);
        }

        // Check if directory already exists
        for (const auto& item : dirItems) {
            if (item.name == dirname && !item.isFile) {
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
        newDirItem.parent_cluster = currentCluster;

        // Append the new directory item to the list
        dirItems.push_back(newDirItem);

        // Write all directory items back to the file
        fs.clear(); // Clear EOF flag
        fs.seekp(desc.data_start_address, std::ios::beg);
        for (const auto& item : dirItems) {
            fs.write(reinterpret_cast<const char*>(&item), sizeof(item));
        }
        if (!fs) {
            return "Cannot write directory items";
        }

        return "Directory created successfully";
    }
}