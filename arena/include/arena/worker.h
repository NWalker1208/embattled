#pragma once
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

// A worker that executes a function in a loop until it is told to stop.
typedef struct Worker {
  // A mutex protecting interactions with the worker state.
  pthread_mutex_t stateMutex;
  // The worker's thread.
  pthread_t thread;
  // Whether the worker thread has been started.
  bool isThreadValid;
  // Whether the worker should stop.
  bool shouldStop;

  // The function to execute on each step.
  // The return value indicates the number of milliseconds to wait outside of the mutex before the next step.
  uint32_t (*onStep)(void* state);
  // The pointer to the state structure expected by the worker function.
  void* state;
} Worker;

// Attempts to initialize a worker. Does not start the worker thread.
// If successful, returns true. Otherwise, returns false.
bool TryInitWorker(Worker* worker, uint32_t (*onStep)(void* state), void* state);

// Destroys the worker (including stopping the worker thread) and cleans up any system resources.
void DestroyWorker(Worker* worker);

// Starts the worker thread if it is not already running.
void StartWorker(Worker* worker);

// Stops and cleans up the worker thread if it is running.
void StopWorker(Worker* worker);
