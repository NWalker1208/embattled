project(processor LANGUAGES C)

include(${CMAKE_SOURCE_DIR}/cmake/EnableWarnings.cmake)

add_subdirectory(tests)

add_library(
  ${PROJECT_NAME}
  src/opcode.c
  src/register.c
  src/instruction.c
  src/process.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

enable_testing()
