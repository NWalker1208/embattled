#include "arena/sleep.h"

#if defined(_MSC_VER)
#include <windows.h>
#else
#include <unistd.h>
#endif

void psleep(unsigned int durationMs) {
  #if defined(_MSC_VER)
  Sleep(durationMs);
  #else
  usleep(durationMs * 1000);
  #endif
}
