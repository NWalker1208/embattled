cmake_minimum_required(VERSION 3.5.0)
project(Embattled VERSION 0.1.0 LANGUAGES C)

include(FetchContent)

FetchContent_Declare(
  cmocka
  GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git
  GIT_TAG cmocka-1.1.5
  GIT_SHALLOW 1
)

set(WITH_STATIC_LIB ON CACHE BOOL "CMocka: Build with a static library" FORCE)
set(WITH_CMOCKERY_SUPPORT OFF CACHE BOOL "CMocka: Install a cmockery header" FORCE)
set(WITH_EXAMPLES OFF CACHE BOOL "CMocka: Build examples" FORCE)
set(UNIT_TESTING OFF CACHE BOOL "CMocka: Build with unit testing" FORCE)
set(PICKY_DEVELOPER OFF CACHE BOOL "CMocka: Build with picky developer flags" FORCE)

FetchContent_MakeAvailable(cmocka)

add_executable(Embattled src/main.c src/instruction.c src/process.c src/parse.c)
target_include_directories(Embattled PRIVATE include)

add_executable(hello_test tests/main.c)
target_link_libraries(hello_test PRIVATE cmocka-static)

enable_testing()
add_test(NAME hello_test COMMAND hello_test)
