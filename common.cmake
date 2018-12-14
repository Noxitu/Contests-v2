find_package(GTest REQUIRED)

set(CURRENT_ROUND "")
set(IGNORED_TASKS "")
include("${CMAKE_SOURCE_DIR}/current.cmake" OPTIONAL)

add_compile_options("$<$<BOOL:${MSVC}>:/std:c++17>" "$<$<BOOL:${MSVC}>:/EHs>")

add_library(Distributed_main STATIC "${CMAKE_SOURCE_DIR}/common/distributed.cpp")

macro(round NAME)
    if (CURRENT_ROUND STREQUAL "" OR CURRENT_ROUND STREQUAL "${NAME}")
        add_subdirectory("${NAME}")
    else()
        message(STATUS "Ignoring round \"${NAME}\" since current round is set.")
    endif()
endmacro()

macro(create_task NAME)
    cmake_parse_arguments(CREATE_TASK 
                        "DISTRIBUTED"    # flags
                        ""    # single value options
                        ""    # multi value options 
                        ${ARGN})

    set(TASK_TYPE "")

    if (${CREATE_TASK_DISTRIBUTED})
        set(TASK_TYPE "${TASK_TYPE}DISTRIBUTED")
    endif()

    if (TASK_TYPE STREQUAL "DISTRIBUTED")
        add_executable(${NAME}_run "src/${NAME}.cpp" "src/${NAME}_input.cpp")
        target_include_directories(${NAME}_run PRIVATE "src/provided_headers")
        target_compile_definitions(${NAME}_run PRIVATE main=unused_main)
        target_link_libraries(${NAME}_run PRIVATE Distributed_main)
    endif()

    if (TASK_TYPE STREQUAL "")
        add_executable(${NAME} "src/${NAME}.cpp")

        add_executable(${NAME}_test "src/${NAME}.cpp" "${CMAKE_SOURCE_DIR}/common/test.cpp")
        target_compile_definitions(${NAME}_test PRIVATE main=unused_main TASK_NAME=${NAME} "TASK_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/test/${NAME}\"")
        target_link_libraries(${NAME}_test PRIVATE GTest::Main)
    endif()
endmacro()

macro(task NAME)
    if ("${NAME}" IN_LIST IGNORED_TASKS)
        message(STATUS "Ignoring task \"${NAME}\".")
    elseif (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${NAME}.cpp")
        message(WARNING "Source for task \"${NAME}\" not found.")
    else()
        create_task("${NAME}" ${ARGN})
    endif()
endmacro()