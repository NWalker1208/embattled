project(utilities-tests LANGUAGES C)

add_executable(text_tests text_tests_Runner.c text_tests.c)
target_link_libraries(text_tests PRIVATE unity utilities)

enable_testing()
add_test(NAME text_tests COMMAND text_tests)
