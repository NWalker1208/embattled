project(assembler-tests LANGUAGES C)

add_executable(assemble_tests assemble_tests_Runner.c assemble_tests.c)
target_link_libraries(assemble_tests PRIVATE unity test_utilities assembler parser)

enable_testing()
add_test(NAME assemble_tests COMMAND assemble_tests)
