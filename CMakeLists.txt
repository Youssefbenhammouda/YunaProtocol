# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project Name
project(YunaProtocol VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the 'core' directory to the build
add_subdirectory(core)

# Add the 'platforms' directory to the build
# This will contain platform-specific executables/libraries
add_subdirectory(platforms)
add_subdirectory(tests)


