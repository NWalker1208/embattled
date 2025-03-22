#include "utilities/sleep.h"

#if defined(WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

void psleep(unsigned int durationMs) {
  #if defined(WIN32)
  Sleep(durationMs);
  #else
  usleep(durationMs * 1000);
  #endif
}
