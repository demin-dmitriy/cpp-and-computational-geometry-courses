cmake_minimum_required(VERSION 2.6)

project(10-minimum_circumscribed_circle)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wconversion -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffloat-store")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

link_libraries(gmp)

include_directories("../lib")

add_library(Geometry "../lib/geometry.cpp" "../lib/geometry_gmp.cpp")

add_executable(minimum_circumscribed_circle main.cpp)

target_link_libraries(minimum_circumscribed_circle Geometry)
