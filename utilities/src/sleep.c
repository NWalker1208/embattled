#include "utilities/sleep.h"

#if defined(WIN32)
#include <windows.h>
#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>
#else
#include <unistd.h>
#endif

void psleep(unsigned int durationMs) {
  #if defined(WIN32)
  Sleep(durationMs);
  #elif defined(__EMSCRIPTEN__)
  emscripten_sleep(durationMs);
  #else
  usleep(durationMs * 1000);
  #endif
}
