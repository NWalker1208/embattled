project(arena LANGUAGES C)


include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.5
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)


include(${CMAKE_SOURCE_DIR}/cmake/EnableWarnings.cmake)

add_executable(${PROJECT_NAME} main.c src/sleep.c)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
