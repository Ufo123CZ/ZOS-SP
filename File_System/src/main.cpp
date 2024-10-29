#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "FileCheck/FileCheck.h"
#include "Utils/CommandMap.h"
#include "Commands/Commands.h"

// Global variables
int currentCluster = ROOT_CLUSTER;
std::string currentPath = ROOT_DIRECTORY;
std::string filename;
bool isFilesystemLoaded = false;
bool isFilesystemDamaged = false;

/**
 * @brief Main function
 * @param argc - number of arguments
 * @param argv - arguments
 * @return 0 if successful
 */
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
        std::cout << "File does not exist." << std::endl
        << " File will be created when the filesystem is formatted." << std::endl;
    } else {
        std::cout << "File exists. Loading filesystem..." << std::endl;
        ifs.close();
        isFilesystemLoaded = true;
        std::cout << "File loaded successfully" << std::endl;
        // Check for bad clusters
        BugCheck::checkForBugs();
    }

    // Initialize the command map
    CommandMap::initCommandMap();

    while (true) {
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
        if (!isFilesystemLoaded && command != "format" && command != "exit" && command != "help" && command != "load" && command != "check") {
            std::cout << "Filesystem not loaded. Only 'format', 'help', 'exit' or 'load' command is available." << std::endl;
        } else if (isFilesystemDamaged && command != "format" && command != "exit" && command != "help" && command != "load" && command != "check") {
            std::cout << "Filesystem damaged. Only 'format', 'help', 'exit' or 'load' command is available." << std::endl;
        } else {
            if (!command.empty()) {
                auto cmd = CommandMap::commandMap.find(command);
                if (cmd != CommandMap::commandMap.end()) {
                    cmd->second(arg1, arg2);
                } else {
                    std::cout << "Invalid command" << std::endl
                              << "Type 'help' for a list of commands" << std::endl;
                }
            }
        }

        // Clear the variables
        command.clear();
        arg1.clear();
        arg2.clear();
    }
}