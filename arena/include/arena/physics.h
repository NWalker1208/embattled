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

// The definition of a physics world containing zero or more bodies.
typedef struct {
  // The upper-left boundary of the world.
  Vector2D upperLeftBound;
  // The lower-right boundary of the world.
  Vector2D lowerRightBound;
  // The number of bodies being simulated.
  unsigned int bodyCount;
  // The array of definitions of the bodies being simulated.
  const PhysicsBodyDefinition* bodies;
} PhysicsWorldDefinition;

// The state of a physics world containing zero or more bodies.
typedef struct {
  // The array of states of the bodies being simulated.
  // The length of array should equal the bodyCount of the corresponding world definition.
  PhysicsBodyState* bodyStates;
} PhysicsWorldState;

// Simulates the given physics world for a single step.
void StepPhysicsWorld(const PhysicsWorldDefinition* world, PhysicsWorldState* state, double deltaTimeSeconds);
