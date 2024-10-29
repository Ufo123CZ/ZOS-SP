#ifndef COMMANDS_H
#define COMMANDS_H

#define COMAND_PREFIX1 "~"
#define COMAND_PREFIX2 "$"

#define ROOT_CLUSTER (0)
#define ROOT_DIRECTORY ("/")

#include <string>
#include <cstdint>

namespace Cp {
    std::string copyFile(std::string& source, std::string& dest);
}

namespace Mv {
    std::string moveFile(std::string& source, std::string& dest);
}

namespace Rm {
    std::string removeFile(std::string& path);
}

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

namespace Outcp {
    std::string copyFileOutput(std::string& source, std::string& dest);
}

namespace Load {
    std::string loadCommands(std::string& commandFile);
}

namespace Format {
    std::string formatFile(std::string& size);
}

// Additional commands
namespace Help {
    void writeHelpInConsole();
}

// Additional required commands
namespace BugCheck {
    void checkForBugs();
}

namespace BugCreator {
    std::string createBug(std::string& path);
}

#endif //COMMANDS_H