#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include "../Utils/FAT.h"
#include <fstream>
#include <iostream>
#include <string>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Cat {
    /**
     * @brief Read the content of a file
     * @param path - path to the file
     */
    void catFile(std::string& path) {
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

        // Read file from the path
        std::string readFile = path.substr(path.find_last_of('/') + 1);
        std::string pathToFile = path.substr(0, path.find_last_of('/'));
        if (pathToFile == readFile) {
            pathToFile = currentPath;
        }

        // Find the that will be read
        int32_t cluster;
        if (!path.empty()) {
            std::pair result = Utils::splitPath(pathToFile);
            if (result.second == -1 && result.first.empty()) {
                std::cerr << "Error: Cannot find file" << std::endl;
                return;
            }
            cluster = result.second;
        } else {
            cluster = currentCluster;
        }

        // Read the directory items from the cluster into the dirItems array
        DirectoryItem dirItems[desc.cluster_size / sizeof(DirectoryItem)];
        DirectoryItem dirItem{};
        ifs.seekg(desc.data_start_address + cluster * desc.cluster_size, std::ios::beg);
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            ifs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
            if (!ifs) {
                std::cerr << "Error: Could not read directory items" << std::endl;
                return;
            }
            dirItems[i] = dirItem;
        }

        // Find the file in the directory items
        bool found = false;
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (dirItems[i].name == readFile && dirItems[i].isFile) {
                found = true;
                dirItem = dirItems[i];
                break;
            }
        }
        if (!found) {
            std::cerr << "Error: File not found" << std::endl;
           return;
        }

        // Initialize the FAT
        FAT fat;
        fat.readFromFile(filename);

        // Parameters for reading the file
        cluster = dirItem.start_cluster;
        int remainingSize = dirItem.size;

        // Get the clusters of the file
        std::vector<int32_t> clusters = fat.getClusterChain(cluster);

        // Print the content of the file in the console from the clusters
        for (int cluster1 : clusters) {
            ifs.seekg(desc.data_start_address + cluster1 * desc.cluster_size, std::ios::beg);
            int readSize = std::min(desc.cluster_size, remainingSize);
            char buffer[desc.cluster_size];
            ifs.read(buffer, readSize);
            if (!ifs) {
                std::cerr << "Error: Could not read file" << std::endl;
                return;
            }
            std::cout << std::string(buffer, readSize);
            remainingSize -= readSize;
            if (remainingSize <= 0) {
                break;
            }
        }
        std::cout << std::endl;

        // Close the filesystem
        ifs.close();
    }
}