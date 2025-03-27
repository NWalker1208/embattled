project(utilities LANGUAGES C)

add_subdirectory(tests)

add_library(
  ${PROJECT_NAME}
  src/file.c
  src/sleep.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

enable_testing()
