project(parser LANGUAGES C)

add_subdirectory(tests)

add_library(
  ${PROJECT_NAME}
  src/parse.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC processor)
