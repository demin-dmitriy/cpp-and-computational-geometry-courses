cmake_minimum_required(VERSION 2.6)

project(13-point_set_delaunay_triangulation)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wconversion -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffloat-store -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories("../lib")

link_libraries(gmp)

add_library(Geometry "../lib/geometry.cpp" "../lib/voronoi_diagram/voronoi_diagram.cpp" "../lib/geometry_gmp.cpp")

add_executable(delaunay_triangulation main.cpp)

target_link_libraries(delaunay_triangulation Geometry)
