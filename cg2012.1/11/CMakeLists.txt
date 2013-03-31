cmake_minimum_required(VERSION 2.6)

project(11-point_set_diameter)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wconversion -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffloat-store")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories("../lib")

link_libraries(gmp)

add_library(Geometry "../lib/geometry/geometry.cpp")

add_executable(point_set_diameter main.cpp)

target_link_libraries(point_set_diameter Geometry)
