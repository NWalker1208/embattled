project(demo LANGUAGES C)

add_executable(
  ${PROJECT_NAME}
  main.c
)

target_link_libraries(${PROJECT_NAME} PUBLIC processor parser assembler utilities)
