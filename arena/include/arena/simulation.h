#pragma once
#include <pthread.h>
#include <stdbool.h>
#include "arena/physics.h"
#include "processor/process.h"

#define MAX_ROBOTS 2
#define ROBOT_INITIAL_ENERGY 16000000
#define ROBOT_WEAPON_COOLDOWN_STEPS 1000

// A robot that is being simulated.
typedef struct {
  // The index of the physics body representing this robot.
  const unsigned int physicsBodyIndex;
  // The amount of energy that the robot has left. Also represents the robot's health.
  int energyRemaining;
  // The number of simulation steps until the robot's weapon is ready to use.
  int weaponCooldownRemaining;
  // The starting point of the weapon the last time it was fired.
  Vector2 lastWeaponStart;
  // The ending point of the weapon the last time it was fired.
  Vector2 lastWeaponEnd;
  // The state of the robot's processor.
  struct ProcessState processState;
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
  // The scaling factor applied to the passage of time in the simulation. +inf for maximum speed.
  double timeScale;
  // Whether the simulation loop should stop.
  bool shouldStop;
} SimulationArguments;

// Starts the simulation loop. Expects a pointer to SimulationArguments.
void* StartSimulation(void* arg);
