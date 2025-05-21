#pragma once
#include <time.h>
#include <stdint.h>

// A timer used to track the passage of time.
typedef struct Timer {
  int64_t ticksPerSec; // The number of ticks that should pass in one second.
  int64_t maxTicks; // The maximum number of ticks the timer can hold.

  clock_t _lastClock; // The clock reading when the timer was last updated.
  int64_t _lastTicks; // The tick count when the timer was last updated.
  int64_t _lastRemainder; // The remainder of the last timer update.
} Timer;

// Initializes the timer such that the number of ticks elapsed is zero.
Timer InitTimer(int64_t ticksPerSec, int64_t maxTicks);

// Gets the current number of ticks elapsed, up to timer.maxTicks.
int64_t GetTimerTicks(Timer* timer);

// Adds the specified number of ticks to the timer.
void AddTimerTicks(Timer* timer, int64_t ticks);
