project(arena LANGUAGES C)

add_subdirectory(tests)

set(PROJECT_LIB_NAME ${PROJECT_NAME}_lib)

add_library(${PROJECT_LIB_NAME} src/physics.c src/raycast.c src/simulation.c)
target_include_directories(${PROJECT_LIB_NAME} PUBLIC include)
target_link_libraries(${PROJECT_LIB_NAME} PUBLIC processor utilities raylib)

add_executable(${PROJECT_NAME} main.c clay.c)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIB_NAME} parser assembler clay)

enable_testing()
