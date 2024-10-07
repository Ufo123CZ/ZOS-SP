#include "FileCheck.h"

bool FileCheck::checkArgcSize(const int argc, const int expectedSize) {
    return argc == expectedSize;
}

bool FileCheck::isDatFile(const std::string &filename) {
    return filename.size() >= 4 && filename.substr(filename.size() - 4) == ".dat";
}