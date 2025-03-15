#pragma once
#include "arena/vector.h"

// The definition of a simple circular physics body.
typedef struct {
  // The initial position of the body in word-space coordinates.
  Vector2D initialPosition;
  // The initial rotation of the body in degrees.
  double initialRotation;
  // The radius of the circular body in word-space units.
  double radius;
} PhysicsBodyDefinition;

// The state of a simple circular physics body.
typedef struct {
  // The position of the body in world-space coordinates.
  Vector2D position;
  // The rotation of the body in degrees.
  double rotation;
} PhysicsBodyState;
