project(parser-tests LANGUAGES C)

add_executable(parse_tests parse_tests_Runner.c parse_tests.c)
target_link_libraries(parse_tests PRIVATE unity test_utilities parser)

enable_testing()
add_test(NAME parse_tests COMMAND parse_tests)
