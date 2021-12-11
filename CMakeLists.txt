cmake_minimum_required(VERSION 3.0)

# Project information.
project(Sandbox
        VERSION 1.0
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# Set directory for .cmake includes.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#include(submodule_setup)

# Build third-party dependencies.
message(STATUS "Adding library subdirectory")
add_subdirectory(lib)

# Build souce.
message(STATUS "Adding source subdirectory")
add_subdirectory(src)






