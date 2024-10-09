#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>



namespace MkDir {
    std::string makeDirectory(std::string& filename, std::string& dirname, int32_t currentCluster);
}

namespace RmDir {
    std::string removeDirectory(std::string& filename, std::string& dirname, int32_t currentCluster);
}

namespace Ls {
    std::string listDirectory(std::string& filename, std::string& path);
}

namespace Cd {
    std::pair<std::string, int32_t> changeDirectory(std::string& filename, std::string& currentDirectory, std::string& dirname, int32_t currentCluster);
}

namespace Format {
    std::string formatFile(std::string& filename, std::string& size);
}

namespace Help {
    void writeHelpInConsole();
}

#endif //COMMANDS_H