#ifndef ITEMS_H
#define ITEMS_H

#include <cstdint>
#include <climits>

#define FAT_MAX_NAME 12
#define ITEM_MAX_NAME 12

// Constants for the filesystem
#define FORMAT_NAME "root"
#define FORMAT_CLUSTER_SIZE 1024

// Special markers in the FAT table
constexpr int32_t FAT_UNUSED = INT32_MAX - 1;
constexpr int32_t FAT_FILE_END = INT32_MAX - 2;
constexpr int32_t FAT_BAD_CLUSTER = INT32_MAX - 3;

// Description of the filesystem
class Description {
public:
    char login[FAT_MAX_NAME];
    int32_t disk_size;
    int32_t cluster_size;
    int32_t cluster_count;
    int32_t fat_start_address;
    int32_t data_start_address;
};

// Directory item structure
class DirectoryItem {
public:
    char name[ITEM_MAX_NAME];
    bool isFile;
    int32_t size;
    int32_t start_cluster;
    int32_t parent_cluster;
};

#endif //ITEMS_H
