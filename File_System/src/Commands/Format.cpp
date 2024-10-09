#include "Commands.h"
#include "../Utils/Items.h"
#include "../Utils/FAT.h"
#include <fstream>
#include <regex>

namespace Format {
    std::string formatFile(std::string& filename, std::string& size) {
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) {
            return "Cannot create file";
        }

        std::regex sizeRegex(R"(^(\d+)(MB)$)");
        std::smatch match;

        if (!std::regex_match(size, match, sizeRegex)) {
            return "Invalid size or wrong format. Size must be in MB";
        }

        // Convert the size to bytes
        int disk_size = std::stoi(match[1]) * 1024 * 1024; // 1MB = 1024KB = 1024 * 1024 bytes

        // Calculate the start addresses
        int fat_start_address = sizeof(Description);
        int data_start_address = fat_start_address + sizeof(int32_t) * FORMAT_CLUSTER_COUNT;


        Description desc = {
            FORMAT_NAME,
            disk_size,
            FORMAT_CLUSTER_SIZE,
            FORMAT_CLUSTER_COUNT,
            FORMAT_CLUSTER_COUNT,
            fat_start_address,
            data_start_address

        };

        // Write the description to the file
        ofs.write(reinterpret_cast<char*>(&desc), sizeof(Description));

        // Initialize and write the FAT clusters
        FAT fat;
        for (int & Cluster : fat.Clusters) {
            Cluster = FAT_UNUSED;
        }
        // Write the Clusters directly after the Description
        for (const auto& index : fat.Clusters) {
            ofs.write(reinterpret_cast<const char*>(&index), sizeof(index));
        }

        return "File formatted successfully";
    }
}