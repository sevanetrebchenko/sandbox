
# PROJECT FILES
set(CORE_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
        )

add_executable(ascii ${CORE_SOURCE_FILES})

target_include_directories(ascii PUBLIC "${PROJECT_SOURCE_DIR}/include/")

# DEPENDENCIES
# Link OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED) # Ensure OpenGL exists on the system.
message(STATUS "Linking OpenGL to ascii project.")
target_link_libraries(ascii OpenGL::GL)

# Link Glad.
# Glad gives access to both OpenGL extensions and the modern version of the core OpenGL API.
message(STATUS "Linking Glad to ascii project.")
target_link_libraries(ascii ascii-glad)

# Link GLFW
# GLFW does not depend on OpenGL - OpenGL/Vulkan dependencies get loaded in at runtime.
message(STATUS "Linking GLFW to ascii project.")
target_link_libraries(ascii glfw)

# GLM.
message(STATUS "Linking GLM to ascii project.")
target_link_libraries(ascii glm)

# STB.
message(STATUS "Linking STB to ascii project.")
target_link_libraries(ascii ascii-stb)

# ImGui.
message(STATUS "Linking ImGui to ascii project.")
target_link_libraries(ascii ascii-imgui)




