cmake_minimum_required(VERSION 3.25)
project(File_System)

set(CMAKE_CXX_STANDARD 23)

add_executable(File_System
        # Main, FileCheck and Utils
        src/main.cpp
        src/FileCheck/FileCheck.cpp
        src/Utils/Utils.cpp
        src/Utils/FAT.cpp

        # Commands
#        src/Commands/Cp.cpp
#        src/Commands/Mv.cpp
#        src/Commands/Rm.cpp
        src/Commands/MkDir.cpp
#        src/Commands/RmDir.cpp
        src/Commands/Ls.cpp
#        src/Commands/Cat.cpp
        src/Commands/Cd.cpp
#        src/Commands/Pwd.cpp
#        src/Commands/Info.cpp
#        src/Commands/Incp.cpp
#        src/Commands/Outcp.cpp
#        src/Commands/Load.cpp
        src/Commands/Format.cpp
        src/Commands/Help.cpp

        src/Utils/Utils.h
        src/Utils/Items.h
        src/Utils/FAT.h
        src/FileCheck/FileCheck.h
        src/Commands/Commands.h
)
