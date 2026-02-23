find_package(FLEX REQUIRED)

FLEX_TARGET(LexerGenerator lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
    COMPILE_FLAGS "--c++")

set(LEXER_SOURCES
    lexer_global.cpp
    ${FLEX_LexerGenerator_OUTPUTS}
)

add_library(lexer_lib STATIC ${LEXER_SOURCES})

target_include_directories(lexer_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(lexer_lib PUBLIC fl)
