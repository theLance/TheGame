project(TheGame)

cmake_minimum_required(VERSION 3.15)

file(GLOB LIST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.txt")
foreach(List IN LISTS LIST_FILES)
    configure_file(${List} ${CMAKE_CURRENT_BINARY_DIR}/${List})
endforeach()

add_executable(TheGame main.cpp)
add_executable(GameTests gametest.cpp thegame.hpp)

add_library(GameLib STATIC thegame.cpp)

