#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>

#include "FileCheck/FileCheck.h"
#include "Utils/UtilF.h"
#include "Commands/Commands.h"

using namespace std;

[[noreturn]] int main(const int argc, char* argv[]) {
    // Check if the number of arguments is correct
    if (!FileCheck::checkArgcSize(argc, 2)) {
        cout << "Need to provide a file" << endl;
        return 1;
    }

    // Check if the file is a .dat file
    if (!FileCheck::isDatFile(argv[1])) {
        cout << "File must have .dat extension" << endl;
        return 1;
    }
    cout << "File is a .dat file" << endl;

    // FILE* file = fopen(argv[1], "rb");

    unordered_map<string, function<void()>> commandMap = {
        {"exit", []() {
            // fclose(file);
            cout << "Exiting..." << endl; exit(0);
        }},
        {"help", []() { UtilF::writeHeplInConsole(); }},
        {"format", []() {
            // string result = Format::formatFile(file, arg);
            // cout << result << endl;
            cout << "format" << endl;
        }}
    };

    while (true) {
        string input;
        cout << "Enter a command: ";
        getline(cin, input);

        istringstream iss(input);
        string command, arg;
        iss >> command;
        getline(iss, arg);
        // arg = arg.substr(arg.find_first_not_of(" ")); // Trim leading spaces

        auto cmd = commandMap.find(command);
        if (cmd != commandMap.end()) {
            cmd->second();
        } else {
            cout << "Invalid command" << endl
                 << "Type 'help' for a list of commands" << endl;
        }

        cout << endl;
    }
}