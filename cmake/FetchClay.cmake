include(FetchContent)

FetchContent_Declare(
  clay
  GIT_REPOSITORY https://github.com/nicbarker/clay.git
  GIT_TAG v0.13
  GIT_SHALLOW TRUE
)

set(CLAY_INCLUDE_ALL_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(clay)

add_library(clay INTERFACE)
target_include_directories(clay INTERFACE ${clay_SOURCE_DIR})
