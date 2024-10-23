#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include "../Utils/FAT.h"
#include <string>
#include <fstream>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Rm {
    std::string removeFile(std::string& path) {
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

        // Locate the source file in the filesystem
        std::string filePath = path.substr(0, path.find_last_of('/'));
        std::string exportName = path.substr(path.find_last_of('/') + 1);

        if (filePath == exportName) {
            filePath = currentPath;
        }

        std::pair<std::string, int32_t> result = Utils::splitPath(filePath);
        if (result.second == -1 && result.first == "") {
            fs.close();
            return "Cannot find source file";
        }

        // Read the directory items from the cluster into the dirItems array
        DirectoryItem dirItems[desc.cluster_size / sizeof(DirectoryItem)];
        DirectoryItem dirItem{};
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.seekg(desc.data_start_address + result.second * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
            fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
            dirItems[i] = dirItem;
        }

        // Find the source file in the directory items
        int32_t cluster = -1;
        for (const auto& item : dirItems) {
            if (std::string(item.name) == exportName && item.isFile) {

                // Rewrite the directory item name[0]
                fs.seekp(desc.data_start_address + result.second * desc.cluster_size + item.start_cluster * sizeof(DirectoryItem), std::ios::beg);
                dirItem = item;
                dirItem.name[0] = '\0';
                fs.write(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));

                cluster = item.start_cluster;
                break;
            }
        }
        if (cluster == -1) {
            fs.close();
            return "Source file not found in the filesystem";
        }

        // Find clusters of the file
        FAT fat;
        fat.readFromFile(filename);
        std::vector<int32_t> clusters = fat.getClusterChain(cluster);

        // Clear the clusters and write the FAT table
        for (int32_t cl : clusters) {
            // Clear the content of the cluster
            fs.seekp(desc.data_start_address + cl * desc.cluster_size, std::ios::beg);
            for (int i = 0; i < desc.cluster_size; ++i) {
                fs.put(0);
            }
            // Mark the cluster as unused
            fat.Clusters[cl] = FAT_UNUSED;
        }
        fat.writeToFile(filename);

        // Close the filesystem
        fs.close();

        return "File removed successfully";
    }
}