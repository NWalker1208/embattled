project(parser LANGUAGES C)

add_subdirectory(tests)

add_library(
  ${PROJECT_NAME}
  src/parse.c
  src/text_utils.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC utilities processor assembler)

enable_testing()
