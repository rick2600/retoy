cmake_minimum_required(VERSION 3.10)

project(compiler)

file(GLOB_RECURSE SOURCES *.h *.c)

add_library(compiler STATIC ${SOURCES})

target_include_directories(compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(compiler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
