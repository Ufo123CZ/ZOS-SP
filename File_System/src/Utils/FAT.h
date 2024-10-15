#ifndef FAT_H
#define FAT_H

#include "Items.h"
#include <string>
#include <cstdint>
#include <vector>

class FAT {
public:
    int clusterCount = 0;
    int32_t* Clusters = nullptr;

    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename) const;
    [[nodiscard]] int findFreeCluster() const;
    [[nodiscard]]std::vector<int32_t> getClusterChain(int32_t startCluster);
};

#endif //FAT_H