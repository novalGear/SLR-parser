cmake_minimum_required(VERSION 3.14)
project(SLR_Parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")
set(GRAMMAR_DATA_DIR "${CMAKE_SOURCE_DIR}/src/grammar/data")

add_compile_definitions(
    DATA_DIR="${DATA_DIR}"
    GRAMMAR_DATA_DIR="${GRAMMAR_DATA_DIR}"
)

add_subdirectory(src/lexer)
add_subdirectory(src/grammar)
add_subdirectory(src/SLR)
