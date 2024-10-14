#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include "../Utils/FAT.h"
#include <string>
#include <fstream>
#include <iostream>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Incp {

    std::string copyFileInput(std::string& source, std::string& dest) {
        std::ifstream srcFile(source, std::ios::binary);

        // Check if the source file exists
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

        // Destination is a empty string
        if (dest.empty()) dest = currentPath;

        // Find the cluster where the file will be written
        int32_t cluster = currentCluster;
        std::pair result = Utils::splitPath(dest);
        if (result.second == -1 && result.first == "/") {
            // Close file
            srcFile.close();
            fs.close();
            return "Cannot find destination directory";
        }
        cluster = result.second;

        // Check if in filesystem is enough space
        int srcClusterRequired = 1 + (srcFile.tellg() / desc.cluster_size);

        // Init FAT
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

        // Split the srcFile into fragments
        std::vector<std::string> fragments;
        char* buffer = new char[desc.cluster_size];
        while (srcFile.read(buffer, desc.cluster_size) || srcFile.gcount() > 0) {
            fragments.emplace_back(buffer, srcFile.gcount());
        }
        delete[] buffer;


        // Write the file to the filesystem
        // Each time cluster is filled with data, find the next free cluster and update the FAT
        int32_t freeCluster = -1;
        for (int i = 0; i < fragments.size(); ++i) {
            // Find the next free cluster
            freeCluster = fat.findFreeCluster();
            if (freeCluster == -1) {
                // Close file
                srcFile.close();
                fs.close();
                return "No free clusters in the filesystem";
            }

            DirectoryItem dirItem{};
            // Name of dirItem is the name of the file
            std::string name = source.substr(source.find_last_of('/') + 1);
            std::copy(name.begin(), name.end(), dirItem.name);
            // Size of the file is the size of the fragment
            dirItem.size = fragments[i].size();
            // File is not a directory
            dirItem.isFile = true;
            // Start cluster is the free cluster
            dirItem.start_cluster = freeCluster;
            // Parent cluster is the cluster where the file will be written
            dirItem.parent_cluster = cluster;

            cluster = freeCluster;

            // Update the FAT
            if (i == fragments.size() - 1) {
                fat.Clusters[freeCluster] = FAT_FILE_END;
            } else {
                fat.Clusters[freeCluster] = freeCluster + 1;
            }

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