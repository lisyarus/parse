set(LEXER_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(LEXER_HEADER ${LEXER_DIR}/lexer.hpp)
set(LEXER_SOURCE ${LEXER_DIR}/lexer.cpp)

add_custom_command(
	OUTPUT ${LEXER_HEADER} ${LEXER_SOURCE}
	COMMAND ${LEXGEN} ${CMAKE_CURRENT_SOURCE_DIR}/lex
	WORKING_DIRECTORY ${LEXER_DIR}
	DEPENDS lexgen lex
)

include_directories(${LEXGEN_INCLUDE_DIRS})
include_directories(${LEXER_DIR})

add_executable(lexgen-test lexgen-test.cpp ${LEXER_HEADER} ${LEXER_SOURCE})
target_link_libraries(lexgen-test ${LEXGEN_LIBRARIES})
