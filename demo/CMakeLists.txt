project(demo LANGUAGES C)

include(${CMAKE_SOURCE_DIR}/cmake/EnableWarnings.cmake)

add_executable(
  ${PROJECT_NAME}
  main.c
)

target_link_libraries(${PROJECT_NAME} PUBLIC processor parser assembler)
