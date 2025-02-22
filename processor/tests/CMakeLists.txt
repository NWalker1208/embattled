project(processor-tests LANGUAGES C)

add_executable(${PROJECT_NAME} test_main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE unity processor)

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
