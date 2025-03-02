# Embattled

## Requirements

- CMake

## Regenerating test runners

TODO: Integrate generate_test_runner into CMake setup.

```sh
ruby ./build/_deps/unity-src/auto/generate_test_runner.rb ./processor/tests/process_tests.c ./processor/tests/process_tests_Runner.c
ruby ./build/_deps/unity-src/auto/generate_test_runner.rb ./processor/tests/instruction_tests.c ./processor/tests/instruction_tests_Runner.c
```
