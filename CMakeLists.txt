cmake_minimum_required(VERSION 3.5.0)
project(Embattled VERSION 0.1.0 LANGUAGES C)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/6910c9d9165801d8827d628cb72eb7ea9dd538c5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(Embattled src/main.c src/instruction.c src/process.c src/parse.c)
target_include_directories(Embattled PRIVATE include)

include(CTest)
enable_testing()

