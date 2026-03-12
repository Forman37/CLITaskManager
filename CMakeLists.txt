cmake_minimum_required(VERSION 4.1)
project(CLITaskManager)

set(CMAKE_CXX_STANDARD 20)

add_executable(CLITaskManager main.cpp
        Task.h
        Task.cpp)
