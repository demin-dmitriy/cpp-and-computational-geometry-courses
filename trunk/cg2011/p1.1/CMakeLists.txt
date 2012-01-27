cmake_minimum_required (VERSION 2.6)

project (segment_intersection)

add_definitions(-Wall -W -Wno-sign-compare -std=c++0x)

add_definitions(-O2)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(CMAKE_BUILD_TYPE Release)

add_executable (solution task_p1.1.cpp geometry.cpp)