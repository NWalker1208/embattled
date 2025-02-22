project(processor-tests LANGUAGES C)

add_executable(process_tests process_tests.c)
target_link_libraries(process_tests PRIVATE unity processor)

enable_testing()
add_test(NAME process_tests COMMAND process_tests)
