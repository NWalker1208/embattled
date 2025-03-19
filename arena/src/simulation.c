#include <raylib.h>
#include <math.h>
#include <time.h>
#include "arena/simulation.h"
#include "utilities/sleep.h"


#define STEPS_PER_SEC 1000
#define ROBOT_MAX_SPEED 300.0
#define ROBOT_ROT_SPEED 6.0
#define MAX_SEC_BEHIND 1

// A unit of simulation time.
// Defined such that both simulation steps and clocks can be represented as whole numbers.
typedef long simtime_t;

#define SIMTIME_PER_SEC   (simtime_t)(STEPS_PER_SEC * CLOCKS_PER_SEC)
#define SIMTIME_PER_STEP  (simtime_t)(CLOCKS_PER_SEC) // (SIMTIME_PER_SEC / STEPS_PER_SEC)
#define SIMTIME_PER_CLOCK (simtime_t)(STEPS_PER_SEC)  // (SIMTIME_PER_SEC / CLOCKS_PER_SEC)

#define DELTA_TIME_SEC (double)(1.0 / STEPS_PER_SEC)


void StepSimulation(SimulationArguments* simulation, double deltaTimeSeconds);


void* StartSimulation(void* arg) {
  SimulationArguments* simulation = arg;
  pthread_mutex_t* mutex = &simulation->mutex;

  clock_t lastClock = clock();
  simtime_t simtimeBehind = 0;
  while (!simulation->shouldStop) {
    while (!simulation->shouldStop && simtimeBehind > 0) {
      pthread_mutex_lock(mutex); {
        StepSimulation(simulation, DELTA_TIME_SEC);
      } pthread_mutex_unlock(mutex);
      simtimeBehind -= SIMTIME_PER_STEP;
    }

    clock_t currentClock = clock();
    clock_t elapsedClocks = currentClock - lastClock;
    simtime_t elapsedSimtime = elapsedClocks * SIMTIME_PER_CLOCK;
    if (elapsedClocks > (MAX_SEC_BEHIND * CLOCKS_PER_SEC) || simtimeBehind + elapsedSimtime > (MAX_SEC_BEHIND * SIMTIME_PER_SEC)) {
      simtimeBehind = CLOCKS_PER_SEC; // Cap the measure of how far we are behind realtime at MAX_SEC_BEHIND.
    } else {
      simtimeBehind += elapsedSimtime;
    }
    lastClock = currentClock;

    long millisecondsAhead = (-simtimeBehind) / (SIMTIME_PER_SEC / 1000);
    if (millisecondsAhead > 0) {
      psleep(millisecondsAhead);
    }
  }

  return NULL;
}


void StepSimulation(SimulationArguments* simulation, double deltaTimeSeconds) {
  PhysicsWorld* physicsWorld = &simulation->physicsWorld;

  if (IsKeyDown(KEY_LEFT)) {
    physicsWorld->bodies[0].rotation -= ROBOT_ROT_SPEED * deltaTimeSeconds;
  }
  
  if (IsKeyDown(KEY_RIGHT)) {
    physicsWorld->bodies[0].rotation += ROBOT_ROT_SPEED * deltaTimeSeconds;
  }

  double velocity = 0;
  if (IsKeyDown(KEY_UP)) {
    velocity += ROBOT_MAX_SPEED;
  }
  if (IsKeyDown(KEY_DOWN)) {
    velocity -= ROBOT_MAX_SPEED;
  }
  physicsWorld->bodies[0].position.x += cos(physicsWorld->bodies[0].rotation) * velocity * deltaTimeSeconds;
  physicsWorld->bodies[0].position.y += sin(physicsWorld->bodies[0].rotation) * velocity * deltaTimeSeconds;

  StepPhysicsWorld(physicsWorld, deltaTimeSeconds);
}


// TODO: Incorporate processor simulation.
