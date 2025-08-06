#pragma once
#include <stdbool.h>
#include "arena/physics.h"
#include "processor/process.h"
#include "arena/robot.h"
#include "arena/timer.h"

#define SIMULATION_MAX_ROBOTS 2
#define SIMULATION_DEFAULT_TICKS_PER_SECOND 1024


// The state of a simulation.
typedef struct {
  // The physics world being simulated.
  PhysicsWorld physicsWorld;

  // The number of robots being simulated.
  size_t robotCount;
  // The array of robots being simulated.
  Robot robots[SIMULATION_MAX_ROBOTS];
  // Whether or not the battle has ended (one or fewer robots left standing).
  bool battleEnded;
  // If battleEnded is true, the index of the last surviving robot; otherwise, undefined.
  size_t lastSurvivingRobotIndex;

  // Timer for tracking elapsed simulation time, where each tick represents a simulation step.
  Timer timer;
  // Whether a simulation step should occur on the next iteration regardless of how much time has elapsed.
  bool forceStep;
} Simulation;


// Adds a robot to the simulation if there are fewer than SIMULATION_MAX_ROBOTS robots and MAX_PHYSICS_BODIES bodies.
void AddRobotToSimulation(Simulation* simulation, Vector2 position, float rotation);

// Adds a static obstacle to the simulation if there are fewer than MAX_PHYSICS_BODIES bodies.
void AddObstacleToSimulation(Simulation* simulation, Vector2 position, float rotation, PhysicsCollider collider);

// Prepares the simulation for the first update given the current set of robots and obstacles.
void PrepSimulation(Simulation* simulation);

// Updates the simulation by advancing the appropriate number of steps.
void UpdateSimulation(Simulation* simulation);
