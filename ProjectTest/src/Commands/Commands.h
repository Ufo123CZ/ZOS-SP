#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <cstdint>


namespace  MkDir {
    std::string makeDirectory(std::string& dirname);
}

namespace RmDir {
    std::string removeDirectory(std::string& path);
}

namespace Ls {
    std::string listDirectory(std::string& path);
}

namespace Cat {
    void catFile(std::string& path);
}

namespace Cd {
    void changeDirectory(std::string& path);
}

namespace Info {
    std::string fileInfo(std::string& path);
}

namespace Incp {
    std::string copyFileInput(std::string& source, std::string& dest);
}

namespace Load {
    std::string loadCommands(std::string& commandFile);
}

namespace Format {
    std::string formatFile(std::string& size);
}

namespace Help {
    void writeHelpInConsole();
}

#endif //COMMANDS_H