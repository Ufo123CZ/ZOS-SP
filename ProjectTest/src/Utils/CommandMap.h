#ifndef COMMANDMAP_H
#define COMMANDMAP_H

#include <unordered_map>
#include <functional>
#include <string>

class CommandMap {
public:
    static void initCommandMap();
    static std::unordered_map<std::string, std::function<void(std::string&, std::string&)>> commandMap;
};

#endif // COMMANDMAP_H