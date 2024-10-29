#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include "../Utils/FAT.h"
#include <fstream>
#include <string.h>
#include <string>
#include <vector>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Cp {
    /**
     * @brief Copy a file from the source to the destination in the filesystem
     * @param source - path to the source file
     * @param dest - path to the destination file
     * @return - message if the file was copied
     */
    std::string copyFile(std::string& source, std::string& dest) {
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

        std::string sPath = source.substr(0, source.find_last_of('/'));
        std::string sName = source.substr(source.find_last_of('/') + 1);

        int32_t sourceDirCluster;
        if (sPath == sName) {
            sPath = currentPath;
            sourceDirCluster = currentCluster;
        } else {
            // Find the source file and its cluster
            std::pair<std::string, int32_t> result = Utils::splitPath(sPath);
            if (result.second == -1 && result.first.empty()) {
                return "Error: Cannot find source file";
            }
            sourceDirCluster = result.second;
        }

        // Find the source file in the directory
        DirectoryItem dirItems[desc.cluster_size / sizeof(DirectoryItem)];
        DirectoryItem dirItem{};
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.seekg(desc.data_start_address + sourceDirCluster * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
            fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
            dirItems[i] = dirItem;
        }

        // Find the source file in the directory items
        bool found = false;
        DirectoryItem copyItem{};
        for (const auto& item : dirItems) {
            if (std::string(item.name) == sName && item.isFile) {
                found = true;
                copyItem = item;
                break;
            }
        }
        if (!found) {
            return "Error: Cannot find file";
        }

        // Initialize the FAT
        FAT fat;
        fat.readFromFile(filename);

        // Find all the clusters of the source file
        std::vector<int32_t> sourceClusters = fat.getClusterChain(copyItem.start_cluster);

        // Check if there is enough free clusters
        int32_t freeClusterCount = 0;
        for (int i = 0; i < fat.clusterCount; ++i) {
            if (fat.Clusters[i] == FAT_UNUSED) {
                ++freeClusterCount;
            }
        }
        if (freeClusterCount < sourceClusters.size()) {
            return "Error: Not enough free clusters";
        }

        std::string dPath = dest.substr(0, dest.find_last_of('/'));
        std::string dName = dest.substr(dest.find_last_of('/') + 1);

        int32_t destDirCluster;
        if (dPath == dName) {
            dPath = currentPath;
            destDirCluster = currentCluster;
        } else {
            // Find the destination directory
            std::pair<std::string, int32_t> result = Utils::splitPath(dPath);
            if (result.second == -1 && result.first.empty()) {
                return "Error: Cannot find destination directory";
            }
            destDirCluster = result.second;
        }

        // Find all items in the destination directory
        fs.seekg(desc.data_start_address + destDirCluster * desc.cluster_size, std::ios::beg);
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.seekg(desc.data_start_address + destDirCluster * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
            fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
            dirItems[i] = dirItem;
        }

        if (dName.size() > sizeof(copyItem.name)) {
            return "Error: Destination file name is too long";
        }
        // Clear copyItem.name
        memset(copyItem.name, 0, sizeof(copyItem.name));
        strncpy(copyItem.name, dName.c_str(), sizeof(copyItem.name));

        // Check if the destination file already exists
        for (const auto& item : dirItems) {
            if (std::string(item.name) == copyItem.name) {
                return "File already exists in the destination directory";
            }
        }

        // Check if the destination directory has enough space
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

        // Find a free starting cluster
        int32_t freeCluster = fat.findFreeCluster();

        // Write the copy item to the destination directory
        DirectoryItem copyItemDest = copyItem;
        copyItemDest.start_cluster = freeCluster;
        copyItemDest.parent_cluster = destDirCluster;
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (dirItems[i].name[0] == '\0' || dirItems[i].name[0] == '0') {
                fs.seekp(desc.data_start_address + destDirCluster * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
                fs.write(reinterpret_cast<const char*>(&copyItemDest), sizeof(copyItemDest));
                break;
            }
        }

        // Find new clusters for the source file and fill the free clusters with the source file data
        int32_t previousCluster = -1;
        for (int i = 0; i < sourceClusters.size(); ++i) {
            if (i != 0) {
                freeCluster = fat.findFreeCluster();
            }
            if (freeCluster == -1) {
                // Close filesystem
                fs.close();
                return "No free clusters in the filesystem";
            }
            // Update the FAT
            if (i == sourceClusters.size() - 1) {
                fat.Clusters[freeCluster] = FAT_FILE_END;
            } else {
                fat.Clusters[freeCluster] = fat.findFreeCluster();
            }
            // Link the previous cluster to the current one
            if (previousCluster != -1) {
                fat.Clusters[previousCluster] = freeCluster;
            }
            previousCluster = freeCluster;

            // Copy the data from the sourceClusters[i] to buffer and write it to the freeCluster
            char buffer[desc.cluster_size];
            fs.seekg(desc.data_start_address + sourceClusters[i] * desc.cluster_size, std::ios::beg);
            fs.read(buffer, desc.cluster_size);

            // Paste the data from buffer to the freeCluster
            fs.seekp(desc.data_start_address + freeCluster * desc.cluster_size, std::ios::beg);
            fs.write(buffer, desc.cluster_size);
        }
        fat.writeToFile(filename);

        // Close filesystem
        fs.close();

        return "File copied successfully";
    }
}