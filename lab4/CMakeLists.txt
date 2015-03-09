cmake_minimum_required(VERSION 2.8)
project(lab4)

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")

add_executable(lexgen lexgen.cpp io_utils.h)
add_executable(parsegen parsegen.cpp io_utils.h)

find_package(Boost COMPONENTS regex REQUIRED)

get_target_property(LEXGEN lexgen LOCATION)
set(LEXGEN_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
set(LEXGEN_LIBRARIES ${Boost_REGEX_LIBRARY})

get_target_property(PARSEGEN parsegen LOCATION)

add_subdirectory(lexgen-test)
add_subdirectory(parsegen-test)
