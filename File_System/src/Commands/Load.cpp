#include "Commands.h"
#include "../Utils/CommandMap.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

extern std::string currentPath;
extern bool isFilesystemLoaded;
extern bool isFilesystemDamaged;

namespace Load {
    /**
     * @brief Load the commands from the file
     * @param commandFile - path to the file with commands
     * @return - message if the commands were loaded
     */
    std::string loadCommands(std::string& commandFile) {
        // Read the command file
        std::ifstream file(commandFile);
        if (!file.is_open()) {
            return "Error: Could not open command file";
        }

        // Read the commands
        std::string line;
        while (std::getline(file, line)) {

            if (line[0] == '#' || line.empty()) {
                continue;
            }

            // Parse the command
            std::istringstream iss(line);
            std::string command, arg1, arg2;
            iss >> command >> arg1 >> arg2;

            // Check if the path is correct
            if (currentPath[currentPath.size() - 1] == '/') {
                currentPath = currentPath.substr(0, currentPath.size() - 1);
            }
            if (currentPath.empty()) {
                currentPath = ROOT_DIRECTORY;
            }
            std::string printedCommand = command;
            if (!arg1.empty()) {
                printedCommand += " " + arg1;
            }
            if (!arg2.empty()) {
                printedCommand += " " + arg2;
            }
            std::cout << "LOAD: " << COMAND_PREFIX1 << currentPath << COMAND_PREFIX2  << printedCommand << std::endl;

            // Check if the filesystem is loaded
            if (!isFilesystemLoaded && command != "format" && command != "exit" && command != "help" && command != "load" && command != "check") {
                std::cout << "Filesystem not loaded. Only 'format', 'help', 'exit' or 'load' command is available." << std::endl;
                return "Commands not executed";
            }
            // Check if the filesystem is damaged
            if (isFilesystemDamaged && command != "format" && command != "exit" && command != "help" && command != "load" && command != "check") {
                std::cout << "Filesystem damaged. Only 'format', 'help', 'exit' or 'load' command is available." << std::endl;
                return "Commands not executed";
            }

            // Check if the command is valid
            if (!command.empty()) {
                auto cmd = CommandMap::commandMap.find(command);
                if (cmd != CommandMap::commandMap.end()) {
                    cmd->second(arg1, arg2);
                } else {
                    return "Command: " + command + " not recognized";
                }
            }
            command.clear();
            arg1.clear();
            arg2.clear();
        }

        file.close();
        return "Commands executed successfully";
    }
} // namespace Load