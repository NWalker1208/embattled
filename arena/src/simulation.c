#include "arena/simulation.h"
#include <math.h>
#include <time.h>
#include <raylib.h>
#include <raymath.h>
#include "arena/raycast.h"
#include "utilities/sleep.h"


#define STEPS_PER_SEC 1000
#define MAX_SEC_BEHIND 1

#define ROBOT_NUM_SENSORS 9
#define ROBOT_SENSORS_FOV 90.0
#define ROBOT_MAX_SENSOR_DIST 500.0
#define ROBOT_MAX_SPEED 300.0
#define ROBOT_ROT_SPEED 6.0

// A unit of simulation time.
// Defined such that both simulation steps and clocks can be represented as whole numbers.
typedef long simtime_t;

#define SIMTIME_PER_SEC   (simtime_t)(STEPS_PER_SEC * CLOCKS_PER_SEC)
#define SIMTIME_PER_STEP  (simtime_t)(CLOCKS_PER_SEC) // (SIMTIME_PER_SEC / STEPS_PER_SEC)
#define SIMTIME_PER_CLOCK (simtime_t)(STEPS_PER_SEC)  // (SIMTIME_PER_SEC / CLOCKS_PER_SEC)

#define DELTA_TIME_SEC (double)(1.0 / STEPS_PER_SEC)

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)


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
  // Update sensors using raycasts
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    Robot* robot = &simulation->robots[i];
    PhysicsBody* body = &simulation->physicsWorld.bodies[robot->physicsBodyIndex];

    for (unsigned int j = 0; j < ROBOT_NUM_SENSORS; j++) {
      float angle = body->rotation - (ROBOT_SENSORS_FOV / 2) + j * (ROBOT_SENSORS_FOV / (ROBOT_NUM_SENSORS - 1));
      Vector2 rayDirection = (Vector2){ cos(angle), sin(angle) };
      Vector2 rayOrigin = Vector2Add(body->position, Vector2Scale(rayDirection, body->radius + 1));
      RaycastResult result = ComputeRaycast(&simulation->physicsWorld, rayOrigin, rayDirection);
      
      double distance = result.distance;
      if (distance > ROBOT_MAX_SENSOR_DIST) { distance = ROBOT_MAX_SENSOR_DIST; }
      robot->processState.memory[0xE000 + (j * 2)] = (unsigned char)(distance / ROBOT_MAX_SENSOR_DIST * 255.0);
      robot->processState.memory[0xE001 + (j * 2)] = (unsigned char)result.type;
    }
  }

  // Step processes
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    stepProcess(&simulation->robots[i].processState);
  }

  // Perform actions based on robot processes
  PhysicsWorld* physicsWorld = &simulation->physicsWorld;
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    Robot* robot = &simulation->robots[i];
    if (robot->energyRemaining <= 0) {
      continue;
    }

    PhysicsBody* body = &simulation->physicsWorld.bodies[robot->physicsBodyIndex];

    signed char rotationControl = MAX((signed char)robot->processState.memory[0xF000], -127);
    signed char velocityControl = MAX((signed char)robot->processState.memory[0xF001], -127);
    unsigned char weaponControl = robot->weaponCooldownRemaining > 0 ? 0 : robot->processState.memory[0xF002];
    robot->energyRemaining -= abs(rotationControl);
    robot->energyRemaining -= abs(velocityControl);
    if (weaponControl > 0) {
      weaponControl = MIN(robot->energyRemaining, weaponControl);
      robot->energyRemaining -= weaponControl;
      robot->weaponCooldownRemaining = ROBOT_WEAPON_COOLDOWN_STEPS;
    } else if (robot->weaponCooldownRemaining > 0) {
      robot->weaponCooldownRemaining--;
    }

    double angularVelocity = rotationControl / 127.0;
    double velocity = velocityControl / 127.0;
    double weaponStrength = weaponControl / 256.0;

    body->rotation += ROBOT_ROT_SPEED * angularVelocity * deltaTimeSeconds;
    body->position.x += ROBOT_MAX_SPEED * cos(body->rotation) * velocity * deltaTimeSeconds;
    body->position.y += ROBOT_MAX_SPEED * sin(body->rotation) * velocity * deltaTimeSeconds;

    if (weaponControl > 0) {
      // Fire laser at other robots using a raycast.
      Vector2 rayDirection = (Vector2){ cos(body->rotation), sin(body->rotation) };
      Vector2 rayOrigin = Vector2Add(body->position, Vector2Scale(rayDirection, body->radius + 1));
      RaycastResult result = ComputeRaycast(physicsWorld, rayOrigin, rayDirection);
      if (result.type != INTERSECTION_NONE) {
        robot->lastWeaponStart = rayOrigin;
        robot->lastWeaponEnd = Vector2Add(rayOrigin, Vector2Scale(rayDirection, result.distance));
        
        if (result.type == INTERSECTION_BODY && result.bodyIndex >= 0) {
          // Find the robot index corresponding to the body index
          Robot* otherRobot = NULL;
          for (unsigned int j = 0; j < simulation->robotCount; j++) {
            if (simulation->robots[j].physicsBodyIndex == (unsigned int)result.bodyIndex) {
              otherRobot = &simulation->robots[j];
              break;
            }
          }
          // Apply damage to the other robot
          if (otherRobot != NULL && otherRobot->energyRemaining > 0) {
            otherRobot->energyRemaining -= weaponStrength * 10;
          }
        }
    }
    }
  }

  // Temporary user control code
  {
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
  }

  // Step the physics world
  StepPhysicsWorld(physicsWorld, deltaTimeSeconds);
}
