#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/Utils.h"
#include <string>
#include <fstream>
#include <iostream>

extern int32_t currentCluster;
extern std::string currentPath;
extern std::string filename;

namespace Cd {
    void changeDirectory(std::string& path) {
        std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!fs) {
            std::cerr << "Cannot open filesystem" << std::endl;
            return;
        }

        // Read the filesystem description
        Description desc{};
        fs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
        if (!fs) {
            std::cerr << "Can't read filesystem description" << std::endl;
            return;
        }

        // Split the path into directories
        std::pair<std::string, int32_t> result = Utils::splitPath(path);
        if (result.first.empty() && result.second == -1) {
            std::cout << "Path does not exist" << std::endl;
        } else {
            //set result.first to currentPath and result.second to currentCluster
            currentPath = result.first;
            currentCluster = result.second;
        }



        // // Handle special case for returning to root
        // if (dirname == "..") {
        //     // Check if the current directory is the root
        //     if (currentDirectory.empty() && currentCluster == 0) {
        //         std::cerr << "Already at root directory" << std::endl;
        //         return {"", -2};
        //     }
        //
        //     // Read the current directory's parent cluster
        //     fs.seekg(desc.data_start_address, std::ios::beg);
        //     DirectoryItem dirItem{};
        //     while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
        //         if (dirItem.start_cluster == currentCluster) {
        //             std::string newDir = currentDirectory.substr(0, currentDirectory.find_last_of('/'));
        //             return {newDir, dirItem.parent_cluster};
        //         }
        //     }
        //     std::cerr << "Parent directory not found" << std::endl;
        //     return {"", -1};
        // }
        //
        // // Search for the directory
        // fs.seekg(desc.data_start_address, std::ios::beg);
        // DirectoryItem dirItem{};
        // while (fs.read(reinterpret_cast<char*>(&dirItem), sizeof(dirItem))) {
        //     if (dirItem.name == dirname && !dirItem.isFile && dirItem.parent_cluster == currentCluster) {
        //         std::string newDir = currentDirectory + "/" + dirItem.name;
        //         return {newDir, dirItem.start_cluster};
        //     }
        // }
        //
        // std::cerr << "Directory not found" << std::endl;
        // return {"", -1};
    }
}