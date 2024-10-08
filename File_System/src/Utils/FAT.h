#ifndef FAT_H
#define FAT_H

#include "Items.h"
#include <string>

class FAT {
public:
    int Clusters[FORMAT_CLUSTER_COUNT] = {};
    void writeToFile(const std::string& filename);
    void readFromFile(const std::string& filename);
    int findFreeCluster() const;
};

#endif //FAT_H