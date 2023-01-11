cmake_minimum_required(VERSION 3.25)
project(MARS_3D)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

add_library(MARS_3D SHARED)

target_include_directories(MARS_3D PUBLIC include)

target_link_libraries(MARS_3D MARS)

add_subdirectory(src)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin/engine/)