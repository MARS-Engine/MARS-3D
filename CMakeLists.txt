cmake_minimum_required(VERSION 3.25)
project(MVRE_3D)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

add_library(MVRE_3D STATIC)

target_include_directories(MVRE_3D PUBLIC include)

target_link_libraries(MVRE_3D MVRE)

add_subdirectory(src)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin/engine/)