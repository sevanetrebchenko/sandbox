
message(STATUS "Building STB.")

# Add STB as a library.
add_library(stb STATIC "${PROJECT_SOURCE_DIR}/lib/stb/src/stb_image.cpp")

# Anything that targets stb will need to see the directories for STB includes.
target_include_directories(stb PUBLIC "${PROJECT_SOURCE_DIR}/lib/stb/include")