project(parser-tests LANGUAGES C)

add_executable(parse_tests parse_tests_Runner.c parse_tests.c text_assertions.c)
target_link_libraries(parse_tests PRIVATE unity parser)

enable_testing()
add_test(NAME parse_tests COMMAND parse_tests)
