cmake_minimum_required(VERSION 3.10)

project(utils)

file(GLOB_RECURSE SOURCES *.h *.c)

add_library(utils STATIC ${SOURCES})

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(utils PUBLIC cjson)
