cmake_minimum_required(VERSION 3.10)
project(VEDirectLib)

add_library(VEDirect STATIC VEDirect.cpp)
target_include_directories(VEDirect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})