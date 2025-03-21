#include <stdbool.h>
#include <math.h>
#include "arena/physics.h"

#define MAX_RESOLVER_ITERATIONS 32
#define EPSILON 0.0001


// Checks whether the body with the given index is colliding with the world boundary.
// If it is, returns true and outputs a vector indicating how far the body is penetrating into the world boundary.
// Otherwise, returns false.
bool CheckCollisionBodyWithBoundary(const PhysicsWorld* world, unsigned int bodyIndex, Vector2D* penetrationOut);

// Checks whether two bodies with the given indices are colliding with one another.
// If they are, returns true and outputs a vector indiciating how far body A is penetrating into body B.
// Otherwise, returns false.
bool CheckCollisionBodyWithBody(const PhysicsWorld* world, unsigned int bodyAIndex, unsigned int bodyBIndex, Vector2D* penetrationOut);


void StepPhysicsWorld(PhysicsWorld* world, double deltaTimeSeconds) {
  (void) deltaTimeSeconds; // The current physics simulator doesn't handle momentum, but this param will be used if that feature is added in the future.
  
  // Resolve collisions between bodies and the world boundary
  for (unsigned int i = 0; i < world->bodyCount; i++) {
    Vector2D penetration;
    if (CheckCollisionBodyWithBoundary(world, i, &penetration)) {
      world->bodies[i].position.x -= penetration.x;
      world->bodies[i].position.y -= penetration.y;
    }
  }

  // Iteratively resolve collisions between physics bodies
  bool foundCollision = true;
  for (unsigned int k = 0; foundCollision && k < MAX_RESOLVER_ITERATIONS; k++) {
    foundCollision = false;
    for (unsigned int i = 0; i < world->bodyCount - 1; i++) {
      for (unsigned int j = i + 1; j < world->bodyCount; j++) {
        Vector2D penetration;
        if (CheckCollisionBodyWithBody(world, i, j, &penetration)) {
          foundCollision = true;
          PhysicsBody* bodyA = &world->bodies[i];
          PhysicsBody* bodyB = &world->bodies[j];

          bodyA->position.x -= penetration.x / 2;
          bodyA->position.y -= penetration.y / 2;
          bodyB->position.x += penetration.x / 2;
          bodyB->position.y += penetration.y / 2;

          // Move both bodies away from boundary if either is now colliding
          if (CheckCollisionBodyWithBoundary(world, i, &penetration)) {
            bodyA->position.x -= penetration.x;
            bodyA->position.y -= penetration.y;
            bodyB->position.x -= penetration.x;
            bodyB->position.y -= penetration.y;
          }

          if (CheckCollisionBodyWithBoundary(world, j, &penetration)) {
            bodyA->position.x -= penetration.x;
            bodyA->position.y -= penetration.y;
            bodyB->position.x -= penetration.x;
            bodyB->position.y -= penetration.y;
          }
        }
      }
    }
  }
}


bool CheckCollisionBodyWithBoundary(const PhysicsWorld* world, unsigned int bodyIndex, Vector2D* penetrationOut) {
  const PhysicsBody* body = &world->bodies[bodyIndex];

  bool colliding = false;

  if (body->position.x - body->radius < world->upperLeftBound.x) {
    colliding = true;
    penetrationOut->x = body->position.x - body->radius - world->upperLeftBound.x - EPSILON;
  } else if (body->position.x + body->radius > world->lowerRightBound.x) {
    colliding = true;
    penetrationOut->x = body->position.x + body->radius - world->lowerRightBound.x + EPSILON;
  } else {
    penetrationOut->x = 0;
  }

  if (body->position.y - body->radius < world->upperLeftBound.y) {
    colliding = true;
    penetrationOut->y = body->position.y - body->radius - world->upperLeftBound.y - EPSILON;
  } else if (body->position.y + body->radius > world->lowerRightBound.y) {
    colliding = true;
    penetrationOut->y = body->position.y + body->radius - world->lowerRightBound.y + EPSILON;
  } else {
    penetrationOut->y = 0;
  }

  return colliding;
}

bool CheckCollisionBodyWithBody(const PhysicsWorld* world, unsigned int bodyAIndex, unsigned int bodyBIndex, Vector2D* penetrationOut) {
  const PhysicsBody* bodyA = &world->bodies[bodyAIndex];
  const PhysicsBody* bodyB = &world->bodies[bodyBIndex];

  Vector2D delta = VectorDifference(bodyB->position, bodyA->position);
  double radii = bodyA->radius + bodyB->radius;

  double squareDistance = VectorGetSqrMagnitude(delta);
  if (squareDistance < radii * radii) {
    double distance = sqrt(squareDistance);
    if (distance > EPSILON) {
      delta.x /= distance;
      delta.y /= distance;
    } else {
      // Arbitrary but deterministic normal vector
      delta.x = 1.0;
      delta.y = 0.0;
    }

    float penetrationDepth = radii - distance + EPSILON;
    penetrationOut->x = delta.x * penetrationDepth;
    penetrationOut->y = delta.y * penetrationDepth;
    return true;
  } else {
    return false;
  }
}
