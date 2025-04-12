#pragma once
#include <pthread.h>
#include <stdbool.h>
#include "arena/physics.h"
#include "processor/process.h"
#include "arena/robot.h"

#define SIMULATION_MAX_ROBOTS 2
#define SIMULATION_NEUTRAL_TIME_SCALE 1024


// The state of a simulation.
typedef struct {
  // A mutex protecting reads/writes to the robots and physics world.
  pthread_mutex_t mutex;
  
  // The physics world being simulated.
  PhysicsWorld physicsWorld;

  // The number of robots being simulated.
  size_t robotCount;
  // The array of robots being simulated.
  Robot robots[SIMULATION_MAX_ROBOTS];

  // The scaling factor applied to the passage of time in the simulation.
  //   0 = Paused
  //   NEUTRAL_TIME_SCALE = Realtime
  //   UINT_MAX = Unlimited
  unsigned int timeScale;
  // Whether a simulation step should occur on the next iteration regardless of the current time scale.
  bool forceStep;

  // Whether the simulation loop should stop.
  bool shouldStop;
} Simulation;

// Initializes a simulation with the given number of robots.
Simulation InitSimulation(size_t robotCount);

// Starts the simulation loop on the current thread. Expects a pointer to a Simulation.
// Returns NULL.
void* StartSimulation(void* arg);
