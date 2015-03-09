set(DIR ${CMAKE_CURRENT_BINARY_DIR})

set(LEXER_HEADER ${DIR}/lexer.hpp)
set(LEXER_SOURCE ${DIR}/lexer.cpp)

set(PARSER_HEADER ${DIR}/parser.hpp)
set(PARSER_SOURCE ${DIR}/parser.cpp)

add_custom_command(
	OUTPUT ${LEXER_HEADER} ${LEXER_SOURCE}
	COMMAND ${LEXGEN} ${CMAKE_CURRENT_SOURCE_DIR}/lex
	WORKING_DIRECTORY ${DIR}
	DEPENDS lexgen lex
)

add_custom_command(
	OUTPUT ${PARSER_HEADER} ${PARSER_SOURCE}
	COMMAND ${PARSEGEN} ${CMAKE_CURRENT_SOURCE_DIR}/parse
	WORKING_DIRECTORY ${DIR}
	DEPENDS parsegen parse
)

include_directories(${LEXGEN_INCLUDE_DIRS})
include_directories(${DIR})

add_executable(parsegen-test parsegen-test.cpp ${LEXER_HEADER} ${LEXER_SOURCE} ${PARSER_HEADER} ${PARSER_SOURCE})
target_link_libraries(parsegen-test ${LEXGEN_LIBRARIES})
