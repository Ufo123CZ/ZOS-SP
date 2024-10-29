#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include "../Utils/FAT.h"
#include <string>
#include <fstream>

#include <vector>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Outcp {
    /**
     * @brief Copy a file from the filesystem to the outside of filesystem
     * @param source - path to the source file in the filesystem
     * @param dest - path to the destination outside filesystem
     * @return - message if the file was copied
     */
    std::string copyFileOutput(std::string& source, std::string& dest) {
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
        std::string sPath = source.substr(0, source.find_last_of('/'));
        std::string sName = source.substr(source.find_last_of('/') + 1);

        // If the source file is in the current directory, set the path to the current path
        int32_t sDirCluster;
        if (sPath == sName) {
            sPath = currentPath;
            sDirCluster = currentCluster;
        } else {
            std::pair<std::string, int32_t> result = Utils::splitPath(sPath);
            if (result.second == -1 && result.first.empty()) {
                fs.close();
                return "Cannot find source file";
            }
            sDirCluster = result.second;
        }

        // Read the directory items from the cluster into the dirItems array
        DirectoryItem dirItems[desc.cluster_size / sizeof(DirectoryItem)];
        DirectoryItem dirItem{};
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.seekg(desc.data_start_address + sDirCluster * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
            fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem));
            dirItems[i] = dirItem;
        }

        // Find the source file in the directory items
        int32_t cluster = -1;
        int fileSize = 0;
        for (const auto& item : dirItems) {
            if (std::string(item.name) == sName && item.isFile) {
                cluster = item.start_cluster;
                fileSize = item.size;
                break;
            }
        }
        if (cluster == -1) {
            fs.close();
            return "Source file not found in the filesystem";
        }

        // Open the destination file
        std::ofstream destFile(dest, std::ios::out | std::ios::binary);
        if (!destFile) {
            fs.close();
            return "Cannot open destination file";
        }

        // Read the FAT table
        FAT fat;
        fat.readFromFile(filename);

        // Read the file's data from the filesystem and write it to the destination file
        while (cluster != FAT_FILE_END) {
            int nowRead;
            if (fileSize > desc.cluster_size) {
                nowRead = desc.cluster_size;
                fileSize -= desc.cluster_size;
            } else {
                nowRead = fileSize;
                fileSize = 0;
            }
            auto buffer = new char[nowRead];
            fs.seekg(desc.data_start_address + cluster * desc.cluster_size, std::ios::beg);
            fs.read(buffer, nowRead);
            destFile.write(buffer, nowRead);
            cluster = fat.Clusters[cluster];
            delete[] buffer;
        }

        // Close the files
        destFile.close();
        fs.close();

        return "File exported successfully";
    }
}