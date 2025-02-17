cmake_minimum_required(VERSION 3.5.0)
project(Embattled VERSION 0.1.0 LANGUAGES C)

add_executable(Embattled src/main.c src/parse.c)
target_include_directories(Embattled PRIVATE include)

include(CTest)
enable_testing()

