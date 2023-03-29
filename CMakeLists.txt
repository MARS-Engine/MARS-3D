target_include_directories(MARS PUBLIC include/)

add_subdirectory(src)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin/engine/)