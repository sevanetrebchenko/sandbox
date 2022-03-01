
find_package(Git REQUIRED)

option(SKIP_GIT_SUBMODULE_UPDATE "Update submodules during build" ${SKIP_GIT_SUBMODULE_UPDATE})
if(NOT ${SKIP_GIT_SUBMODULE_UPDATE})
    message(STATUS "Updating project dependencies to latest versions.")
    set(GIT_SUBMODULE_UPDATE ON)
else()
    message(STATUS "Skipping project dependency update.")
endif()

if(GIT_SUBMODULE_UPDATE)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive # git submodule update --init --recursive
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            RESULT_VARIABLE SUBMODULE_FETCH_RESULT)
    # Check the return value of the above command to make sure it executed properly.
    if(NOT SUBMODULE_FETCH_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${SUBMODULE_FETCH_RESULT}.")
    endif()
endif()
