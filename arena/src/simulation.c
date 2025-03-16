#include <raylib.h>
#include <math.h>
#include "arena/simulation.h"
#include "utilities/sleep.h"

#define PHYSICS_TIMESTEP 0.001

#define ROBOT_MAX_SPEED 300.0
#define ROBOT_ROT_SPEED 6.0

void* StartSimulation(void* arg) {
  SimulationArguments* sim = arg;
  PhysicsWorld* physics = &sim->physicsWorld;
  pthread_mutex_t* mutex = &sim->mutex;

  while (!sim->shouldStop) {
    pthread_mutex_lock(mutex); {
      if (IsKeyDown(KEY_LEFT)) {
        physics->bodies[0].rotation -= ROBOT_ROT_SPEED * PHYSICS_TIMESTEP;
      }
      
      if (IsKeyDown(KEY_RIGHT)) {
        physics->bodies[0].rotation += ROBOT_ROT_SPEED * PHYSICS_TIMESTEP;
      }

      double velocity = 0;
      if (IsKeyDown(KEY_UP)) {
        velocity += ROBOT_MAX_SPEED;
      }
      if (IsKeyDown(KEY_DOWN)) {
        velocity -= ROBOT_MAX_SPEED;
      }
      physics->bodies[0].position.x += cos(physics->bodies[0].rotation) * velocity * PHYSICS_TIMESTEP;
      physics->bodies[0].position.y += sin(physics->bodies[0].rotation) * velocity * PHYSICS_TIMESTEP;

      StepPhysicsWorld(physics, PHYSICS_TIMESTEP);
    } pthread_mutex_unlock(mutex);
  
    psleep((int)(PHYSICS_TIMESTEP * 1000));
  }

  return NULL;
}
