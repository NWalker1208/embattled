#include <raylib.h>
#include <math.h>
#include "arena/simulation.h"
#include "utilities/sleep.h"

#define PHYSICS_TIMESTEP 0.001

#define ROBOT_MAX_SPEED 300.0
#define ROBOT_ROT_SPEED 6.0

void* StartSimulation(void* arg) {
  SimulationArguments* simulationArgs = arg;

  while (!simulationArgs->shouldStop) {
    pthread_mutex_lock(&simulationArgs->stateMutex);
    
    if (IsKeyDown(KEY_LEFT)) {
      simulationArgs->physicsWorldState.bodyStates[0].rotation -= ROBOT_ROT_SPEED * PHYSICS_TIMESTEP;
    }
    
    if (IsKeyDown(KEY_RIGHT)) {
      simulationArgs->physicsWorldState.bodyStates[0].rotation += ROBOT_ROT_SPEED * PHYSICS_TIMESTEP;
    }

    double velocity;
    if (IsKeyDown(KEY_UP)) {
      velocity += ROBOT_MAX_SPEED;
    }
    if (IsKeyDown(KEY_DOWN)) {
      velocity -= ROBOT_MAX_SPEED;
    }
    simulationArgs->physicsWorldState.bodyStates[0].position.x += cos(simulationArgs->physicsWorldState.bodyStates[0].rotation) * velocity * PHYSICS_TIMESTEP;
    simulationArgs->physicsWorldState.bodyStates[0].position.y += sin(simulationArgs->physicsWorldState.bodyStates[0].rotation) * velocity * PHYSICS_TIMESTEP;

    StepPhysicsWorld(&simulationArgs->physicsWorldDefinition, &simulationArgs->physicsWorldState, PHYSICS_TIMESTEP);
    pthread_mutex_unlock(&simulationArgs->stateMutex);
  
    psleep((int)(PHYSICS_TIMESTEP * 1000));
  }

  return NULL;
}

