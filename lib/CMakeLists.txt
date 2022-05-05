
# Initialize dependencies.

message(STATUS "Building Glad.")
add_subdirectory(glad)
message(STATUS "--------------------")

message(STATUS "Building GLFW.")
set(GLFW_BUILD_DOCS OFF)     # Build GLFW documentation.
set(GLFW_BUILD_TESTS OFF)    # Build GLFW tests.
set(GLFW_BUILD_EXAMPLES OFF) # Build GLFW examples.
add_subdirectory(glfw)
message(STATUS "--------------------")

message(STATUS "Building GLM.")
add_subdirectory(glm)
message(STATUS "--------------------")

message(STATUS "Building ImGui.")
include(imgui_setup)
message(STATUS "--------------------")

message(STATUS "Building STB.")
add_subdirectory(stb)
message(STATUS "--------------------")

message(STATUS "Building TinyOBJLoader.")
add_subdirectory(tinyobj)
message(STATUS "--------------------")

message(STATUS "Building Shaderc.")
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_COPYRIGHT_CHECK ON)
set(SPIRV_HEADERS_SKIP_EXAMPLES ON)
set(SKIP_SPIRV_TOOLS_INSTALL ON)
include(shaderc_setup)
add_subdirectory(shaderc)
message(STATUS "--------------------")

message(STATUS "Building SPIRV-Cross.")
set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS OFF) # Instead of throwing exceptions assert.
set(SPIRV_CROSS_SHARED OFF)                   # Build the C API as a single shared library.
set(SPIRV_CROSS_STATIC ON)                    # Build the C and C++ API as static libraries.
set(SPIRV_CROSS_CLI OFF)                      # Build the CLI binary. Requires SPIRV_CROSS_STATIC.
set(SPIRV_CROSS_ENABLE_TESTS OFF)             # Enable SPIRV-Cross tests.
set(SPIRV_CROSS_ENABLE_GLSL ON)               # Enable GLSL support.
set(SPIRV_CROSS_ENABLE_HLSL OFF)              # Enable HLSL target support.
set(SPIRV_CROSS_ENABLE_MSL OFF)               # Enable MSL target support.
set(SPIRV_CROSS_ENABLE_CPP OFF)               # Enable C++ target support.
set(SPIRV_CROSS_ENABLE_REFLECT OFF)           # Enable JSON reflection target support.
set(SPIRV_CROSS_ENABLE_C_API OFF)             # Enable C API wrapper support in static library.
set(SPIRV_CROSS_ENABLE_UTIL OFF)              # Enable util module support.
set(SPIRV_CROSS_SANITIZE_ADDRESS OFF)         # Sanitize memory addresses.
set(SPIRV_CROSS_SANITIZE_MEMORY OFF)          # Sanitize memory.
set(SPIRV_CROSS_SANITIZE_THREADS OFF)         # Sanitize threads.
set(SPIRV_CROSS_SANITIZE_UNDEFINED OFF)       # Sanitize undefined.
# set(SPIRV_CROSS_NAMESPACE_OVERRIDE "")       # Override the namespace used in the C++ API.
set(SPIRV_CROSS_FORCE_STL_TYPES OFF)          # Force use of STL types instead of STL replacements in certain places. (Note: compilation fails if on).
set(SPIRV_CROSS_SKIP_INSTALL ON)              # Skips installation targets.
set(SPIRV_CROSS_WERROR OFF)                   # Fail build on warnings.
set(SPIRV_CROSS_MISC_WARNINGS OFF)            # Misc warnings useful for Travis runs.
set(SPIRV_CROSS_FORCE_PIC OFF)                # Force position-independent code for all targets.
add_subdirectory(spirv-cross)
message(STATUS "--------------------")
