#pragma once
#include <raylib.h>

#define MAX_PHYSICS_BODIES 16

// A simple circular physics body.
typedef struct {
  // The radius of the circular body in word-space units.
  double radius;

  // The position of the body in world-space coordinates.
  Vector2 position;
  // The rotation of the body in radians.
  float rotation;

  // The current linear velocity of the body in word-space coordinates per second.
  Vector2 linearVelocity;
  // The current angular velocity of the body in radians per second.
  float angularVelocity;
} PhysicsBody;

// A physics world containing zero or more bodies.
typedef struct {
  // The boundary in which physics bodies are confined.
  Rectangle boundary;
  
  // The number of physics bodies being simulated.
  unsigned int bodyCount;
  // The array of physics bodies being simulated.
  PhysicsBody bodies[MAX_PHYSICS_BODIES];
} PhysicsWorld;

// Simulates the given physics world for a single step.
void StepPhysicsWorld(PhysicsWorld* world, double deltaTimeSeconds);
