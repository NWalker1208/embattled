project(parser LANGUAGES C)

include(${CMAKE_SOURCE_DIR}/cmake/EnableWarnings.cmake)

add_subdirectory(tests)

add_library(
  ${PROJECT_NAME}
  src/assembly.c
  src/parse.c
  src/utilities.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC processor)

enable_testing()
