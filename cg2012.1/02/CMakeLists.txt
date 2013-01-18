cmake_minimum_required(VERSION 2.6)

project(02-segment_rasterization)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wconversion -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffloat-store")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

include_directories("../lib")

add_library(Geometry "../lib/geometry.cpp")

add_executable(segment_rasterization main.cpp)

target_link_libraries(segment_rasterization Geometry)
