cmake_minimum_required(VERSION 3.10)

project(vm)

file(GLOB_RECURSE SOURCES *.h *.c)

add_library(vm STATIC ${SOURCES})

target_include_directories(vm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(vm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})