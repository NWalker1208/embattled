#pragma once
#include <stdint.h>
#include <raylib.h>
#include "processor/process.h"
#include "arena/physics.h"

#define ROBOT_RADIUS                50.0
#define ROBOT_INITIAL_ENERGY        4000000
#define ROBOT_WEAPON_COOLDOWN_STEPS 1000
#define ROBOT_NUM_SENSORS           3


typedef struct {
  // The index of the physics body representing this robot.
  size_t physicsBodyIndex;
  
  // The amount of energy that the robot has left. Also represents the robot's health.
  int energyRemaining;

  // The number of simulation steps until the robot's weapon is ready to use.
  int weaponCooldownRemaining;
  // The start and end points of the robot's last weapon fire.
  struct {
    Vector2 start, end;
  } lastWeaponFire;

  // The current start and end points of the robot's sensors.
  struct {
    Vector2 start, end;
  } sensors[ROBOT_NUM_SENSORS];

  // The state of the robot's processor.
  struct ProcessState processState;
} Robot;

// Initializes a robot.
Robot InitRobot(size_t physicsBodyIndex);

// Steps the robot's internal simulation.
void ApplyRobotControls(Robot* robot, PhysicsWorld* physicsWorld);
