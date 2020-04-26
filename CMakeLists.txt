project(TheGame)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/text.txt ${CMAKE_CURRENT_BINARY_DIR}/text.txt COPYONLY)

add_executable(TheGame main.cpp)
add_executable(GameTests gametest.cpp thegame.hpp)


