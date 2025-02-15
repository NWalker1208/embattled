cmake_minimum_required(VERSION 3.5.0)
project(Embattled VERSION 0.1.0 LANGUAGES C)

add_executable(Embattled src/main.c)

include(CTest)
enable_testing()

