cmake_minimum_required(VERSION 2.8.12)
project(lr-parser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

add_subdirectory(lexer)
add_subdirectory(parsegen)

get_target_property(PARSEGEN parsegen LOCATION)

add_subdirectory(test)
