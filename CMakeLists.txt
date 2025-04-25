cmake_minimum_required(VERSION 3.14)

project(Shell)

include_directories(include/)
file(GLOB SHELL_SOURCES src/*.c)

add_executable(shell ${SHELL_SOURCES})

target_link_libraries(shell m)


