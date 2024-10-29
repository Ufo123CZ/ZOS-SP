#include "Commands.h"
#include "../Utils/Utils.h"
#include "../Utils/FAT.h"
#include "../Utils/Items.h"
#include <string>
#include <cstring>
#include <fstream>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Mv {
    /**
     * @brief Move a file from one directory to another and rename it if needed
     * @param source - path to the source file
     * @param dest - path to the destination file
     * @return - message if the file was moved and renamed
     */
    std::string moveFile(std::string& source, std::string& dest) {
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

        // Split the source path into directory and file name
        std::string sPath = source.substr(0, source.find_last_of('/'));
        std::string sName = source.substr(source.find_last_of('/') + 1);
        std::string dPath = dest.substr(0, dest.find_last_of('/'));
        std::string dName = dest.substr(dest.find_last_of('/') + 1);

        // Rename the file if the source and destination names are different
        bool rename = false;
        if (sName != dName) {
            rename = true;
        }

        // Split the paths into directory and file name for source and destination
        std::pair<std::string, int32_t> sResult, dResult;
        if (sPath == sName) {
            sResult = {currentPath, currentCluster};
        } else {
            sResult = Utils::splitPath(sPath);
            if (sResult.second == -1 && sResult.first.empty()) {
                return "Error: Cannot find file";
            }
        }
        if (dPath == dName) {
            dResult = {currentPath, currentCluster};
        } else {
            dResult = Utils::splitPath(dPath);
            if (dResult.second == -1 && dResult.first.empty()) {
                return "Error: Cannot find file";
            }
        }

        // Find the source file in the directory
        DirectoryItem sDirItems[desc.cluster_size / sizeof(DirectoryItem)];
        DirectoryItem sDirItem{};
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.seekg(desc.data_start_address + sResult.second * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
            fs.read(reinterpret_cast<char*>(&sDirItem), sizeof(sDirItem));
            sDirItems[i] = sDirItem;
        }

        // Find the destination directory
        DirectoryItem dDirItems[desc.cluster_size / sizeof(DirectoryItem)];
        DirectoryItem dDirItem{};
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            fs.seekg(desc.data_start_address + dResult.second * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
            fs.read(reinterpret_cast<char*>(&dDirItem), sizeof(dDirItem));
            dDirItems[i] = dDirItem;
        }

        // Find the source file in the directory items
        bool found = false;
        DirectoryItem sourceI{};
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (std::string(sDirItems[i].name) == sName && sDirItems[i].isFile) {
                found = true;
                sourceI = sDirItems[i];
                break;
            }
        }
        if (!found) {
            fs.close();
            return "Source file not found in the filesystem";
        }

        // Rename the file
        if (rename) {
            // Clear sourceI
            std::memset(sourceI.name, 0, sizeof(sourceI.name));
            // Copy the new name
            strncpy(sourceI.name, dName.c_str(), dName.size());
        }

        // Only Renaming
        if (sResult.second == dResult.second) {
            // Check if sourceI is already in the destination directory
            for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
                if (std::string(dDirItems[i].name) == sourceI.name) {
                    fs.close();
                    return "File already exists in the destination directory";
                }
            }

            for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
                if (std::string(sDirItems[i].name) == sName && sDirItems[i].isFile) {
                    fs.seekg(desc.data_start_address + sResult.second * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
                    fs.write(reinterpret_cast<char*>(&sourceI), sizeof(sourceI));
                    break;
                }
            }

            // Close the filesystem
            fs.close();

            return "File renamed successfully";
        }
        // Check if sourceI is already in the destination directory
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (std::string(dDirItems[i].name) == sourceI.name) {
                fs.close();
                return "File already exists in the destination directory";
            }
        }

        // Check if there is a free space in destination directory
        bool freeSpace = false;
        for (const auto& item : dDirItems) {
            if (item.name[0] == '\0') {
                freeSpace = true;
                break;
            }
        }
        if (!freeSpace) {
            return "Error: No free space in the destination directory";
        }

        // Create a new file in the destination directory
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (dDirItems[i].name[0] == '\0') {
                sourceI.parent_cluster = dResult.second;
                fs.seekp(desc.data_start_address + dResult.second * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
                fs.write(reinterpret_cast<const char*>(&sourceI), sizeof(sourceI));
                break;
            }
        }

        // Remove the source file from the source directory
        for (int i = 0; i < desc.cluster_size / sizeof(DirectoryItem); ++i) {
            if (std::string(sDirItems[i].name) == sName && sDirItems[i].isFile) {
                // Clear sourceI
                sDirItems[i].name[0] = '\0';
                fs.seekg(desc.data_start_address + sResult.second * desc.cluster_size + i * sizeof(DirectoryItem), std::ios::beg);
                fs.write(reinterpret_cast<char*>(&sDirItems[i]), sizeof(sDirItems[i]));
                break;
            }
        }

        // Close the filesystem
        fs.close();

        if (rename) {
            return "File moved and renamed successfully";
        }
        return "File moved successfully";
    }
}