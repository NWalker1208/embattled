#pragma once
#include <stdbool.h>
#include "arena/physics.h"
#include "processor/process.h"
#include "arena/robot.h"

#define SIMULATION_MAX_ROBOTS 2
#define SIMULATION_NEUTRAL_TIME_SCALE 1024


// The state of a simulation.
typedef struct {
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
} Simulation;


// Adds a robot to the simulation if there are fewer than SIMULATION_MAX_ROBOTS robots and MAX_PHYSICS_BODIES bodies.
void AddRobotToSimulation(Simulation* simulation, Vector2 position, float rotation);

// Adds a static obstacle to the simulation if there are fewer than MAX_PHYSICS_BODIES bodies.
int AddObstacleToSimulation(Simulation* simulation, Vector2 position, PhysicsCollider collider);

// Initializes the simulation given the current set of robots and obstacles.
void InitSimulation(Simulation* simulation);

// Updates the simulation by advancing the appropriate number of steps.
// Returns the number of milliseconds that can be waited before calling this function again.
uint32_t UpdateSimulation(Simulation* simulation);
