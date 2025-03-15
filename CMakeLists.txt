cmake_minimum_required(VERSION 3.5.0)
project(Embattled VERSION 0.1.0)

# Configure C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Fetch dependencies
include(cmake/FetchRaylib.cmake)
include(cmake/FetchUnity.cmake)

# Configure compiler warnings globally
if(CMAKE_COMPILER_IS_GNUCC)
  add_compile_options(-Wall -Wextra)
elseif(CMAKE_COMPILER_IS_MSVC)
  add_compile_options(/W4)
elseif(CMAKE_COMPILER_IS_CLANG)
  add_compile_options(-Wall -Wextra)
endif()

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_LINK_WARNING_AS_ERROR ON)

# Add subprojects
add_subdirectory(processor)
add_subdirectory(parser)
add_subdirectory(assembler)
add_subdirectory(demo)
add_subdirectory(arena)

# Configure testing
enable_testing()
