cmake_minimum_required(VERSION 3.5.0)
project(Embattled VERSION 0.1.0)

include(cmake/FetchUnity.cmake)

add_subdirectory(processor)
add_subdirectory(parser)
add_subdirectory(assembler)
add_subdirectory(demo)
add_subdirectory(arena)

enable_testing()
