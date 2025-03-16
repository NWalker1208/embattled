#pragma once
#include <pthread.h>
#include <stdbool.h>
#include "arena/physics.h"

#define MAX_ROBOTS 2

// A robot that is being simulated.
typedef struct {
  // The index of the physics body representing this robot.
  const unsigned int physicsBodyIndex;
  // TODO: Add process state
} Robot;

// The arguments expected by the StartSimulation function.
typedef struct {
  // The number of robots being simulated.
  unsigned int robotCount;
  // The array of robots being simulated.
  Robot robots[MAX_ROBOTS];
  // The physics world containing the robots being simulated.
  PhysicsWorld physicsWorld;
  // A mutex protecting reads/writes to the robots and physics world.
  pthread_mutex_t mutex;
  // Whether the simulation loop should stop.
  bool shouldStop;
} SimulationArguments;

// Starts the simulation loop. Expects a pointer to SimulationArguments.
void* StartSimulation(void* arg);
