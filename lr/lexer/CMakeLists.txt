find_package(Boost COMPONENTS regex REQUIRED)

add_library(lexer STATIC include/lexer/lexer.hpp source/lexer.cpp)
target_link_libraries(lexer ${Boost_REGEX_LIBRARY})
target_include_directories(lexer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(lexer-test test/test.cpp)
target_link_libraries(lexer-test lexer)
