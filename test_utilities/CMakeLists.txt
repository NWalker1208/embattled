project(test_utilities LANGUAGES C)

add_library(
  ${PROJECT_NAME}
  src/text_assertions.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC unity utilities)
