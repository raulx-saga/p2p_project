cmake_minimum_required(VERSION 3.15)
project(P2PFileSharer VERSION 1.0 LANGUAGES CXX)

# Force C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prevent CMake from cluttering your source directory with build files
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Define your executable and the source files it needs
add_executable(p2p_app main.cpp)