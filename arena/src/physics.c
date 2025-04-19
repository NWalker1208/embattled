#include "arena/physics.h"
#include <stdbool.h>
#include <math.h>
#include <raymath.h>
#include <assert.h>

#define MAX_RESOLVER_ITERATIONS 32


#pragma region Collision helper functions

// Checks whether the body is colliding with the world boundary.
// If it is, returns true and outputs a vector indicating how far the body is penetrating into the world boundary.
// Otherwise, returns false.
bool checkCollisionBodyBoundary(const PhysicsBody* body, const PhysicsWorld* world, Vector2* penetrationOut);

// Checks whether two bodies are colliding with one another.
// If they are, returns true and outputs a vector indiciating how far body A is penetrating into body B.
// Otherwise, returns false.
bool checkCollisionBodies(const PhysicsBody* bodyA, const PhysicsBody* bodyB, Vector2* penetrationOut);

bool checkCollisionCircleColliderBoundary(Vector2 position, float radius, const PhysicsWorld* world, Vector2* penetrationOut);
bool checkCollisionRectangleColliderBoundary(Vector2 position, float rotation, Vector2 widthHeight, const PhysicsWorld* world, Vector2* penetrationOut);
bool checkCollisionCircleColliders(Vector2 positionA, float radiusA, Vector2 positionB, float radiusB, Vector2* penetrationOut);
bool checkCollisionRectangleColliders(Vector2 positionA, float rotationA, Vector2 widthHeightA, Vector2 positionB, float rotationB, Vector2 widthHeightB, Vector2* penetrationOut);
bool checkCollisionCircleColliderRectangleCollider(Vector2 positionA, float radiusA, Vector2 positionB, float rotationB, Vector2 widthHeightB, Vector2* penetrationOut);

#pragma endregion


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
    if (checkCollisionBodyBoundary(body, world, &penetration)) {
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
        if (checkCollisionBodies(bodyA, bodyB, &penetration)) {
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
          if (!bodyA->isStatic && checkCollisionBodyBoundary(bodyA, world, &penetration)) {
            bodyA->position = Vector2Subtract(bodyA->position, penetration);
            if (!bodyB->isStatic) { bodyB->position = Vector2Subtract(bodyB->position, penetration); }
          }

          if (!bodyB->isStatic && checkCollisionBodyBoundary(bodyB, world, &penetration)) {
            if (!bodyA->isStatic) { bodyA->position = Vector2Subtract(bodyA->position, penetration); }
            bodyB->position = Vector2Subtract(bodyB->position, penetration);
          }
        }
      }
    }
  }
}


bool checkCollisionBodyBoundary(const PhysicsBody* body, const PhysicsWorld* world, Vector2* penetrationOut) {
  switch (body->collider.kind) {
    case PHYSICS_COLLIDER_CIRCLE:
      return checkCollisionCircleColliderBoundary(body->position, body->collider.radius, world, penetrationOut);
    case PHYSICS_COLLIDER_RECTANGLE:
      return checkCollisionRectangleColliderBoundary(body->position, body->rotation, body->collider.widthHeight, world, penetrationOut);
  }

  assert(false);
  return false;
}

bool checkCollisionBodies(const PhysicsBody* bodyA, const PhysicsBody* bodyB, Vector2* penetrationOut) {
  switch (bodyA->collider.kind) {
    case PHYSICS_COLLIDER_CIRCLE:
      switch (bodyB->collider.kind) {
        case PHYSICS_COLLIDER_CIRCLE:
          return checkCollisionCircleColliders(bodyA->position, bodyA->collider.radius, bodyB->position, bodyB->collider.radius, penetrationOut);
        case PHYSICS_COLLIDER_RECTANGLE:
          return checkCollisionCircleColliderRectangleCollider(bodyA->position, bodyA->collider.radius, bodyB->position, bodyB->rotation, bodyB->collider.widthHeight, penetrationOut);
      } break;

    case PHYSICS_COLLIDER_RECTANGLE:
      switch (bodyB->collider.kind) {
        case PHYSICS_COLLIDER_CIRCLE:
          bool colliding = checkCollisionCircleColliderRectangleCollider(bodyB->position, bodyB->collider.radius, bodyA->position, bodyA->rotation, bodyA->collider.widthHeight, penetrationOut);
          *penetrationOut = Vector2Negate(*penetrationOut);
          return colliding;
        case PHYSICS_COLLIDER_RECTANGLE:
          return checkCollisionRectangleColliders(bodyA->position, bodyA->rotation, bodyA->collider.widthHeight, bodyB->position, bodyB->rotation, bodyB->collider.widthHeight, penetrationOut);
      } break;
  }
  
  assert(false);
  return false;
}


