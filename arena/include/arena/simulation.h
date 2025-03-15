#pragma once
#include <pthread.h>
#include <stdbool.h>
#include "arena/physics.h"

typedef struct {
  const unsigned int physicsBodyIndex;
} Robot;

typedef struct {
  const unsigned int robotCount;
  Robot* const robots;

  const PhysicsWorldDefinition physicsWorldDefinition;
  PhysicsWorldState physicsWorldState;
  
  pthread_mutex_t stateMutex;

  bool shouldStop;
} SimulationArguments;

void* StartSimulation(void* arg);
