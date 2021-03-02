
# Initialize dependencies.
# Build glad.
message(STATUS "Building Glad.")
add_subdirectory(glad)

# Build GLFW.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW documentation." FORCE) # Turn off building GLFW documentation.
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests." FORCE)        # Turn off building GLFW tests.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples." FORCE)  # Turn off building GLFW example code.

message(STATUS "Building GLFW.")
add_subdirectory(glfw)

# Build GLM.
message(STATUS "Building GLM.")
add_subdirectory(glm)

# Build ImGui.
message(STATUS "Building ImGui.")
include(imgui_setup IMGUI_SETUP_RESULT)

# Build STB
message(STATUS "Building STB.")
add_subdirectory(stb)

# Build TinyOBJLoader
message(STATUS "Building TinyOBJLoader.")
add_subdirectory(tinyobj)

