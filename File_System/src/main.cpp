#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>

#include "FileCheck/FileCheck.h"
#include "Utils/UtilF.h"
#include "Commands/Commands.h"

FILE* file;

void endProgram() {
    fclose(file);
    std::cout << "Exiting..." << std::endl;
    exit(0);
}

[[noreturn]] int main(const int argc, char* argv[]) {
    // Check if the number of arguments is correct
    if (!FileCheck::checkArgcSize(argc, 2)) {
        std::cout << "Need to provide a file" << std::endl;
        return 1;
    }

    // Check if the file is a .dat file
    if (!FileCheck::isDatFile(argv[1])) {
        std::cout << "File must have .dat extension" << std::endl;
        return 1;
    }
    std::cout << "File is a .dat file" << std::endl;

    file = fopen(argv[1], "rb");

    std::unordered_map<std::string, std::function<void(std::string)>> commandMap = {
        {"exit", [](const std::string&) { endProgram(); }},
        {"help", [](const std::string&) { UtilF::writeHeplInConsole(); }},
        {"format", [](const std::string& arg1) { std::cout << Format::formatFile(file, arg1) << std::endl; }},
    };

    while (true) {
        std::string input;
        std::cout << "Enter a command: ";
        getline(std::cin, input);

        std::istringstream iss(input);
        std::string command, arg1, arg2;
        iss >> command;
        getline(iss, arg1);
        getline(iss, arg2);
        arg1 = arg1.substr(arg1.find_first_not_of(' ')); // Trim leading spaces
        arg2 = arg2.substr(arg2.find_last_of(' '));

        auto cmd = commandMap.find(command);
        if (cmd != commandMap.end()) {
            cmd->second(arg1);
        } else {
            std::cout << "Invalid command" << std::endl
                 << "Type 'help' for a list of commands" << std::endl;
        }

        std::cout << std::endl;
    }
}