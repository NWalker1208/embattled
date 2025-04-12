#include "arena/robot.h"

Robot InitRobot(size_t physicsBodyIndex) {
  return (Robot){
    .physicsBodyIndex = physicsBodyIndex,
    .energyRemaining = ROBOT_INITIAL_ENERGY,
  };
}
