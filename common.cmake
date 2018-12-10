find_package(GTest REQUIRED)

macro(task NAME)
    add_executable(${NAME} "src/${NAME}.cpp")

    add_executable(${NAME}_test "src/${NAME}.cpp" "${CMAKE_SOURCE_DIR}/common/test.cpp")
    target_compile_definitions(${NAME}_test PRIVATE main=unused_main TASK_NAME=${NAME} "TASK_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/test/${NAME}\"")
    target_link_libraries(${NAME}_test PRIVATE GTest::Main)
endmacro()