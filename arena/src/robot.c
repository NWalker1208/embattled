#include "arena/robot.h"
#include <raymath.h>
#include "arena/raycast.h"

#define MOVE_ADDRESS   0xF000
#define ROTATE_ADDRESS 0xF001
#define WEAPON_ADDRESS 0xF002

#define MOVE_SPEED    300.0
#define ROTATE_SPEED  6.0
#define WEAPON_DAMAGE 4096

#define MOVE_COST   1
#define ROTATE_COST 1
#define WEAPON_COST 1024

#define SENSORS_FOV_RAD (DEG2RAD * 90.0)
#define MAX_SENSOR_DIST 500.0

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)


Robot InitRobot(size_t physicsBodyIndex) {
  return (Robot){
    .physicsBodyIndex = physicsBodyIndex,
    .energyRemaining = ROBOT_INITIAL_ENERGY,
  };
}

void ApplyRobotControls(Robot* robot, PhysicsWorld* physicsWorld, WeaponDamageCallback weaponDamageCallback) {
  PhysicsBody* body = &physicsWorld->bodies[robot->physicsBodyIndex];

  if (robot->weaponCooldownRemaining > 0) {
    robot->weaponCooldownRemaining--;
  }

  if (robot->energyRemaining <= 0) {
    robot->energyRemaining = 0;
    body->linearVelocity = (Vector2){ 0, 0 };
    body->angularVelocity = 0;
    return;
  }

  signed char moveControl = MAX((signed char)robot->processState.memory[MOVE_ADDRESS], -127);
  signed char rotateControl = MAX((signed char)robot->processState.memory[ROTATE_ADDRESS], -127);
  unsigned char weaponControl = robot->processState.memory[WEAPON_ADDRESS];

  // Temporary user control code
  if (robot->physicsBodyIndex == 0) {
    if (IsKeyDown(KEY_UP)) {
      moveControl = 127;
    } else if (IsKeyDown(KEY_DOWN)) {
      moveControl = -127;
    }

    if (IsKeyDown(KEY_RIGHT)) {
      rotateControl = 127;
    } else if (IsKeyDown(KEY_LEFT)) {
      rotateControl = -127;
    }

    if (IsKeyDown(KEY_SPACE)) {
      weaponControl = 255;
    }
  }

  robot->energyRemaining -= abs(moveControl) * MOVE_COST;
  robot->energyRemaining -= abs(rotateControl) * ROTATE_COST;
  if (robot->weaponCooldownRemaining > 0) {
    weaponControl = 0;
  } else if (weaponControl > 0) {
    weaponControl = MIN(robot->energyRemaining / WEAPON_COST, weaponControl);
    robot->energyRemaining -= weaponControl * WEAPON_COST;
    robot->weaponCooldownRemaining = ROBOT_WEAPON_COOLDOWN_STEPS;
  }

  double moveVelocity = MOVE_SPEED * (moveControl / 127.0);
  body->linearVelocity = (Vector2){ cos(body->rotation) * moveVelocity, sin(body->rotation) * moveVelocity };
  body->angularVelocity = ROTATE_SPEED * (rotateControl / 127.0);

  if (weaponControl > 0) {
    // Fire laser at other robots using a raycast.
    Vector2 rayDirection = (Vector2){ cos(body->rotation), sin(body->rotation) };
    Vector2 rayOrigin = Vector2Add(body->position, Vector2Scale(rayDirection, body->radius + 1));
    RaycastResult result = ComputeRaycast(physicsWorld, rayOrigin, rayDirection);
    if (result.type != INTERSECTION_NONE) {
      robot->lastWeaponFire.start = rayOrigin;
      robot->lastWeaponFire.end = Vector2Add(rayOrigin, Vector2Scale(rayDirection, result.distance));
      
      if (result.type == INTERSECTION_BODY && result.bodyIndex >= 0) {
        weaponDamageCallback.func(weaponDamageCallback.context, result.bodyIndex, weaponControl * WEAPON_DAMAGE);
      }
    }
  }
}

void UpdateRobotSensors(Robot* robot, PhysicsWorld* physicsWorld) {
  PhysicsBody* body = &physicsWorld->bodies[robot->physicsBodyIndex];

  for (unsigned int j = 0; j < ROBOT_NUM_SENSORS; j++) {
    float angle = body->rotation - (SENSORS_FOV_RAD / 2) + j * (SENSORS_FOV_RAD / (ROBOT_NUM_SENSORS - 1));
    Vector2 rayDirection = (Vector2){ cos(angle), sin(angle) };
    Vector2 rayOrigin = Vector2Add(body->position, Vector2Scale(rayDirection, body->radius + 1));
    RaycastResult result = ComputeRaycast(physicsWorld, rayOrigin, rayDirection);
    
    float distance = result.distance;
    if (distance > MAX_SENSOR_DIST) { distance = MAX_SENSOR_DIST; }
    robot->sensors[j].start = rayOrigin;
    robot->sensors[j].end = Vector2Add(rayOrigin, Vector2Scale(rayDirection, distance));
    robot->processState.memory[0xE000 + (j * 2)] = (unsigned char)(distance / MAX_SENSOR_DIST * 255.0);
    robot->processState.memory[0xE001 + (j * 2)] = (unsigned char)result.type;
  }
}
