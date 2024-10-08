#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>

namespace Format {
    std::string formatFile(std::string& filename, std::string& size);
}

namespace MkDir {
    std::string makeDirectory(std::string& filename, std::string& dirname);
}

namespace Cd {
    std::string changeDirectory(std::string& filename, std::string& dirname);
}

namespace Help {
    void writeHelpInConsole();
}
#endif //COMMANDS_H