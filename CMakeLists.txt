cmake_minimum_required(VERSION 3.0)

# Project information.
project(Sandbox
        # VERSION 1.0
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# Prevent clearing of CMake variables (exists for backwards compatibility).
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Set directory for .cmake includes.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(SKIP_GIT_SUBMODULE_UPDATE ON)
include(submodule_setup)

# Build third-party dependencies.
message(STATUS "Adding library subdirectory")
add_subdirectory(lib)

# Build souce.
message(STATUS "Adding source subdirectory")
add_subdirectory(src)






