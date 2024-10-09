#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/FAT.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

namespace RmDir {
    std::string removeDirectory(std::string& filename, std::string& dirname, int32_t currentCluster) {
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

        // Navigate to the specified directory
        std::istringstream iss(dirname);
        std::string dir;
        int32_t parentCluster = currentCluster;
        while (std::getline(iss, dir, '/')) {
            if (dir.empty()) continue;

            fs.seekg(desc.data_start_address, std::ios::beg);
            DirectoryItem dirItem{};
            bool found = false;
            while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
                if (dirItem.name == dir && !dirItem.isFile) {
                    // parentCluster = currentCluster;
                    currentCluster = dirItem.start_cluster;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return "Directory not found: " + dir;
            }
        }

        // Check if the directory is empty
        fs.seekg(desc.data_start_address, std::ios::beg);
        DirectoryItem dirItem{};
        while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
            if (dirItem.parent_cluster == currentCluster) {
                return "Directory is not empty";
            }
        }

        // Remove the directory entry
        fs.clear();
        fs.seekg(desc.data_start_address, std::ios::beg);
        while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
            if (dirItem.start_cluster == currentCluster && dirItem.parent_cluster == parentCluster) {
                dirItem.name[0] = '\0'; // Mark as deleted
                fs.seekp(-static_cast<int>(sizeof(dirItem)), std::ios::cur);
                fs.write(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
                break;
            }
        }

        // Mark clusters as free in FAT and clear data segment
        FAT fat;
        fat.readFromFile(filename);

        // Mark the cluster as free
        fat.Clusters[currentCluster - 1] = FAT_UNUSED;
        // Find current cluster in data segment
        // fill with zeros
        // fs.seekp(desc.data_start_address + currentCluster * desc.cluster_size, std::ios::beg);
        // for (int i = 0; i < desc.cluster_size; ++i) {
        //     fs.put(0);
        // }

        fat.writeToFile(filename);

        return "Directory removed";
    }
}