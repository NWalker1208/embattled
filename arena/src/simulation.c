#include "arena/simulation.h"
#include <math.h>
#include <time.h>
#include <limits.h>
#include <raylib.h>
#include <raymath.h>
#include "arena/raycast.h"
#include "utilities/sleep.h"


#define STEPS_PER_SEC 1024
#define MAX_MS_BEHIND 100

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)


// A unit of time used to compare realtime and simulation time.
// Defined such that both simulation steps and clocks can be represented as whole numbers.
typedef long ticks_t;


#pragma region Computed time constants

#define DELTA_TIME_SEC    (double)(1.0 / STEPS_PER_SEC)

#define TICKS_PER_SEC   (ticks_t)(STEPS_PER_SEC * CLOCKS_PER_SEC)
#define TICKS_PER_STEP  (ticks_t)(CLOCKS_PER_SEC) // (SIMTIME_PER_SEC / STEPS_PER_SEC)
#define TICKS_PER_CLOCK (ticks_t)(STEPS_PER_SEC)  // (SIMTIME_PER_SEC / CLOCKS_PER_SEC)

#define MAX_CLOCKS_BEHIND (clock_t)(MAX_MS_BEHIND * (CLOCKS_PER_SEC / 1000))
#define MAX_TICKS_BEHIND  (ticks_t)(MAX_MS_BEHIND * (TICKS_PER_SEC / 1000))

#pragma endregion


void* simulationThread(void* arg);
void stepSimulation(Simulation* simulation, double deltaTimeSeconds);
void onWeaponDamage(void* context, size_t physicsBodyIndex, int damageAmount);


bool TryInitSimulation(Simulation* simulation, size_t robotCount, Rectangle boundary) {
  if (pthread_mutex_init(&simulation->mutex, NULL)) {
    return false;
  }

  *simulation = (Simulation){
    .robotCount = robotCount,
    .physicsWorld.boundary = boundary,
    .physicsWorld.bodyCount = robotCount,
    .timeScale = 0,
    .shouldStop = false
  };

  for (size_t i = 0; i < robotCount; i++) {
    simulation->robots[i] = InitRobot(i);
    simulation->physicsWorld.bodies[i] = (PhysicsBody){
      .collider = {
        .kind = PHYSICS_COLLIDER_CIRCLE,
        .radius = ROBOT_RADIUS
      }
    };
  }

  return true;
}

void DestroySimulation(Simulation* simulation) {
  StopSimulationThread(simulation);
  pthread_mutex_destroy(&simulation->mutex);
}

void StartSimulationThread(Simulation* simulation) {
  if (simulation->isThreadValid) { return; }

  simulation->shouldStop = false;
  if (!pthread_create(&simulation->thread, NULL, simulationThread, simulation)) {
    simulation->isThreadValid = true;
  }
}

void StopSimulationThread(Simulation* simulation) {
  if (!simulation->isThreadValid) { return; }

  simulation->shouldStop = true;
  pthread_join(simulation->thread, NULL);
  simulation->isThreadValid = false;
}


void* simulationThread(void* arg) {
  Simulation* simulation = arg;
  pthread_mutex_t* mutex = &simulation->mutex;

  // Initialize sensors
  pthread_mutex_lock(mutex); {
    for (unsigned int i = 0; i < simulation->robotCount; i++) {
      UpdateRobotSensor(&simulation->robots[i], &simulation->physicsWorld);
    }
  } pthread_mutex_unlock(mutex);

  // Enter simulation loop
  ticks_t relativeTicks = 0; // The difference between the simulation time and realtime measured in ticks
  unsigned int timeScale = 0; // The number of simulation seconds per realtime second multiplied by NEUTRAL_TIME_SCALE
  clock_t lastRealtimeClock;
  ticks_t lastElapsedScaledTicksRemainder = 0; // The remainder of the previous iteration's elapsed ticks (0 <= elapsedTicksRemainder < NEUTRAL_TIME_SCALE)

  while (!simulation->shouldStop) {
    // Update the difference between simulation and realtime
    clock_t currentRealtimeClock = clock();
    clock_t elapsedRealtimeClocks = currentRealtimeClock - lastRealtimeClock;
    lastRealtimeClock = currentRealtimeClock;
    elapsedRealtimeClocks = MAX(0, MIN(elapsedRealtimeClocks, MAX_CLOCKS_BEHIND));

    ticks_t elapsedScaledTicks = ((elapsedRealtimeClocks * TICKS_PER_CLOCK) * timeScale) + lastElapsedScaledTicksRemainder;
    relativeTicks -= elapsedScaledTicks / SIMULATION_NEUTRAL_TIME_SCALE;
    lastElapsedScaledTicksRemainder = elapsedScaledTicks % SIMULATION_NEUTRAL_TIME_SCALE;
    
    if (relativeTicks < -MAX_TICKS_BEHIND || timeScale == UINT_MAX) {
      relativeTicks = -MAX_TICKS_BEHIND; // Cap the measure of how far we are behind realtime at MAX_SEC_BEHIND.
    }

    // Access/modify the simulation state
    long msToSleep;
    pthread_mutex_lock(mutex); {
      if (simulation->forceStep && relativeTicks >= 0) {
        relativeTicks = -TICKS_PER_STEP;
        simulation->forceStep = false;
      }

      if (relativeTicks < 0) {
        // Running behind realtime -> step the simulation
        stepSimulation(simulation, DELTA_TIME_SEC);
        relativeTicks += TICKS_PER_STEP;
        msToSleep = -1;
      } else {
        // Running ahead of realtime -> sleep
        msToSleep = relativeTicks / (TICKS_PER_SEC / 1000);
      }

      // Update timeScale
      timeScale = simulation->timeScale;
    } pthread_mutex_unlock(mutex);

    if (msToSleep >= 0) {
      psleep(msToSleep);
    }
  }

  return NULL;
}

void stepSimulation(Simulation* simulation, double deltaTimeSeconds) {
  PhysicsWorld* physicsWorld = &simulation->physicsWorld;

  // Step robot processes
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    stepProcess(&simulation->robots[i].processState);
  }

  // Apply robot controls
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    ApplyRobotControls(&simulation->robots[i], &simulation->physicsWorld, (WeaponDamageCallback){ simulation, onWeaponDamage });
  }

  // Step physics world
  StepPhysicsWorld(physicsWorld, deltaTimeSeconds);

  // Update robot sensors
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    UpdateRobotSensor(&simulation->robots[i], &simulation->physicsWorld);
  }
}

void onWeaponDamage(void* context, size_t physicsBodyIndex, int damageAmount) {
  Simulation* simulation = context;

  // Find the robot index corresponding to the body index
  Robot* otherRobot = NULL;
  for (unsigned int j = 0; j < simulation->robotCount; j++) {
    if (simulation->robots[j].physicsBodyIndex == physicsBodyIndex) {
      otherRobot = &simulation->robots[j];
      break;
    }
  }

  // Apply damage to the other robot
  if (otherRobot != NULL && otherRobot->energyRemaining > 0) {
    otherRobot->energyRemaining -= damageAmount;
  }
}
