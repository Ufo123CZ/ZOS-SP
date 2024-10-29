#include "FileCheck.h"

namespace FileCheck {
    /**
     * @brief Check if the number of arguments is correct
     * @param argc - number of arguments
     * @param expectedSize - expected number of arguments
     * @return - true if the number of arguments is correct
     */
    bool checkArgcSize(const int argc, const int expectedSize) {
        return argc == expectedSize;
    }

    /**
     * @brief Check if the file is a .dat file
     * @param filename - name of the file
     * @return - true if the file is a .dat file
     */
    bool isDatFile(const std::string &filename) {
        return filename.size() >= 4 && filename.substr(filename.size() - 4) == ".dat";
    }
} // namespace FileCheck