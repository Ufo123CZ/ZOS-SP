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
        std::string path = source.substr(0, source.find_last_of('/'));
        std::string exportName = source.substr(source.find_last_of('/') + 1);

        if (path == exportName) {
            path = currentPath;
        }

        std::pair<std::string, int32_t> result = Utils::splitPath(path);
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

        int32_t cluster = -1;
        std::string newFileName = dest + "/" + exportName;
        for (const auto& item : dirItems) {
            if (std::string(item.name) == exportName && item.isFile) {
                cluster = item.start_cluster;
                break;
            }
        }
        if (cluster == -1) {
            fs.close();
            return "Source file not found in the filesystem";
        }


        // Open the destination file
        std::ofstream destFile(newFileName, std::ios::out | std::ios::binary);
        if (!destFile) {
            fs.close();
            return "Cannot open destination file";
        }

        // Read the FAT table
        FAT fat;
        fat.readFromFile(filename);

        // Read the file's data from the filesystem and write it to the destination file
        auto buffer = new char[desc.cluster_size];
        while (cluster != FAT_FILE_END) {
            fs.seekg(desc.data_start_address + cluster * desc.cluster_size, std::ios::beg);
            fs.read(buffer, desc.cluster_size);
            destFile.write(buffer, fs.gcount());
            cluster = fat.Clusters[cluster];
        }
        delete[] buffer;

        // Close the files
        destFile.close();
        fs.close();

        return "File exported successfully";
    }
}