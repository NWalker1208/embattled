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
  // A mutex protecting reads and writes to this struct.
  pthread_mutex_t mutex;
  // The thread that is running the simulation.
  pthread_t thread;
  // Whether the thread has been started.
  bool isThreadValid;
  
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
// On success, returns true. If an error occurs, returns false.
bool TryInitSimulation(Simulation* simulation, size_t robotCount, Rectangle boundary);

// Destroys the simulation and cleans up any system resources.
void DestroySimulation(Simulation* simulation);

// Starts the simulation loop on a new thread, if it is not already running.
void StartSimulationThread(Simulation* simulation);

// Stops and cleans up the simulation thread if it is running.
void StopSimulationThread(Simulation* simulation);

// Function that should be called when the simulation starts.
void OnSimulationStart(Simulation* simulation);

// Function that should be called on each simulation step.
void OnSimulationStep(Simulation* simulation);
