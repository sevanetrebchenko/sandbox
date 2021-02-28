# Add Glad as a library.
add_library(ascii-glad STATIC src/glad.c ../stb/stb_image.h)

# Set includes for targets using library.
set(PROJECT_INCLUDE_DIRS
        "${PROJECT_SOURCE_DIR}/lib/glad/include"
        )

# Any targets that link to the 'CS350-glad' target will also need the 'glad' include files directory.
target_include_directories(ascii-glad PUBLIC "${PROJECT_INCLUDE_DIRS}")
