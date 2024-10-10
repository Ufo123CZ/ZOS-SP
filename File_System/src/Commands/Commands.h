#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>



namespace  MkDir {
    std::string makeDirectory(std::string& dirname);
}

namespace RmDir {
    std::string removeDirectory(std::string& filename, std::string& dirname, int32_t currentCluster);
}

namespace Ls {
    std::string listDirectory(std::string& path);
}

namespace Cd {
    void changeDirectory(std::string& path);
}

namespace Format {
    std::string formatFile(std::string& filename, std::string& size);
}

namespace Help {
    void writeHelpInConsole();
}

#endif //COMMANDS_H