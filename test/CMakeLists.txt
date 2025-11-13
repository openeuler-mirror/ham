cmake_minimum_required(VERSION 3.13)

project(HAM VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS " ${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -DDEBUG_UT")
add_link_options(-lgcov --coverage)
set(CMAKE_EXE_LINKER_FLAGS " -lgcov --coverage ")
set(CMAKE_SHARED_LINKER_FLAGS " -lgcov --coverage ")

execute_process(
    COMMAND make -j4
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/libboundscheck
)

add_subdirectory(3rdparty)

# test cases source files
aux_source_directory(cases TEST_SRC)
aux_source_directory(../src TEST_SRC)

# huawei_secure_c objects files
file(GLOB_RECURSE SECURE_OBJS ../3rdparty/libboundscheck/obj/*.o)

add_executable(ham_dt ham_main.cpp ${TEST_SRC} ${SECURE_OBJS})

TARGET_MOCKCPP(ham_dt)
TARGET_GTEST(ham_dt)

# include headers
target_include_directories(ham_dt PUBLIC
        ../src
        ../3rdparty/libboundscheck/include
        3rdparty/googletest/googletest/include
)

get_target_property(COMPILE_FLAGS ham_dt COMPILE_OPTIONS)
get_target_property(LINK_FLAGS ham_dt LINK_OPTIONS)
message(STATUS "Compiler id: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compile flags: ${COMPILE_FLAGS}")
message(STATUS "Link flags: ${LINK_FLAGS}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")