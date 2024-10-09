#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>

namespace Format {
    std::string formatFile(std::string& filename, std::string& size);
}

namespace MkDir {
    std::string makeDirectory(std::string& filename, std::string& dirname, int32_t currentCluster);
}

namespace Cd {
    std::pair<std::string, int32_t> changeDirectory(std::string& filename, std::string& currentDirectory, std::string& dirname, int32_t currentCluster);
}

namespace Help {
    void writeHelpInConsole();
}
#endif //COMMANDS_H