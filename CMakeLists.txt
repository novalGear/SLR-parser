cmake_minimum_required(VERSION 3.14)
project(SLR_Parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")
set(GRAMMAR_DATA_DIR "${CMAKE_SOURCE_DIR}/src/grammar/data")

add_compile_definitions(
    DATA_DIR="${DATA_DIR}"
    GRAMMAR_DATA_DIR="${GRAMMAR_DATA_DIR}"
)

# --- Подключение Google Test ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Подкаталоги
add_subdirectory(src/lexer)
add_subdirectory(src/grammar)
add_subdirectory(src/SLR)
add_subdirectory(tests)
