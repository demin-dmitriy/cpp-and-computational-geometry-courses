cmake_minimum_required(VERSION 2.6)

project(01-convex_hull)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffloat-store")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

include_directories("../lib")

add_library(Geometry "../lib/geometry.cpp")

add_executable(convex_hull main.cpp)

target_link_libraries(convex_hull Geometry)
