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

#define ROBOT_SENSORS_FOV_RAD (DEG2RAD * 90.0)
#define ROBOT_MAX_SENSOR_DIST 500.0
#define ROBOT_MAX_SPEED 300.0
#define ROBOT_ROT_SPEED 6.0

// A unit of time used to compare realtime and simulation time.
// Defined such that both simulation steps and clocks can be represented as whole numbers.
typedef long ticks_t;

#define TICKS_PER_SEC   (ticks_t)(STEPS_PER_SEC * CLOCKS_PER_SEC)
#define TICKS_PER_STEP  (ticks_t)(CLOCKS_PER_SEC) // (SIMTIME_PER_SEC / STEPS_PER_SEC)
#define TICKS_PER_CLOCK (ticks_t)(STEPS_PER_SEC)  // (SIMTIME_PER_SEC / CLOCKS_PER_SEC)

#define DELTA_TIME_SEC    (double)(1.0 / STEPS_PER_SEC)
#define MAX_CLOCKS_BEHIND (clock_t)(MAX_MS_BEHIND * (CLOCKS_PER_SEC / 1000))
#define MAX_TICKS_BEHIND  (ticks_t)(MAX_MS_BEHIND * (TICKS_PER_SEC / 1000))

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)


void StepSimulation(SimulationArguments* simulation, double deltaTimeSeconds);


void* StartSimulation(void* arg) {
  SimulationArguments* simulation = arg;
  pthread_mutex_t* mutex = &simulation->mutex;

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
    relativeTicks -= elapsedScaledTicks / NEUTRAL_TIME_SCALE;
    lastElapsedScaledTicksRemainder = elapsedScaledTicks % NEUTRAL_TIME_SCALE;
    
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
        StepSimulation(simulation, DELTA_TIME_SEC);
        relativeTicks += TICKS_PER_STEP;
        msToSleep = -1;
      } else {
        // Running ahead of realtime -> sleep
        msToSleep = relativeTicks / (TICKS_PER_SEC / 1000);
      }

      // Update timeScale
      // Temporary code for manipulating time scale
      if (IsKeyPressed(KEY_ZERO)) {
        simulation->timeScale = 0;
      } else if (IsKeyPressed(KEY_ONE)) {
        simulation->timeScale = 1;
      } else if (IsKeyPressed(KEY_TWO)) {
        simulation->timeScale = NEUTRAL_TIME_SCALE / 4;
      } else if (IsKeyPressed(KEY_THREE)) {
        simulation->timeScale = NEUTRAL_TIME_SCALE / 2;
      } else if (IsKeyPressed(KEY_FOUR)) {
        simulation->timeScale = NEUTRAL_TIME_SCALE;
      } else if (IsKeyPressed(KEY_FIVE)) {
        simulation->timeScale = NEUTRAL_TIME_SCALE * 2;
      } else if (IsKeyPressed(KEY_SIX)) {
        simulation->timeScale = NEUTRAL_TIME_SCALE * 4;
      } else if (IsKeyPressed(KEY_SEVEN)) {
        simulation->timeScale = UINT_MAX;
      }

      if (IsKeyPressed(KEY_SPACE)) {
        simulation->forceStep = true;
      }

      timeScale = simulation->timeScale;
    } pthread_mutex_unlock(mutex);

    if (msToSleep >= 0) {
      psleep(msToSleep);
    }
  }

  return NULL;
}


void StepSimulation(SimulationArguments* simulation, double deltaTimeSeconds) {
  PhysicsWorld* physicsWorld = &simulation->physicsWorld;

  // Step the physics world
  StepPhysicsWorld(physicsWorld, deltaTimeSeconds);

  // Update sensors using raycasts
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    Robot* robot = &simulation->robots[i];
    PhysicsBody* body = &simulation->physicsWorld.bodies[robot->physicsBodyIndex];

    for (unsigned int j = 0; j < ROBOT_NUM_SENSORS; j++) {
      float angle = body->rotation - (ROBOT_SENSORS_FOV_RAD / 2) + j * (ROBOT_SENSORS_FOV_RAD / (ROBOT_NUM_SENSORS - 1));
      Vector2 rayDirection = (Vector2){ cos(angle), sin(angle) };
      Vector2 rayOrigin = Vector2Add(body->position, Vector2Scale(rayDirection, body->radius + 1));
      RaycastResult result = ComputeRaycast(&simulation->physicsWorld, rayOrigin, rayDirection);
      
      float distance = result.distance;
      if (distance > ROBOT_MAX_SENSOR_DIST) { distance = ROBOT_MAX_SENSOR_DIST; }
      robot->sensors[j].start = rayOrigin;
      robot->sensors[j].end = Vector2Add(rayOrigin, Vector2Scale(rayDirection, distance));
      robot->processState.memory[0xE000 + (j * 2)] = (unsigned char)(distance / ROBOT_MAX_SENSOR_DIST * 255.0);
      robot->processState.memory[0xE001 + (j * 2)] = (unsigned char)result.type;
    }
  }

  // Step processes
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    stepProcess(&simulation->robots[i].processState);
  }

  // Perform actions based on robot processes
  for (unsigned int i = 0; i < simulation->robotCount; i++) {
    Robot* robot = &simulation->robots[i];
    if (robot->energyRemaining <= 0) {
      if (robot->weaponCooldownRemaining > 0) {
        robot->weaponCooldownRemaining--;
      }
      continue;
    }

    PhysicsBody* body = &simulation->physicsWorld.bodies[robot->physicsBodyIndex];

    signed char rotationControl = MAX((signed char)robot->processState.memory[0xF000], -127);
    signed char velocityControl = MAX((signed char)robot->processState.memory[0xF001], -127);
    unsigned char weaponControl = robot->weaponCooldownRemaining > 0 ? 0 : robot->processState.memory[0xF002];

    // Temporary user control code
    if (i == 0) {
      if (IsKeyDown(KEY_RIGHT)) {
        rotationControl = 127;
      } else if (IsKeyDown(KEY_LEFT)) {
        rotationControl = -127;
      }

      if (IsKeyDown(KEY_UP)) {
        velocityControl = 127;
      } else if (IsKeyDown(KEY_DOWN)) {
        velocityControl = -127;
      }

      if (IsKeyDown(KEY_SPACE)) {
        weaponControl = 255;
      }
    }

    robot->energyRemaining -= abs(rotationControl);
    robot->energyRemaining -= abs(velocityControl);
    if (robot->weaponCooldownRemaining > 0) {
      weaponControl = 0;
      robot->weaponCooldownRemaining--;
    } else if (weaponControl > 0) {
      weaponControl = MIN(robot->energyRemaining, weaponControl);
      robot->energyRemaining -= weaponControl;
      robot->weaponCooldownRemaining = ROBOT_WEAPON_COOLDOWN_STEPS;
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
        robot->lastWeaponFire.start = rayOrigin;
        robot->lastWeaponFire.end = Vector2Add(rayOrigin, Vector2Scale(rayDirection, result.distance));
        
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
            otherRobot->energyRemaining -= (int)(weaponStrength * 4096);
          }
        }
      }
    }
  }
}
