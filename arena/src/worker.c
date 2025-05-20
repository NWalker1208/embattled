#include "arena/worker.h"
#include "utilities/sleep.h"


void* workerThread(void* arg);


bool TryInitWorker(Worker* worker, uint32_t (*onStep)(void* state), void* state) {
  *worker = (Worker){
    .onStep = onStep,
    .state = state,
  };
  if (pthread_mutex_init(&worker->stateMutex, NULL)) {
    return false;
  }
  return true;
}

void DestroyWorker(Worker* worker) {
  StopWorker(worker);
  pthread_mutex_destroy(&worker->stateMutex);
  *worker = (Worker){ 0 };
}

void StartWorker(Worker* worker) {
  if (worker->isThreadValid) { return; }

  worker->shouldStop = false;
  if (!pthread_create(&worker->thread, NULL, workerThread, worker)) {
    worker->isThreadValid = true;
  }
}

void StopWorker(Worker* worker) {
  if (!worker->isThreadValid) { return; }

  worker->shouldStop = true;
  pthread_join(worker->thread, NULL);
  worker->isThreadValid = false;
}


void* workerThread(void* arg) {
  Worker* worker = (Worker*)arg;
  while (!worker->shouldStop) {
    uint32_t waitMs;
    pthread_mutex_lock(&worker->stateMutex); {
      waitMs = worker->onStep(worker->state);
    } pthread_mutex_unlock(&worker->stateMutex);
    psleep(waitMs);
  }
  return NULL;
}
