cmake_minimum_required(VERSION 3.10)

project(retoy)

add_executable(retoy main.c re.c)

target_link_libraries(retoy PRIVATE compiler vm cjson utils)
