#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <cstdint>

namespace Utils {
    std::pair<std::string, int32_t> splitPath(std::string& path);
}

#endif // UTILS_H