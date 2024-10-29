#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include "../Utils/FAT.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Incp {
    /**
     * @brief Copy a file from the outside of filesystem into the filesystem
     * @param source - path to the source file outside filesystem
     * @param dest - path to the destination in the filesystem
     * @return - message if the file was copied
     */
    std::string copyFileInput(std::string& source, std::string& dest) {
        // Check if the source file exists
        std::ifstream srcFile(source, std::ios::binary);
        if (!srcFile) {
            return "Cannot open source file";
        }

        // Open the filesystem
        std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!fs) {
            // Close file
            srcFile.close();
            return "Cannot open filesystem";
        }

        // Read the filesystem description
        Description desc{};
        fs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
        if (!fs) {
            // Close file
            srcFile.close();
            fs.close();
            return "Can't read filesystem description";
        }

        // Size of the source file
        // Move the file pointer to the end to get the file size
        srcFile.seekg(0, std::ios::end);
        std::streampos fileSize = srcFile.tellg();
        if (fileSize == -1) {
            srcFile.close();
            return "Error determining file size";
        }
        srcFile.seekg(0, std::ios::beg);

        // Check if there is enought space in the filesystem
        int currentFreeSpace = desc.disk_size - desc.data_start_address;
        if (fileSize > currentFreeSpace) {
            // Close file
            srcFile.close();
            fs.close();
            return "Not enough space in the filesystem";
        }

        // Find the cluster where the file will be written
        int32_t cluster;

        std::string dPath = dest.substr(0, dest.find_last_of('/'));
        std::string dName = dest.substr(dest.find_last_of('/') + 1);

        if (dPath == dName) {
            dPath = currentPath;
            cluster = currentCluster;
        } else {
            std::pair result = Utils::splitPath(dPath);
            if (result.second == -1 && result.first.empty()) {
                // Close file
                srcFile.close();
                fs.close();
                return "Cannot find destination directory";
            }
            cluster = result.second;
        }

        // Read the directory items from the cluster into the dirItems array
        DirectoryItem dirItems[desc.cluster_size / sizeof(DirectoryItem)];
        DirectoryItem dirItem{};
        fs.seekg(desc.data_start_address + cluster * desc.cluster_size, std::ios::beg);
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
            dirItems[i] = dirItem;
        }

        // Check if the file already exists in the destination directory
        for (const auto& item : dirItems) {
            if (std::string(item.name) == dName) {
                // Close file
                srcFile.close();
                fs.close();
                return "File already exists in the destination directory";
            }
        }

        // Check if cluster has enough space for directory item
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

        // Check if in filesystem is enough space
        int srcClusterRequired = 1 + (fileSize / desc.cluster_size);

        // Initialize the FAT
        FAT fat;
        fat.readFromFile(filename);

        // Find if there is enough space in the filesystem
        int freeClusters = 0;
        for (int i = 0; i < desc.cluster_count; ++i) {
            if (fat.Clusters[i] == FAT_UNUSED) {
                freeClusters++;
            }
        }
        if (freeClusters < srcClusterRequired) {
            // Close file
            srcFile.close();
            fs.close();
            return "Not enough space in the filesystem";
        }

        // Filename limiter
        if (dName.size() > ITEM_MAX_NAME) {
            return "Error: Destination file name is too long";
        }

        // Split the srcFile into fragments
        std::vector<std::string> fragments;
        auto buffer = new char[desc.cluster_size];
        while (srcFile.read(buffer, desc.cluster_size) || srcFile.gcount() > 0) {
            fragments.emplace_back(buffer, srcFile.gcount());
        }
        delete[] buffer;

        // Find free cluster for the content of the file
        int32_t freeCluster = fat.findFreeCluster();

        // Prepare directory item that will be in destination directory
        std::string name = dName;
        std::copy(name.begin(), name.end(), dirItem.name);
        dirItem.isFile = true;
        dirItem.size = static_cast<int32_t>(fileSize);
        dirItem.start_cluster = freeCluster;
        dirItem.parent_cluster = cluster;

        // Write the directory item to the parent cluster
        // Find the first free directory item in dirItems
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (dirItems[i].name[0] == '\0' || dirItems[i].name[0] == '0') {
                dirItems[i] = dirItem;
                fs.seekp(desc.data_start_address + cluster * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
                fs.write(reinterpret_cast<char*>(&dirItems[i]), sizeof(dirItem));
                break;
            }
        }

        // Write the file to the filesystem
        // Each time cluster is filled with data, find the next free cluster and update the FAT
        int32_t previousCluster = -1;
        for (int i = 0; i < fragments.size(); ++i) {
            // Find the next free cluster
            if (i != 0) {
                freeCluster = fat.findFreeCluster();
            }
            if (freeCluster == -1) {
                // Close file
                srcFile.close();
                fs.close();
                return "No free clusters in the filesystem";
            }

            // Update the FAT
            if (i == fragments.size() - 1) {
                fat.Clusters[freeCluster] = FAT_FILE_END;
            } else {
                fat.Clusters[freeCluster] = fat.findFreeCluster();
            }
            // Link the previous cluster to the current one
            if (previousCluster != -1) {
                fat.Clusters[previousCluster] = freeCluster;
            }
            previousCluster = freeCluster;

            // Write the fragment to the filesystem
            fs.seekp(desc.data_start_address + freeCluster * desc.cluster_size, std::ios::beg);
            fs.write(fragments[i].c_str(), fragments[i].size());
            if (!fs) {
                // Close file
                srcFile.close();
                fs.close();
                return "Cannot write to the filesystem";
            }
        }

        // Write the FAT back to the filesystem
        fat.writeToFile(filename);

        // Close the files
        srcFile.close();
        fs.close();

        return "File copied successfully";
    }
}