
# Prevent clearing of CMake variables (exists for backwards compatibility).
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Initialize dependencies.

# Build glad.
message(STATUS "Building Glad.")
add_subdirectory(glad)
message(STATUS "--------------------")

# Build GLFW.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW documentation." FORCE) # Turn off building GLFW documentation.
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests." FORCE)        # Turn off building GLFW tests.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples." FORCE)  # Turn off building GLFW example code.

message(STATUS "Building GLFW.")
add_subdirectory(glfw)
message(STATUS "--------------------")

# Build GLM.
message(STATUS "Building GLM.")
add_subdirectory(glm)
message(STATUS "--------------------")

# Build ImGui.
message(STATUS "Building ImGui.")
include(imgui_setup IMGUI_SETUP_RESULT)
message(STATUS "--------------------")

# Build STB
message(STATUS "Building STB.")
add_subdirectory(stb)
message(STATUS "--------------------")

# Build TinyOBJLoader.
message(STATUS "Building TinyOBJLoader.")
add_subdirectory(tinyobj)
message(STATUS "--------------------")

# Build Shaderc.
message(STATUS "Building Shaderc.")
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_COPYRIGHT_CHECK ON)
set(SPIRV_HEADERS_SKIP_EXAMPLES ON)
set(SKIP_SPIRV_TOOLS_INSTALL ON)
add_subdirectory(shaderc)
message(STATUS "--------------------")

# Build SPIRV-Cross.
#message(STATUS "Building SPIRV-Cross.")
#add_subdirectory(spirv-cross)
#message(STATUS "--------------------")