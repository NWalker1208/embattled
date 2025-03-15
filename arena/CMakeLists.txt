project(arena LANGUAGES C)

add_executable(${PROJECT_NAME} main.c src/sleep.c)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
