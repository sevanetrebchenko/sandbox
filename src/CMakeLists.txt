
# PROJECT FILES
set(CORE_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
        "${PROJECT_SOURCE_DIR}/src/sandbox_pch.cpp"

        # Scenes
        "${PROJECT_SOURCE_DIR}/src/scenes/deferred_rendering.cpp"

        # Buffer
        "${PROJECT_SOURCE_DIR}/src/framework/buffer/buffer.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/buffer/vbo.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/buffer/vao.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/buffer/ebo.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/buffer/ubo.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/buffer/fbo.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/buffer/rbo.cpp"

        # Framework
        "${PROJECT_SOURCE_DIR}/src/framework/backend.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/camera.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/directory_utils.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/imgui_log.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/mesh.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/model.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/model_manager.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/material.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/material_library.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/model_manager.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/object_loader.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/scene.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/shader.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/shader_data_types.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/shader_library.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/shader_uniform.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/texture.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/transform.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/window.cpp"
        )

add_executable(Sandbox ${CORE_SOURCE_FILES})

target_include_directories(Sandbox PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_precompile_headers(Sandbox PRIVATE "${PROJECT_SOURCE_DIR}/include/sandbox_pch.h")

# DEPENDENCIES
# Link OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED) # Ensure OpenGL exists on the system.
message(STATUS "Linking OpenGL to ascii project.")
target_link_libraries(Sandbox OpenGL::GL)

# Link Glad.
# Glad gives access to both OpenGL extensions and the modern version of the core OpenGL API.
message(STATUS "Linking Glad to Sandbox project.")
target_link_libraries(Sandbox glad)

# Link GLFW
# GLFW does not depend on OpenGL - OpenGL/Vulkan dependencies get loaded in at runtime.
message(STATUS "Linking GLFW to Sandbox project.")
target_link_libraries(Sandbox glfw)

# GLM.
message(STATUS "Linking GLM to Sandbox project.")
target_link_libraries(Sandbox glm)

# STB.
message(STATUS "Linking STB to Sandbox project.")
target_link_libraries(Sandbox stb)

# TinyObjLoader.
message(STATUS "Linking TinyObjLoader to Sandbox project.")
target_link_libraries(Sandbox tinyobjloader)

# ImGui.
message(STATUS "Linking ImGui to Sandbox project.")
target_link_libraries(Sandbox imgui)




