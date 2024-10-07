cmake_minimum_required(VERSION 3.25)
project(File_System)

set(CMAKE_CXX_STANDARD 23)

add_executable(File_System
        src/main.cpp
        src/FileCheck/FileCheck.cpp
)
