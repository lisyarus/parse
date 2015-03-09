add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/paren/parser.hpp
	COMMAND ${PARSEGEN} < ${CMAKE_CURRENT_SOURCE_DIR}/paren/grammar > ${CMAKE_CURRENT_SOURCE_DIR}/paren/parser.hpp
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/paren
	DEPENDS parsegen ${CMAKE_CURRENT_SOURCE_DIR}/paren/grammar
)

add_executable(test_paren paren/main.cpp paren/parser.hpp)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/eval/parser.hpp
	COMMAND ${PARSEGEN} < ${CMAKE_CURRENT_SOURCE_DIR}/eval/grammar > ${CMAKE_CURRENT_SOURCE_DIR}/eval/parser.hpp
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/eval
	DEPENDS parsegen ${CMAKE_CURRENT_SOURCE_DIR}/eval/grammar
)

add_executable(test_eval eval/main.cpp eval/parser.hpp)
target_link_libraries(test_eval lexer)
