cmake_minimum_required(VERSION 2.6)

project(18-voronoi_diagram_cgal)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wconversion -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffloat-store")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

link_libraries(boost_system-mgw47-mt-1_50 CGAL)

if (additional_paths)
    message(STATUS "Setting addtional paths")
    link_directories("C:/Program Files (x86)/CGAL-4.1/build/lib")
    link_directories("D:/stuff/tools/boost_1_50_0/stage/lib")
    include_directories("C:/Program Files (x86)/CGAL-4.1/include")
    include_directories("D:/stuff/tools/boost_1_50_0")
endif()

add_executable(voronoi_diagram_cgal main.cpp)

