project(arena LANGUAGES C)

add_subdirectory(tests)

set(PROJECT_LIB_NAME ${PROJECT_NAME}_lib)

add_library(${PROJECT_LIB_NAME} src/physics.c src/raycast.c src/simulation.c src/robot.c)
target_include_directories(${PROJECT_LIB_NAME} PUBLIC include)
target_link_libraries(${PROJECT_LIB_NAME} PUBLIC processor utilities raylib)

if (EMSCRIPTEN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sGL_ENABLE_GET_PROC_ADDRESS=1 -pthread")
  add_link_options(-sUSE_GLFW=3 -sASSERTIONS=1 -sWASM=1 -sASYNCIFY)
  add_link_options(-sPTHREAD_POOL_SIZE=3)
  add_link_options(--shell-file ${PROJECT_SOURCE_DIR}/web/shell.html)
  add_link_options(--preload-file ${PROJECT_SOURCE_DIR}/resources)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  configure_file("web/coi-serviceworker.js" "${PROJECT_BINARY_DIR}/coi-serviceworker.js" COPYONLY)
endif ()

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIB_NAME} parser assembler)

enable_testing()
