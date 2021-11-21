
# PROJECT FILES
set(CORE_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
        "${PROJECT_SOURCE_DIR}/src/sandbox_pch.cpp"

        # Scenes
        "${PROJECT_SOURCE_DIR}/src/scenes/deferred_rendering/deferred_rendering.cpp"
        "${PROJECT_SOURCE_DIR}/src/scenes/project1/project1.cpp"
        "${PROJECT_SOURCE_DIR}/src/scenes/project2/project2.cpp"
        "${PROJECT_SOURCE_DIR}/src/scenes/project3/project3.cpp"

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
        "${PROJECT_SOURCE_DIR}/src/framework/texture_library.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/lighting_manager.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/light.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/skinned_mesh.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/assimp_loader.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/assimp_helper.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/bone.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/animation.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/animator.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/animated_model.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/ubo_manager.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/quaternion.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/vqs.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/interpolation.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/debug.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/path.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/pather.cpp"
        "${PROJECT_SOURCE_DIR}/src/framework/ik.cpp"
        )

add_executable(Sandbox ${CORE_SOURCE_FILES})

target_include_directories(Sandbox PUBLIC "${PROJECT_SOURCE_DIR}/include/")

if(MSVC)
    # Set Visual studio working directory.
    set_property(TARGET Sandbox PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/build")

    # Copy over assets and data directories.
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
    file(COPY "${PROJECT_SOURCE_DIR}/assets" DESTINATION "${PROJECT_SOURCE_DIR}/build")
    file(COPY "${PROJECT_SOURCE_DIR}/data" DESTINATION "${PROJECT_SOURCE_DIR}/build")
endif()

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

# Assimp
message(STATUS "Linking Assimp to Sandbox project.")
target_link_libraries(Sandbox assimp)

# ImGui.
message(STATUS "Linking ImGui to Sandbox project.")
target_link_libraries(Sandbox imgui)

# ImPlot.
message(STATUS "Linking ImPlot to Sandbox project.")
target_link_libraries(Sandbox implot)




