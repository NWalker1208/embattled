#include <raylib.h>
#include <math.h>
#include "arena/simulation.h"
#include "utilities/sleep.h"

#define PHYSICS_TIMESTEP 0.001

#define ROBOT_MAX_SPEED 300.0
#define ROBOT_ROT_SPEED 6.0


void StepSimulation(SimulationArguments* simulation);


void* StartSimulation(void* arg) {
  SimulationArguments* simulation = arg;
  pthread_mutex_t* mutex = &simulation->mutex;

  // TODO: Have this execute a number of steps depending on the amount of time elapsed.
  while (!simulation->shouldStop) {
    pthread_mutex_lock(mutex); {
      StepSimulation(simulation);
    } pthread_mutex_unlock(mutex);
  
    psleep((int)(PHYSICS_TIMESTEP * 1000));
  }

  return NULL;
}


void StepSimulation(SimulationArguments* simulation) {
  PhysicsWorld* physicsWorld = &simulation->physicsWorld;

  if (IsKeyDown(KEY_LEFT)) {
    physicsWorld->bodies[0].rotation -= ROBOT_ROT_SPEED * PHYSICS_TIMESTEP;
  }
  
  if (IsKeyDown(KEY_RIGHT)) {
    physicsWorld->bodies[0].rotation += ROBOT_ROT_SPEED * PHYSICS_TIMESTEP;
  }

  double velocity = 0;
  if (IsKeyDown(KEY_UP)) {
    velocity += ROBOT_MAX_SPEED;
  }
  if (IsKeyDown(KEY_DOWN)) {
    velocity -= ROBOT_MAX_SPEED;
  }
  physicsWorld->bodies[0].position.x += cos(physicsWorld->bodies[0].rotation) * velocity * PHYSICS_TIMESTEP;
  physicsWorld->bodies[0].position.y += sin(physicsWorld->bodies[0].rotation) * velocity * PHYSICS_TIMESTEP;

  StepPhysicsWorld(physicsWorld, PHYSICS_TIMESTEP);
}


// TODO: Incorporate processor simulation.
