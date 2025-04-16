#include "arena/physics.h"
#include <stdbool.h>
#include <math.h>
#include <raymath.h>

#define MAX_RESOLVER_ITERATIONS 32


// Checks whether the body is colliding with the world boundary.
// If it is, returns true and outputs a vector indicating how far the body is penetrating into the world boundary.
// Otherwise, returns false.
bool CheckCollisionBodyBoundary(const PhysicsBody* body, const PhysicsWorld* world, Vector2* penetrationOut);

// Checks whether two bodies are colliding with one another.
// If they are, returns true and outputs a vector indiciating how far body A is penetrating into body B.
// Otherwise, returns false.
bool CheckCollisionBodies(const PhysicsBody* bodyA, const PhysicsBody* bodyB, Vector2* penetrationOut);


void StepPhysicsWorld(PhysicsWorld* world, double deltaTimeSeconds) {
  // Update positions and rotations based on current velocities
  for (unsigned int i = 0; i < world->bodyCount; i++) {
    PhysicsBody* body = &world->bodies[i];
    body->position.x += body->linearVelocity.x * deltaTimeSeconds;
    body->position.y += body->linearVelocity.y * deltaTimeSeconds;
    body->rotation += body->angularVelocity * deltaTimeSeconds;
    body->rotation -= floor(body->rotation / (M_PI * 2)) * (M_PI * 2);
  }

  // Resolve collisions between bodies and the world boundary
  for (unsigned int i = 0; i < world->bodyCount; i++) {
    PhysicsBody* body = &world->bodies[i];
    Vector2 penetration;
    if (CheckCollisionBodyBoundary(body, world, &penetration)) {
      body->position = Vector2Subtract(body->position, penetration);
    }
  }

  // Iteratively resolve collisions between physics bodies
  bool foundCollision = true;
  for (unsigned int k = 0; foundCollision && k < MAX_RESOLVER_ITERATIONS; k++) {
    foundCollision = false;
    for (unsigned int i = 0; i < world->bodyCount - 1; i++) {
      for (unsigned int j = i + 1; j < world->bodyCount; j++) {
        PhysicsBody* bodyA = &world->bodies[i];
        PhysicsBody* bodyB = &world->bodies[j];
        if (bodyA->isStatic && bodyB->isStatic) {
          continue; // Two static bodies don't affect each other.
        }

        Vector2 penetration;
        if (CheckCollisionBodies(bodyA, bodyB, &penetration)) {
          foundCollision = true;

          if (!bodyA->isStatic && !bodyB->isStatic) {
            Vector2 halfPenetration = Vector2Scale(penetration, 0.5);
            bodyA->position = Vector2Subtract(bodyA->position, halfPenetration);
            bodyB->position = Vector2Add(bodyB->position, halfPenetration);
          } else if (!bodyA->isStatic) {
            bodyA->position = Vector2Subtract(bodyA->position, penetration);
          } else {
            bodyB->position = Vector2Add(bodyB->position, penetration);
          }

          // Move both bodies away from boundary if either is now colliding
          if (!bodyA->isStatic && CheckCollisionBodyBoundary(bodyA, world, &penetration)) {
            bodyA->position = Vector2Subtract(bodyA->position, penetration);
            if (!bodyB->isStatic) { bodyB->position = Vector2Subtract(bodyB->position, penetration); }
          }

          if (!bodyB->isStatic && CheckCollisionBodyBoundary(bodyB, world, &penetration)) {
            if (!bodyA->isStatic) { bodyA->position = Vector2Subtract(bodyA->position, penetration); }
            bodyB->position = Vector2Subtract(bodyB->position, penetration);
          }
        }
      }
    }
  }
}


bool CheckCollisionBodyBoundary(const PhysicsBody* body, const PhysicsWorld* world, Vector2* penetrationOut) {
  bool colliding = false;

  if (body->position.x - body->radius < world->boundary.x) {
    colliding = true;
    penetrationOut->x = body->position.x - body->radius - world->boundary.x - EPSILON;
  } else if (body->position.x + body->radius > world->boundary.x + world->boundary.width) {
    colliding = true;
    penetrationOut->x = body->position.x + body->radius - (world->boundary.x + world->boundary.width) + EPSILON;
  } else {
    penetrationOut->x = 0;
  }

  if (body->position.y - body->radius < world->boundary.y) {
    colliding = true;
    penetrationOut->y = body->position.y - body->radius - world->boundary.y - EPSILON;
  } else if (body->position.y + body->radius > world->boundary.y + world->boundary.height) {
    colliding = true;
    penetrationOut->y = body->position.y + body->radius - (world->boundary.y + world->boundary.height) + EPSILON;
  } else {
    penetrationOut->y = 0;
  }

  return colliding;
}

bool CheckCollisionBodies(const PhysicsBody* bodyA, const PhysicsBody* bodyB, Vector2* penetrationOut) {
  Vector2 delta = Vector2Subtract(bodyB->position, bodyA->position);
  float radii = bodyA->radius + bodyB->radius;

  float squareDistance = Vector2LengthSqr(delta);
  if (squareDistance < radii * radii) {
    float distance = sqrt(squareDistance);
    if (distance > EPSILON) {
      delta.x /= distance; delta.y /= distance;
    } else {
      // Arbitrary but deterministic normal vector
      delta = (Vector2){ 1.0, 0.0 };
    }

    float penetrationDepth = radii - distance + EPSILON;
    *penetrationOut = Vector2Scale(delta, penetrationDepth);
    return true;
  } else {
    return false;
  }
}
