cmake_minimum_required(VERSION 3.14)
project(SLR_Parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем поддиректории
add_subdirectory(src/grammar)
add_subdirectory(src/SLR)
