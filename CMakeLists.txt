cmake_minimum_required(VERSION 2.8.11)

project(compare)
file(GLOB_RECURSE compare_sources ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file(GLOB_RECURSE compare_headers ${CMAKE_CURRENT_SOURCE_DIR} include/compare/*.hpp)

add_executable(compare ${compare_sources} ${compare_headers})

target_include_directories(compare PUBLIC include PRIVATE src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
