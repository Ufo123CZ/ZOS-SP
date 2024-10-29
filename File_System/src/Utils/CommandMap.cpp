#include "CommandMap.h"

#include <complex>

#include "../Commands/Commands.h"
#include "Utils.h"
#include <iostream>

extern int currentCluster;
extern std::string currentPath;
extern bool isFilesystemLoaded;

std::unordered_map<std::string, std::function<void(std::string&, std::string&)>> CommandMap::commandMap;

void CommandMap::initCommandMap() {
    commandMap = {
        {"cp", [](std::string& arg1, std::string& arg2) {
            std::cout << Cp::copyFile(arg1, arg2) << std::endl;
        }},
        {"mv", [](std::string& arg1, std::string& arg2) {
            std::cout << Mv::moveFile(arg1, arg2) << std::endl;
        }},
        {"rm", [](std::string& arg1, std::string&) {
            std::cout << Rm::removeFile(arg1) << std::endl;
        }},
        {"mkdir", [](std::string& arg1, std::string&) {
            std::cout << MkDir::makeDirectory(arg1) << std::endl;
        }},
        {"rmdir", [](std::string& arg1, std::string&) {
            std::cout << RmDir::removeDirectory(arg1) << std::endl;
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
        {"outcp", [](std::string& arg1, std::string& arg2) {
            std::cout << Outcp::copyFileOutput(arg1, arg2) << std::endl;
        }},
        {"load", [](std::string& arg1, std::string&) {
            std::cout << Load::loadCommands(arg1) << std::endl;
        }},
        {"format", [](std::string& arg1, std::string&) {
            isFilesystemLoaded = true;
            std::cout << Format::formatFile(arg1) << std::endl;
        }},
        // Additional commands
        {"exit", [](std::string&, std::string&) {
            Utils::endProgram();
        }},
        {"help", [](std::string&, std::string&) {
            Help::writeHelpInConsole();
        }},
        // Additional required commands
        {"bug", [](std::string& arg1, std::string&) {
            BugCreator::createBug(arg1);
        }},
        {"check", [](std::string&, std::string&) {
            BugCheck::checkForBugs();
        }}
    };
}