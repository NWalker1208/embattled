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


Robot InitRobot(size_t physicsBodyIndex) {
  return (Robot){
    .physicsBodyIndex = physicsBodyIndex,
    .energyRemaining = ROBOT_INITIAL_ENERGY,
  };
}

void ApplyRobotControls(Robot* robot, PhysicsWorld* physicsWorld) {
  PhysicsBody* body = &physicsWorld->bodies[robot->physicsBodyIndex];

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
          otherRobot->energyRemaining -= weaponControl * WEAPON_DAMAGE;
        }
      }
    }
  }
}
