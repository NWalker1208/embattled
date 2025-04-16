#include "arena/raycast.h"
#include <raymath.h>
#include <assert.h>


float checkRaycastWithBody(const PhysicsBody* body, Vector2 origin, Vector2 direction);
float checkRaycastWithCircleCollider(Vector2 position, float radius, Vector2 origin, Vector2 direction);
float checkRaycastWithRectangleCollider(Vector2 position, Vector2 widthHeight, Vector2 origin, Vector2 direction);
float checkRaycastWithBoundary(const PhysicsWorld* world, Vector2 origin, Vector2 direction);


RaycastResult ComputeRaycast(const PhysicsWorld* world, Vector2 origin, Vector2 direction) {
  RaycastResult nearestResult = { .distance = INFINITY, .type = INTERSECTION_NONE, .bodyIndex = -1 };
  
  direction = Vector2Normalize(direction);
  if (direction.x == 0 && direction.y == 0) {
    return nearestResult;
  }

  // Check for ray intersection with each of the physics bodies
  double distance;
  for (unsigned int i = 0; i < world->bodyCount; i++) {
    distance = checkRaycastWithBody(&world->bodies[i], origin, direction);
    if (distance < nearestResult.distance) {
      nearestResult.distance = distance;
      nearestResult.type = INTERSECTION_BODY;
      nearestResult.bodyIndex = i;
    }
  }

  // Check for ray intersection with each of the world boundaries
  distance = checkRaycastWithBoundary(world, origin, direction);
  if (distance < nearestResult.distance) {
    nearestResult.distance = distance;
    nearestResult.type = INTERSECTION_BOUNDARY;
    nearestResult.bodyIndex = -1;
  }

  return nearestResult;
}


float checkRaycastWithBody(const PhysicsBody* body, Vector2 origin, Vector2 direction) {
  switch (body->collider.kind) {
    case PHYSICS_COLLIDER_CIRCLE:
      return checkRaycastWithCircleCollider(body->position, body->collider.radius, origin, direction);
    case PHYSICS_COLLIDER_RECTANGLE:
      return checkRaycastWithRectangleCollider(body->position, body->collider.widthHeight, origin, direction);
  }

  assert(false);
  return INFINITY;
}

float checkRaycastWithCircleCollider(Vector2 position, float radius, Vector2 origin, Vector2 direction) {
  // Compute the intersection point between the ray and the circle
  Vector2 relativePosition = Vector2Subtract(position, origin);
  float projectedDistanceAlongRay = relativePosition.x * direction.x + relativePosition.y * direction.y;
  float sqrDistanceBetweenPositionAndProjectedPosition = Vector2LengthSqr(relativePosition) - projectedDistanceAlongRay * projectedDistanceAlongRay;
  float sqrDistanceBetweenProjectedPositionAndIntersections = radius * radius - sqrDistanceBetweenPositionAndProjectedPosition;

  if (sqrDistanceBetweenProjectedPositionAndIntersections < 0) {
    return INFINITY; // The ray does not intersect with the collider
  }

  float distanceBetweenProjectedPositionAndIntersections = sqrt(sqrDistanceBetweenProjectedPositionAndIntersections);
  if (distanceBetweenProjectedPositionAndIntersections > fabs(projectedDistanceAlongRay)) {
    return 0; // The ray origin is inside the collider
  }

  if (projectedDistanceAlongRay < 0) {
    return INFINITY; // The collider is behind the origin of the ray
  }

  float firstIntersectionDistance = projectedDistanceAlongRay - distanceBetweenProjectedPositionAndIntersections;
  assert(firstIntersectionDistance >= 0);
  return firstIntersectionDistance;
}

float checkRaycastWithBoundary(const PhysicsWorld* world, Vector2 origin, Vector2 direction) {
  float leftRightBoundaryDistance;
  if (world->boundary.x <= origin.x && direction.x < 0) {
    leftRightBoundaryDistance = (world->boundary.x - origin.x) / direction.x;
  } else if (world->boundary.x + world->boundary.width >= origin.x && direction.x > 0) {
    leftRightBoundaryDistance = (world->boundary.x + world->boundary.width - origin.x) / direction.x;
  } else if (world->boundary.x > origin.x || world->boundary.x + world->boundary.width < origin.x) {
    return 0; // Origin is outside the boundaries
  }
  assert(leftRightBoundaryDistance >= 0);

  float topBottomBoundaryDistance;
  if (world->boundary.y <= origin.y && direction.y < 0) {
    topBottomBoundaryDistance = (world->boundary.y - origin.y) / direction.y;
  } else if (world->boundary.y + world->boundary.height >= origin.y && direction.y > 0) {
    topBottomBoundaryDistance = (world->boundary.y + world->boundary.height - origin.y) / direction.y;
  } else if (world->boundary.y > origin.y || world->boundary.y + world->boundary.height < origin.y) {
    return 0; // Origin is outside the boundaries
  }
  assert(topBottomBoundaryDistance >= 0);

  return fminf(leftRightBoundaryDistance, topBottomBoundaryDistance);
}
