project(assembler LANGUAGES C)

add_subdirectory(tests)

add_library(
  ${PROJECT_NAME}
  src/assembly.c
  src/assemble.c
  src/binary.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC utilities processor)

enable_testing()
