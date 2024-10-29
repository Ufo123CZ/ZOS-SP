#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/FAT.h"
#include <iostream>
#include <string>
#include <fstream>

extern std::string filename;
extern bool isFilesystemDamaged;

namespace BugCheck {
    /**
     * @brief Check for bad clusters in the filesystem
     */
    void checkForBugs() {
        // Open the filesystem
        std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!fs) {
            std::cout << "Cannot open filesystem" << std::endl;
            return;
        }

        // Read the filesystem description
        Description desc{};
        fs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
        if (!fs) {
            // Close file
            fs.close();
            std::cout << "Can't read filesystem description" << std::endl;
            return;
        }

        // Initialize the FAT
        FAT fat;
        fat.readFromFile(filename);

        // Check for bad clusters
        bool found = false;
        for (int i = 0; i < desc.cluster_count; i++) {
            if (fat.Clusters[i] == FAT_BAD_CLUSTER) {
                found = true;
                std::cout << "In Cluster " << i << " is something wrong!" << std::endl;
            }
        }
        if (!found) {
            std::cout << "No bad clusters found" << std::endl;
        } else {
            isFilesystemDamaged = true;
            std::cout << "Bad clusters found. Flagging FileSystem as broken." << std::endl;
        }

        // Close filesystem
        fs.close();
    }
}