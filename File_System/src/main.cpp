#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <fstream>
#include "FileCheck/FileCheck.h"
#include "Utils/Utils.h"
#include "Commands/Commands.h"

#define COMAND_PREFIX1 "~"
#define COMAND_PREFIX2 "$"

#define ROOT_CLUSTER (-2)

// Global variables
int currentCluster = ROOT_CLUSTER;
std::string currentDirectory;

int main(int argc, char* argv[]) {
    // Check if the number of arguments is correct
    if (!FileCheck::checkArgcSize(argc, 2)) {
        std::cout << "Need to provide a file" << std::endl;
        exit(1);
    }

    // Check if the file is a .dat file
    if (!FileCheck::isDatFile(argv[1])) {
        std::cout << "File must have .dat extension" << std::endl;
        exit(1);
    }
    std::cout << "File is a .dat file" << std::endl;

    // Check if the file exists
    if (std::ifstream ifs(argv[1], std::ios::binary); !ifs) {
        std::cout << "File does not exist. Creating and initializing filesystem..." << std::endl;
        std::ofstream ofs(argv[1], std::ios::binary);
        if (!ofs) {
            std::cout << "Error: Could not create file " << argv[1] << std::endl;
        } else {
            std::cout << "File created successfully" << std::endl;
        }
    } else {
        std::cout << "File exists. Loading filesystem..." << std::endl;
        ifs.close();
        std::cout << "File loaded successfully" << std::endl;
    }

    std::unordered_map<std::string, std::function<void(std::string&, std::string&)>> commandMap = {
    {"cp", [argv](std::string& arg1, std::string& arg2){ }},
    {"mv", [argv](std::string& arg1, std::string& arg2){ }},
    {"rm", [argv](std::string& arg1, std::string&){ }},
    {"mkdir", [argv](std::string& arg1, std::string&){ std::cout << MkDir::makeDirectory(reinterpret_cast<std::string &>(argv[1]), arg1, currentCluster) << std::endl; }},
    {"rmdir", [argv](std::string& arg1, std::string&){ std::cout << RmDir::removeDirectory(reinterpret_cast<std::string &>(argv[1]), arg1, currentCluster) << std::endl; }},
    {"ls", [argv](std::string& arg1, std::string&) {
        std::string listThis;
        if (arg1.empty()) { listThis = currentDirectory; } else { listThis = arg1; }
        std::cout << Ls::listDirectory(reinterpret_cast<std::string &>(argv[1]), listThis) << std::endl;
    }},
    {"cat", [argv](std::string& arg1, std::string&){ }},
    {"cd", [argv](std::string& arg1, std::string&) {
        std::pair<std::string, int32_t> result = Cd::changeDirectory(reinterpret_cast<std::string &>(argv[1]), currentDirectory, arg1, currentCluster);
        if (result.second != -1) { currentDirectory = result.first; currentCluster = result.second; }
    }},
    {"pwd", [argv](std::string&, std::string&) {
        if (currentDirectory.empty()) { std::cout << "You are in root" << std::endl; }
        else { std::cout << "Current directory: " << currentDirectory << std::endl; }
    }},
    {"info", [argv](std::string& arg1, std::string& arg2){ }},
    {"incp", [argv](std::string& arg1, std::string& arg2){ }},
    {"outcp", [argv](std::string& arg1, std::string& arg2){ }},
    {"load", [argv](std::string& arg1, std::string&){ }},
    {"format", [argv](std::string& arg1, std::string&){ std::cout << Format::formatFile(reinterpret_cast<std::string &>(argv[1]), arg1) << std::endl; }},

    {"exit", [](std::string&, std::string&) { Utils::endProgram(); }},
    {"help", [](std::string&, std::string&) { Help::writeHelpInConsole(); }}
    // {"test", [](std::string& arg1, std::string& arg2) { std::cout << "Command: test, Arg1: " << arg1 << ", Arg2: " << arg2 << std::endl; }}
};

    while (true) {
        if (std::ifstream ifs(argv[1], std::ios::binary); !ifs) {
            std::cerr << "Error: Could not open file " << argv[1] << std::endl;
            break;
        }
        // Get the input
        std::string input;
        std::string command, arg1, arg2;
        std::cout << COMAND_PREFIX1 << currentDirectory << COMAND_PREFIX2;
        getline(std::cin, input);

        // Parse the input
        std::istringstream iss(input);
        iss >> command >> arg1 >> arg2;

        // Check if the command is valid
        auto cmd = commandMap.find(command);
        if (cmd != commandMap.end()) {
            cmd->second(arg1, arg2);
        } else {
            std::cout << "Invalid command" << std::endl
            << "Type 'help' for a list of commands" << std::endl;
        }



        // Clear the variables
        command.clear();
        arg1.clear();
        arg2.clear();
    }

    return 0;
}