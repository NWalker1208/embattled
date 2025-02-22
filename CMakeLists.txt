cmake_minimum_required(VERSION 3.5.0)
project(Embattled VERSION 0.1.0)

include(cmake/FetchUnity.cmake)

add_subdirectory(processor)
add_subdirectory(parser)

enable_testing()
