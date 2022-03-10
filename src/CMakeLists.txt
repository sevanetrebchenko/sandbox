
# PROJECT FILES
set(CORE_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
        "${PROJECT_SOURCE_DIR}/src/pch.cpp"

        # Scenes
        "scenes/deferred_rendering/deferred_rendering.cpp"
        "scenes/cs562/project1/project1.cpp"
        "scenes/cs562/project1/light.cpp"
        "scenes/cs562/project2/project2.cpp"

        # Buffer
        "common/api/buffer/buffer.cpp"
        "common/api/buffer/vbo.cpp"
        "common/api/buffer/vao.cpp"
        "common/api/buffer/ebo.cpp"
        "common/api/buffer/ubo.cpp"
        "common/api/buffer/fbo.cpp"
        "common/api/buffer/rbo.cpp"
        "common/api/buffer/vao_manager.cpp"

        # ECS
        "common/ecs/entity/entity_manager.cpp"
        "common/ecs/ecs.cpp"
        "common/ecs/component/component.cpp"
        "common/ecs/component/component_manager.cpp"
        "common/ecs/component/component_list.cpp"
        "common/ecs/system/component_system.cpp"
        "common/ecs/iterator/entity_component_iterator.cpp"

        # Framework
        "common/application/application.cpp"
        "common/application/scene_manager.cpp"
        "common/geometry/topology.cpp"
        "common/api/backend.cpp"
        "common/application/time.cpp"
        "common/application/input.cpp"
        "common/camera/camera.cpp"
        "common/camera/fps_camera.cpp"
        "common/utility/directory.cpp"
        "common/utility/log.cpp"
        "common/geometry/mesh.cpp"
        "common/geometry/model.cpp"
        "common/geometry/model_manager.cpp"
        "common/material/material.cpp"
        "common/material/material_library.cpp"
        "common/geometry/model_manager.cpp"
        "common/geometry/object_loader.cpp"
        "common/application/scene.cpp"
        "common/api/shader/shader.cpp"
        "common/api/shader/shader_types.cpp"
        "common/api/shader/shader_component.cpp"
        "common/api/shader/shader_library.cpp"
        "common/api/shader/shader_uniform.cpp"
        "common/api/shader/shader_preprocessor.cpp"
        "common/api/shader/shader_descriptor.cpp"
        "common/texture/texture.cpp"
        "common/geometry/transform.cpp"
        "common/api/window.cpp"
        "common/texture/texture_library.cpp"
        "common/lighting/lighting_manager.cpp"
        "common/lighting/light.cpp"
        "common/utility/reloadable.cpp"
        )

add_executable(Sandbox ${CORE_SOURCE_FILES})

target_include_directories(Sandbox PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_precompile_headers(Sandbox PRIVATE "${PROJECT_SOURCE_DIR}/include/pch.h")

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

# Shaderc.
message(STATUS "Linking Shaderc to Sandbox project.")
target_link_libraries(Sandbox shaderc)

## SPIRV-Cross.
#message(STATUS "Linking SPIRV-Cross to Sandbox project.")
#target_link_libraries(Sandbox SPIRV-Cross)




