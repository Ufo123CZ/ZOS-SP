#include "Commands.h"
#include "../Utils/CommandMap.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

extern bool isFilesystemLoaded;

namespace Load {
    std::string loadCommands(std::string& commandFile) {
        // Read the command file
        std::ifstream file(commandFile);
        if (!file.is_open()) {
            return "Error: Could not open command file";
        }

        std::string line;
        while (std::getline(file, line)) {

            if (line[0] == '#' || line.empty()) {
                continue;
            }

            std::istringstream iss(line);
            std::string command, arg1, arg2;
            iss >> command >> arg1 >> arg2;
            if (!isFilesystemLoaded && command != "format" && command != "exit" && command != "help" && command != "load") {
                std::cout << "Filesystem not loaded. Only 'format', 'help', 'exit' or 'load' command is available." << std::endl;
            } else {
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