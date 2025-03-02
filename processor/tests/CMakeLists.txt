project(processor-tests LANGUAGES C)

add_executable(process_tests process_tests_Runner.c process_tests.c custom_assertions.c)
target_link_libraries(process_tests PRIVATE unity processor)

add_executable(instruction_tests instruction_tests_Runner.c instruction_tests.c custom_assertions.c)
target_link_libraries(instruction_tests PRIVATE unity processor)

enable_testing()
add_test(NAME process_tests COMMAND process_tests)
add_test(NAME instruction_tests COMMAND instruction_tests)
