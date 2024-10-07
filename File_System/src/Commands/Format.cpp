#include "Commands.h"

#include <iostream>
#include <fstream>

std::string Format::formatFile(FILE* file, const std::string &size) {
    if (file == nullptr) {
        std::cerr << "Error: Could not open file" << std::endl;
        return "CANNOT CREATE FILE";
    }

    int fileSize = std::stoi(size);
    fseek(file, fileSize - 1, SEEK_SET);
    fputc('\0', file);
    fflush(file);
    return "OK";
}