bool checkCollisionCircleColliderBoundary(Vector2 position, float radius, const PhysicsWorld* world, Vector2* penetrationOut) {
  bool colliding = false;

  if (position.x - radius < world->boundary.x) {
    colliding = true;
    penetrationOut->x = position.x - radius - world->boundary.x - EPSILON;
  } else if (position.x + radius > world->boundary.x + world->boundary.width) {
    colliding = true;
    penetrationOut->x = position.x + radius - (world->boundary.x + world->boundary.width) + EPSILON;
  } else {
    penetrationOut->x = 0;
  }

  if (position.y - radius < world->boundary.y) {
    colliding = true;
    penetrationOut->y = position.y - radius - world->boundary.y - EPSILON;
  } else if (position.y + radius > world->boundary.y + world->boundary.height) {
    colliding = true;
    penetrationOut->y = position.y + radius - (world->boundary.y + world->boundary.height) + EPSILON;
  } else {
    penetrationOut->y = 0;
  }

  return colliding;
}

bool checkCollisionRectangleColliderBoundary(Vector2 position, float rotation, Vector2 widthHeight, const PhysicsWorld* world, Vector2* penetrationOut) {
  (void)position;
  (void)rotation;
  (void)widthHeight;
  (void)world;
  (void)penetrationOut;
  return false; // TODO: Not necessary to implement at the moment. All dynamic bodies are circles.
}

bool checkCollisionCircleColliders(Vector2 positionA, float radiusA, Vector2 positionB, float radiusB, Vector2* penetrationOut) {
  Vector2 delta = Vector2Subtract(positionB, positionA);
  float radii = radiusA + radiusB;

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

bool checkCollisionRectangleColliders(Vector2 positionA, float rotationA, Vector2 widthHeightA, Vector2 positionB, float rotationB, Vector2 widthHeightB, Vector2* penetrationOut) {
  (void)positionA;
  (void)rotationA;
  (void)widthHeightA;
  (void)positionB;
  (void)rotationB;
  (void)widthHeightB;
  (void)penetrationOut;
  return false; // TODO: Not necessary to implement at the moment. All dynamic bodies are circles.
}

bool checkCollisionCircleColliderRectangleCollider(Vector2 positionA, float radiusA, Vector2 positionB, float rotationB, Vector2 widthHeightB, Vector2* penetrationOut) {
  Vector2 circleRelativePosition = Vector2Subtract(positionA, positionB);
  circleRelativePosition = Vector2Rotate(circleRelativePosition, -rotationB);

  float halfWidth = widthHeightB.x / 2;
  float halfHeight = widthHeightB.y / 2;

  float leftRightSignedDistance = fabsf(circleRelativePosition.x) - halfWidth;
  float topBottomSignedDistance = fabsf(circleRelativePosition.y) - halfHeight;

  bool circleIsBetweenLeftAndRight = leftRightSignedDistance <= 0;
  bool circleIsBetweenTopAndBottom = topBottomSignedDistance <= 0;

  if (circleIsBetweenTopAndBottom && (!circleIsBetweenLeftAndRight || leftRightSignedDistance > topBottomSignedDistance)) {
    // Check for collision with left-right sides.
    if (leftRightSignedDistance < radiusA) {
      float penetrationDepth = radiusA - leftRightSignedDistance + EPSILON;
      penetrationOut->x = circleRelativePosition.x > 0 ? -penetrationDepth : penetrationDepth;
      penetrationOut->y = 0;
      *penetrationOut = Vector2Rotate(*penetrationOut, rotationB);
      return true;
    }
    
  } else if (circleIsBetweenLeftAndRight) {
    // Check for collision with top-bottom sides.
    if (topBottomSignedDistance < radiusA) {
      float penetrationDepth = radiusA - topBottomSignedDistance + EPSILON;
      penetrationOut->x = 0;
      penetrationOut->y = circleRelativePosition.y > 0 ? -penetrationDepth : penetrationDepth;
      *penetrationOut = Vector2Rotate(*penetrationOut, rotationB);
      return true;
    }


  } else {
    // Check for collision with nearest corner.
    Vector2 deltaToNearestCorner = (Vector2){ leftRightSignedDistance, topBottomSignedDistance };
    
    float sqrDistanceToNearestCorner = Vector2LengthSqr(deltaToNearestCorner);
    if (sqrDistanceToNearestCorner < radiusA * radiusA) {
      float distanceToNearestCorner = sqrt(sqrDistanceToNearestCorner);
      if (distanceToNearestCorner > EPSILON) {
        deltaToNearestCorner.x /= distanceToNearestCorner; deltaToNearestCorner.y /= distanceToNearestCorner;
      } else {
        // Arbitrary but deterministic normal vector
        deltaToNearestCorner = (Vector2){ M_SQRT1_2, M_SQRT1_2 };
      }

      // Set signs based on which corner the circle is closest to.
      deltaToNearestCorner.x = circleRelativePosition.x > 0 ? -deltaToNearestCorner.x : deltaToNearestCorner.x;
      deltaToNearestCorner.y = circleRelativePosition.y > 0 ? -deltaToNearestCorner.y : deltaToNearestCorner.y;

      float penetrationDepth = radiusA - distanceToNearestCorner + EPSILON;
      *penetrationOut = Vector2Scale(deltaToNearestCorner, penetrationDepth);
      *penetrationOut = Vector2Rotate(*penetrationOut, rotationB);
      return true;
    }
  }

  return false;
}
