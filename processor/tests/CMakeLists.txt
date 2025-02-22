project(processor-tests LANGUAGES C)

add_executable(test-processor test_main.c)

target_link_libraries(test-processor PRIVATE processor)

enable_testing()
add_test(NAME test-01 COMMAND test-processor test_01.bin 10)
