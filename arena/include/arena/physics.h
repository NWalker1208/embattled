#pragma once
#include "arena/vector.h"

#define MAX_PHYSICS_BODIES 16

// A simple circular physics body.
typedef struct {
  // The radius of the circular body in word-space units.
  const double radius;
  // The position of the body in world-space coordinates.
  Vector2D position;
  // The rotation of the body in degrees.
  double rotation;
} PhysicsBody;

// A physics world containing zero or more bodies.
typedef struct {
  // The upper-left boundary of the world.
  const Vector2D upperLeftBound;
  // The lower-right boundary of the world.
  const Vector2D lowerRightBound;

  // The number of bodies being simulated.
  unsigned int bodyCount;
  // The array of bodies being simulated.
  PhysicsBody bodies[MAX_PHYSICS_BODIES];
} PhysicsWorld;

// Simulates the given physics world for a single step.
void StepPhysicsWorld(PhysicsWorld* state, double deltaTimeSeconds);
