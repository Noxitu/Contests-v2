cmake_minimum_required(VERSION 3.0.0)
project(Contests VERSION 0.1.0)

set(CMAKE_PREFIX_PATH "D:/Sources/3rdparties/ninja-install/all" CACHE filepath "")

include(common.cmake)

add_compile_options("$<$<BOOL:${MSVC}>:/std:c++17>")

add_subdirectory("Potyczki Algorytmiczne 2018")
