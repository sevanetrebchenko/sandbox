
find_package(Git REQUIRED)
find_package(PythonInterp REQUIRED)
execute_process(COMMAND ${PYTHON_EXECUTABLE} "${PROJECT_SOURCE_DIR}/lib/shaderc/utils/git-sync-deps")
