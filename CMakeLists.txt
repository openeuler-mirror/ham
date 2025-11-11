cmake_minimum_required(VERSION 3.16)
project(HAM CXX C)

set(CMAKE_CXX_STANDARD 11)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/output)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXE_LINKER_FLAGS "-pie")
set(CMAKE_SKIP_RPATH TRUE)

set(PROJECT_3RDPARTY_SRC_DIR ${PROJECT_SOURCE_DIR}/3rdparty)

include("cmake/building.cmake")

include_directories(${PROJECT_3RDPARTY_SRC_DIR}/libboundscheck/include)

if (${DEBUG_UT})
    add_compile_options(-ftest-coverage -fprofile-arcs)
    add_compile_definitions(DEBUG_UT)
    link_libraries(gcov)
    add_compile_options(-fno-inline)
endif()

add_subdirectory(src)