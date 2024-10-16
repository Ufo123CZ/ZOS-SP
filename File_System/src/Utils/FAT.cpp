#include "FAT.h"
#include "Items.h"
#include <fstream>
#include <stdexcept>
#include <vector>

void FAT::readFromFile(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Cannot open file for reading");
    }

    // Read the Description
    Description desc{};
    ifs.read(reinterpret_cast<char*>(&desc), sizeof(desc));
    if (!ifs) {
        throw std::runtime_error("Cannot read filesystem description");
    }

    // Allocate Clusters array based on cluster_count
    clusterCount = desc.cluster_count;
    Clusters = new int32_t[clusterCount];

    // Read the Clusters
    ifs.read(reinterpret_cast<char*>(Clusters), sizeof(int32_t) * clusterCount);
    if (!ifs) {
        throw std::runtime_error("Cannot read FAT clusters");
    }
}

void FAT::writeToFile(const std::string& filename) const {
    std::ofstream ofs(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!ofs) {
        throw std::runtime_error("Cannot open file for writing");
    }

    // Write the Clusters
    ofs.seekp(sizeof(Description), std::ios::beg); // Skip the Description
    ofs.write(reinterpret_cast<const char*>(Clusters), sizeof(int32_t) * clusterCount);
    if (!ofs) {
        throw std::runtime_error("Cannot write FAT clusters");
    }
}

int FAT::findFreeCluster() const {
    for (int i = 0; i < clusterCount; ++i) {
        if (Clusters[i] == FAT_UNUSED) {
            return i;
        }
    }
    return -1;
}

std::vector<int32_t> FAT::getClusterChain(int32_t startCluster) const {
    std::vector<int32_t> clusters;

    if (Clusters[startCluster] == FAT_FILE_END) {
        return clusters;
    }

    int32_t cluster = startCluster;

    while (Clusters[cluster] != FAT_FILE_END) {
        clusters.push_back(cluster);
        cluster = Clusters[cluster];
    }
    clusters.push_back(cluster);

    return clusters;
}