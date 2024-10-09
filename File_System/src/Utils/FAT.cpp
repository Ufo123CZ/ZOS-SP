#include "FAT.h"
#include <fstream>
#include <stdexcept>

void FAT::readFromFile(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Cannot open file for reading");
    }

    for (auto& index : Clusters) {
        ifs.read(reinterpret_cast<char*>(&index), sizeof(index));
    }
}

void FAT::writeToFile(const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Cannot open file for writing");
    }


    for (const auto& index : Clusters) {
        ofs.write(reinterpret_cast<const char*>(&index), sizeof(index));
    }
}

int FAT::findFreeCluster() const {
    for (int i = 0; i < FORMAT_CLUSTER_COUNT; ++i) {
        if (Clusters[i] == FAT_UNUSED) {
            return i;
        }
    }
    return -1;
}
