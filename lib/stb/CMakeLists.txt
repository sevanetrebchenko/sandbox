
message(STATUS "Building STB.")

# Add STB as a library.
add_library(ascii-stb INTERFACE)

# Anything that targets stb will need to see the directories for STB includes.
target_include_directories(ascii-stb INTERFACE "${PROJECT_SOURCE_DIR}/lib/stb")