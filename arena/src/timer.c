#include "arena/timer.h"

Timer InitTimer(int64_t ticksPerSec, int64_t maxTicks) {
  return (Timer){
    .ticksPerSec = ticksPerSec,
    .maxTicks = maxTicks,
    ._lastClock = clock(),
    ._lastTicks = 0,
    ._lastRemainder = 0,
  };
}

int64_t GetTimerTicks(Timer* timer) {
  int64_t ticksPerSec = timer->ticksPerSec;
  if (ticksPerSec < 0) { ticksPerSec = 0; }

  clock_t currentClock = clock();
  clock_t elapsedClocks = currentClock - timer->_lastClock;
  if (elapsedClocks < 0) { elapsedClocks = 0; }
  clock_t maxElapsedClocks = (INT64_MAX - timer->_lastRemainder) / ticksPerSec;
  if (elapsedClocks > maxElapsedClocks) { elapsedClocks = maxElapsedClocks; }

  int64_t elapsedTicksClocksPerSec = elapsedClocks * ticksPerSec + timer->_lastRemainder;
  int64_t elapsedTicks = elapsedTicksClocksPerSec / CLOCKS_PER_SEC;
  int64_t newRemainder = elapsedTicksClocksPerSec % CLOCKS_PER_SEC;
  
  timer->_lastClock = currentClock;
  AddTimerTicks(timer, elapsedTicks);
  timer->_lastRemainder = newRemainder;

  return timer->_lastTicks;
}

void AddTimerTicks(Timer* timer, int64_t ticks) {
  int64_t newTicks = timer->_lastTicks + ticks;
  if (newTicks < timer->_lastTicks && ticks > 0) { newTicks = INT64_MAX; } // Overflow check
  if (newTicks > timer->_lastTicks && ticks < 0) { newTicks = INT64_MIN; } // Underflow check
  timer->_lastTicks = (newTicks > timer->maxTicks) ? timer->maxTicks : newTicks;
}
