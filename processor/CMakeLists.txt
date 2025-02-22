project(processor LANGUAGES C)

add_subdirectory(tests)

add_library(
  ${PROJECT_NAME}
  src/instruction.c
  src/process.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

enable_testing()
