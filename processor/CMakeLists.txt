project(processor LANGUAGES C)

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC include)
