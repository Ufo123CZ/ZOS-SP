#ifndef FAT_H
#define FAT_H

#include "Items.h"
#include <string>
#include <cstdint>

class FAT {
public:
    int clusterCount = 0;
    int32_t* Clusters = nullptr;

    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename) const;
    [[nodiscard]] int findFreeCluster() const;
};

#endif //FAT_H