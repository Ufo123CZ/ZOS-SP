#include "Utils.h"
#include <iostream>
#include <fstream>

namespace Utils {

void endProgram() {
    std::cout << "Exiting..." << std::endl;
    exit(0);
}

void loadFilesystem(std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }
}

} // namespace Utils