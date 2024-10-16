#include <cstring>
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

#define ROOT_CLUSTER (0)
#define ROOT_DIRECTORY ("/")

// Global variables
int currentCluster = ROOT_CLUSTER;
std::string currentPath = ROOT_DIRECTORY;
std::string filename;
bool isFilesystemLoaded = false;

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

    // in filename store argv[1]
    filename = argv[1];

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
        isFilesystemLoaded = true;
        std::cout << "File loaded successfully" << std::endl;
    }

    std::unordered_map<std::string, std::function<void(std::string&, std::string&)>> commandMap = {
        // Commands
    {"cp", [](std::string& arg1, std::string& arg2){ }},
    {"mv", [](std::string& arg1, std::string& arg2){ }},
    {"rm", [](std::string& arg1, std::string&){ }},
    {"mkdir", [](std::string& arg1, std::string&) {
        std::cout << MkDir::makeDirectory(arg1) << std::endl;
    }},
    {"rmdir", [](std::string& arg1, std::string&) {
        // std::cout << RmDir::removeDirectory(reinterpret_cast<std::string &>(argv[1]), arg1, currentCluster) << std::endl;
    }},
    {"ls", [](std::string& arg1, std::string&) {
        std::cout << Ls::listDirectory(arg1) << std::endl;
    }},
    {"cat", [](std::string& arg1, std::string&) {
        Cat::catFile(arg1);
    }},
    {"cd", [](std::string& arg1, std::string&) {
        Cd::changeDirectory(arg1);
    }},
    {"pwd", [](std::string&, std::string&) {
        if (currentPath == "/") std::cout << "You are in root" << std::endl;
        else std::cout << "Current directory: " << currentPath << std::endl;
    }},
    {"info", [](std::string& arg1, std::string&) {
        std::cout << Info::fileInfo(arg1) << std::endl;
    }},
    {"incp", [](std::string& arg1, std::string& arg2) {
        std::cout << Incp::copyFileInput(arg1, arg2) << std::endl;
    }},
    {"outcp", [](std::string& arg1, std::string& arg2){ }},
    {"load", [](std::string& arg1, std::string&){ }},
    {"format", [](std::string& arg1, std::string&) {
        isFilesystemLoaded = true;
        std::cout << Format::formatFile(arg1) << std::endl;
    }},

        // Extra commands
    {"exit", [](std::string&, std::string&) {
        Utils::endProgram();
    }},
    {"help", [](std::string&, std::string&) {
        Help::writeHelpInConsole();
    }},
    {"test", [](std::string&, std::string&) {
        std::cout << "Command: test, Current path: " << currentPath << ", Current cluster: " << currentCluster << std::endl;
    }}
};

    while (true) {
        if (std::ifstream ifs(argv[1], std::ios::binary); !ifs) {
            std::cerr << "Error: Could not open file " << argv[1] << std::endl;
            break;
        }
        // Get the input
        std::string input;
        std::string command, arg1, arg2;
        if (currentPath[currentPath.size() - 1] == '/') {
            currentPath = currentPath.substr(0, currentPath.size() - 1);
        }
        if (currentPath.empty()) {
            currentPath = ROOT_DIRECTORY;
        }
        std::cout << COMAND_PREFIX1 << currentPath << COMAND_PREFIX2;
        getline(std::cin, input);

        // Parse the input
        std::istringstream iss(input);
        iss >> command >> arg1 >> arg2;

        // Check if the command is valid
        if (!isFilesystemLoaded && command != "format" && command != "exit" && command != "help") {
            std::cout << "Filesystem not loaded. Only 'format', 'help' or 'exit' command is available." << std::endl;
        } else {
            auto cmd = commandMap.find(command);
            if (cmd != commandMap.end()) {
                cmd->second(arg1, arg2);
            } else {
                std::cout << "Invalid command" << std::endl
                          << "Type 'help' for a list of commands" << std::endl;
            }
        }



        // Clear the variables
        command.clear();
        arg1.clear();
        arg2.clear();
    }

    return 0;
}