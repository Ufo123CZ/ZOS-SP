#include "Commands.h"

string Format::formatFile(FILE* file, const string &size) {
    if (file == nullptr) {
        cerr << "Error: Could not open file" << endl;
        return "CANNOT CREATE FILE";
    }

    int fileSize = stoi(size);
    fseek(file, fileSize - 1, SEEK_SET);
    fputc('\0', file);
    fflush(file);
    return "OK";
}