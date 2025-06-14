project(arena LANGUAGES C)

add_subdirectory(tests)

set(PROJECT_LIB_NAME ${PROJECT_NAME}_lib)

set(
  PROJECT_LIB_SOURCES
  src/physics.c
  src/raycast.c
  src/simulation.c
  src/robot.c
  src/timer.c
)
if (NOT EMSCRIPTEN)
  list(APPEND PROJECT_LIB_SOURCES src/worker.c)
endif ()

add_library(${PROJECT_LIB_NAME} ${PROJECT_LIB_SOURCES})
target_include_directories(${PROJECT_LIB_NAME} PUBLIC include)
target_link_libraries(${PROJECT_LIB_NAME} PUBLIC processor utilities raylib)

if (EMSCRIPTEN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sGL_ENABLE_GET_PROC_ADDRESS=1 -Os")
  add_link_options(-sEXPORTED_RUNTIME_METHODS=cwrap)
  add_link_options(-sUSE_GLFW=3 -sASSERTIONS=1 -sWASM=1 -sASYNCIFY)
  add_link_options(-sTOTAL_STACK=64MB -sINITIAL_MEMORY=128MB -sALLOW_MEMORY_GROWTH=1)
  add_link_options(--shell-file ${PROJECT_SOURCE_DIR}/web/shell.html)
  add_link_options(--extern-post-js ${PROJECT_SOURCE_DIR}/web/post.js)
  add_link_options(--preload-file ${PROJECT_SOURCE_DIR}/resources@/resources)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  configure_file("web/coi-serviceworker.js" "${PROJECT_BINARY_DIR}/coi-serviceworker.js" COPYONLY)
  configure_file("web/easm.js" "${PROJECT_BINARY_DIR}/easm.js" COPYONLY)
endif ()

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIB_NAME} parser assembler)

enable_testing()
