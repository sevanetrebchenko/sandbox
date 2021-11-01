# Set Sources and Header files.
set(SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/lib/implot/implot.cpp"
        "${PROJECT_SOURCE_DIR}/lib/implot/implot_demo.cpp"
        "${PROJECT_SOURCE_DIR}/lib/implot/implot_items.cpp"
        )

set(IMPLOT_INCLUDE_DIRS
        "${PROJECT_SOURCE_DIR}/lib/implot"
        )

# Add ImGui as a library.
add_library(implot STATIC "${SOURCE_FILES}")

# Anything that targets ImGui will need to see the directories for ImGui includes.
target_include_directories(implot PUBLIC "${IMPLOT_INCLUDE_DIRS}")

target_link_libraries(implot PUBLIC imgui)

