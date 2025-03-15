#include <stdbool.h>
#include <math.h>
#include "arena/physics.h"

#define MAX_RESOLVER_ITERATIONS 32
#define EPSILON 0.0001


bool CheckCollisionBodyWithBoundary(const PhysicsWorldDefinition* world, const PhysicsWorldState* state, unsigned int bodyIndex, Vector2D* penetrationOut);
bool CheckCollisionBodyWithBody(const PhysicsWorldDefinition* world, const PhysicsWorldState* state, unsigned int bodyAIndex, unsigned int bodyBIndex, Vector2D* penetrationOut);


void StepPhysicsWorld(const PhysicsWorldDefinition* world, PhysicsWorldState* state, double deltaTimeSeconds) {
  (void) deltaTimeSeconds; // The current physics simulator doesn't handle momentum, but this param will be used if that feature is added in the future.
  
  // Resolve collisions between bodies and the world boundary
  for (unsigned int i = 0; i < world->bodyCount; i++) {
    Vector2D penetration;
    if (CheckCollisionBodyWithBoundary(world, state, i, &penetration)) {
      state->bodyStates[i].position.x -= penetration.x;
      state->bodyStates[i].position.y -= penetration.y;
    }
  }

  // Iteratively resolve collisions between physics bodies
  bool foundCollision = true;
  for (unsigned int k = 0; foundCollision && k < MAX_RESOLVER_ITERATIONS; k++) {
    foundCollision = false;
    for (unsigned int i = 0; i < world->bodyCount - 1; i++) {
      for (unsigned int j = i + 1; j < world->bodyCount; j++) {
        Vector2D penetration;
        if (CheckCollisionBodyWithBody(world, state, i, j, &penetration)) {
          foundCollision = true;
          PhysicsBodyState* bodyAState = &state->bodyStates[i];
          PhysicsBodyState* bodyBState = &state->bodyStates[j];

          bodyAState->position.x -= penetration.x / 2;
          bodyAState->position.y -= penetration.y / 2;
          bodyBState->position.x += penetration.x / 2;
          bodyBState->position.y += penetration.y / 2;

          // Move both bodies away from boundary if either is now colliding
          if (CheckCollisionBodyWithBoundary(world, state, i, &penetration)) {
            bodyAState->position.x -= penetration.x;
            bodyAState->position.y -= penetration.y;
            bodyBState->position.x -= penetration.x;
            bodyBState->position.y -= penetration.y;
          }

          if (CheckCollisionBodyWithBoundary(world, state, j, &penetration)) {
            bodyAState->position.x -= penetration.x;
            bodyAState->position.y -= penetration.y;
            bodyBState->position.x -= penetration.x;
            bodyBState->position.y -= penetration.y;
          }
        }
      }
    }
  }
}


bool CheckCollisionBodyWithBoundary(const PhysicsWorldDefinition* world, const PhysicsWorldState* state, unsigned int bodyIndex, Vector2D* penetrationOut) {
  const PhysicsBodyDefinition* body = &world->bodies[bodyIndex];
  const PhysicsBodyState* bodyState = &state->bodyStates[bodyIndex];

  bool colliding = false;

  if (bodyState->position.x - body->radius < world->upperLeftBound.x) {
    colliding = true;
    penetrationOut->x = bodyState->position.x - body->radius - world->upperLeftBound.x - EPSILON;
  } else if (bodyState->position.x + body->radius > world->lowerRightBound.x) {
    colliding = true;
    penetrationOut->x = bodyState->position.x + body->radius - world->lowerRightBound.x + EPSILON;
  }

  if (bodyState->position.y - body->radius < world->upperLeftBound.y) {
    colliding = true;
    penetrationOut->y = bodyState->position.y - body->radius - world->upperLeftBound.y - EPSILON;
  } else if (bodyState->position.y + body->radius > world->lowerRightBound.y) {
    colliding = true;
    penetrationOut->y = bodyState->position.y + body->radius - world->lowerRightBound.y + EPSILON;
  }

  return colliding;
}

bool CheckCollisionBodyWithBody(const PhysicsWorldDefinition* world, const PhysicsWorldState* state, unsigned int bodyAIndex, unsigned int bodyBIndex, Vector2D* penetrationOut) {
  const PhysicsBodyDefinition* bodyA = &world->bodies[bodyAIndex];
  const PhysicsBodyDefinition* bodyB = &world->bodies[bodyBIndex];
  const PhysicsBodyState* bodyAState = &state->bodyStates[bodyAIndex];
  const PhysicsBodyState* bodyBState = &state->bodyStates[bodyBIndex];

  double deltaX = bodyBState->position.x - bodyAState->position.x;
  double deltaY = bodyBState->position.y - bodyAState->position.y;
  double radii = bodyA->radius + bodyB->radius;

  double squareDistance = deltaX * deltaX + deltaY * deltaY;
  if (squareDistance < radii * radii) {
    double distance = sqrt(squareDistance);
    if (distance > EPSILON) {
      deltaX /= distance;
      deltaY /= distance;
    } else {
      // Arbitrary but deterministic normal vector
      deltaX = 1.0;
      deltaY = 0.0;
    }

    float penetrationDepth = radii - distance + EPSILON;
    penetrationOut->x = deltaX * penetrationDepth;
    penetrationOut->y = deltaY * penetrationDepth;
    return true;
  } else {
    return false;
  }
}
