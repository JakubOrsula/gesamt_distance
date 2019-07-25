cmake_minimum_required(VERSION 3.9)
project(gesamt_distance)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")

add_subdirectory(gesamt_src)
add_subdirectory(distance)
