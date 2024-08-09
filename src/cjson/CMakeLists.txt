cmake_minimum_required(VERSION 3.10)

project(cjson)

add_library(cjson STATIC cJSON.c cJSON.h)

target_include_directories(cjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(cjson INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})