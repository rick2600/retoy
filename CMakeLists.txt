cmake_minimum_required(VERSION 3.10)

project(retoy)


# Default build type (if not specified)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()


add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -B ${CMAKE_BINARY_DIR}/release-build -S ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/release-build
    COMMENT "Release Building"
)

add_custom_target(asan
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Asan -B ${CMAKE_BINARY_DIR}/asan-build -S ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/asan-build
    COMMENT "ASAN Building"
)

add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -B ${CMAKE_BINARY_DIR}/debug-build -S ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/debug-build
    COMMENT "DEBUG Building"
)

add_custom_target(build_all
    DEPENDS release asan debug
    COMMENT "Building all configurations: Release, ASan, and Debug"
)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Asan")
    message(STATUS "AddressSanitizer enabled")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fsanitize=address")
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug enabled")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -ggdb")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb")
endif()



add_subdirectory(src)
add_subdirectory(src/compiler)
add_subdirectory(src/vm)
add_subdirectory(src/cjson)
add_subdirectory(src/utils)



