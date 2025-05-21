#include "arena/simulation.h"
#include <math.h>
#include <time.h>
#include <limits.h>
#include <raylib.h>
#include <raymath.h>
#include "arena/raycast.h"
#include "utilities/sleep.h"
#if defined(PLATFORM_WEB)
#include "emscripten.h"
#endif


#define DELTA_TIME_SEC (double)(1.0 / SIMULATION_DEFAULT_TICKS_PER_SECOND)

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)


void stepSimulation(Simulation* simulation);
void onWeaponDamage(void* context, size_t physicsBodyIndex, int damageAmount);


void AddRobotToSimulation(Simulation* simulation, Vector2 position, float rotation) {
  if (simulation->robotCount >= SIMULATION_MAX_ROBOTS || simulation->physicsWorld.bodyCount >= MAX_PHYSICS_BODIES) { return; }

  size_t bodyIndex = simulation->physicsWorld.bodyCount;
  simulation->physicsWorld.bodyCount++;
  simulation->physicsWorld.bodies[bodyIndex] = (PhysicsBody){
    .position = position,
    .rotation = rotation,
    .collider = (PhysicsCollider){
      .kind = PHYSICS_COLLIDER_CIRCLE,
      .radius = ROBOT_RADIUS,
    }
  };

  size_t robotIndex = simulation->robotCount;
  simulation->robotCount++;
  simulation->robots[robotIndex] = InitRobot(bodyIndex);
}

void AddObstacleToSimulation(Simulation* simulation, Vector2 position, float rotation, PhysicsCollider collider) {
  if (simulation->physicsWorld.bodyCount >= MAX_PHYSICS_BODIES) { return; }
  
  size_t bodyIndex = simulation->physicsWorld.bodyCount;
  simulation->physicsWorld.bodyCount++;
  simulation->physicsWorld.bodies[bodyIndex] = (PhysicsBody){
    .isStatic = true,
    .position = position,
    .rotation = rotation,
    .collider = collider,
  };
}

void PrepSimulation(Simulation* simulation) {
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    UpdateRobotSensor(&simulation->robots[i], &simulation->physicsWorld);
  }
}

void UpdateSimulation(Simulation* simulation) {
  int64_t elapsedTicks = GetTimerTicks(&simulation->timer);
  if (simulation->forceStep) {
    simulation->forceStep = false;
    stepSimulation(simulation);
  }
  if (elapsedTicks > 0) {
    for (int64_t i = 0; i < elapsedTicks; i++) {
      stepSimulation(simulation);
    }
    AddTimerTicks(&simulation->timer, -elapsedTicks);
  }
}


void stepSimulation(Simulation* simulation) {
  PhysicsWorld* physicsWorld = &simulation->physicsWorld;

  // Step robot processes
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    if (simulation->robots[i].energyRemaining > 0) {
      stepProcess(&simulation->robots[i].processState);
    }
  }

  // Apply robot controls
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    ApplyRobotControls(&simulation->robots[i], &simulation->physicsWorld, (WeaponDamageCallback){ simulation, onWeaponDamage });
  }

  // Step physics world
  StepPhysicsWorld(physicsWorld, DELTA_TIME_SEC);

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
