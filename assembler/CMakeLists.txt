project(assembler LANGUAGES C)

include(${CMAKE_SOURCE_DIR}/cmake/EnableWarnings.cmake)

add_subdirectory(tests)

add_library(
  ${PROJECT_NAME}
  src/assemble.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC processor parser)

enable_testing()
