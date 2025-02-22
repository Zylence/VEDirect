cmake_minimum_required(VERSION 3.10)
project(VEDirectTests)

# gtest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# add tests
add_executable(runTests 
    integration/test_VEDirect.cpp 
    ../src/VEDirect.cpp
)

target_include_directories(runTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mocks)
target_include_directories(runTests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# link gtest
target_link_libraries(runTests PRIVATE gtest gtest_main)

# register tests
add_test(NAME VEDirectTests COMMAND runTests)
