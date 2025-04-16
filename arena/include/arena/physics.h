#pragma once
#include <raylib.h>

#define MAX_PHYSICS_BODIES 16

// A kind of physics collider.
typedef enum {
  PHYSICS_COLLIDER_CIRCLE,
  PHYSICS_COLLIDER_RECTANGLE,
} PhysicsColliderKind;

// A physics collider.
typedef struct {
  // The kind of the collider.
  PhysicsColliderKind kind;
  union {
    // The radius of the circular collider (kind == PHYSICS_COLLIDER_CIRCLE)
    double radius;
    // The width and height of the rectangular collider (kind == PHYSICS_COLLIDER_RECTANGLE)
    Vector2 widthHeight;
  };
} PhysicsCollider;

// A simple circular physics body.
typedef struct {
  // The body's collider.
  PhysicsCollider collider;
  // Whether or not the body is static (i.e., unaffected by other bodies).
  bool isStatic;

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
