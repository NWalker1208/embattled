project(parser LANGUAGES C)

add_library(
  ${PROJECT_NAME}
  src/parse.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
