#include "arena/raycast.h"
#include <raymath.h>

RaycastResult ComputeRaycast(PhysicsWorld* world, Vector2 origin, Vector2 direction) {
  RaycastResult nearestResult = { .distance = INFINITY, .type = INTERSECTION_NONE, .bodyIndex = -1 };
  
  direction = Vector2Normalize(direction);
  if (direction.x == 0 && direction.y == 0) {
    return nearestResult;
  }

  // Check for ray intersection with each of the physics bodies
  for (unsigned int i = 0; i < world->bodyCount; i++) {
    // Compute the intersection point between the ray and the circular physics body
    Vector2 relativeBodyPosition = Vector2Subtract(world->bodies[i].position, origin);
    float projectedBodyDistanceAlongRay = relativeBodyPosition.x * direction.x + relativeBodyPosition.y * direction.y;
    float sqrDistanceBetweenProjectedPositionAndRay = Vector2LengthSqr(relativeBodyPosition) - projectedBodyDistanceAlongRay * projectedBodyDistanceAlongRay;
    float bodyRadius = world->bodies[i].radius;
    float sqrDistanceBetweenProjectedPositionAndIntersections = bodyRadius * bodyRadius - sqrDistanceBetweenProjectedPositionAndRay;

    if (sqrDistanceBetweenProjectedPositionAndIntersections < 0) {
      continue; // No intersection
    }

    float distanceBetweenProjectedPositionAndIntersections = sqrt(sqrDistanceBetweenProjectedPositionAndIntersections);
    if (distanceBetweenProjectedPositionAndIntersections > fabs(projectedBodyDistanceAlongRay)) {
      // The ray origin is inside the body
      nearestResult.distance = 0;
      nearestResult.type = INTERSECTION_BODY;
      nearestResult.bodyIndex = i;
      continue;
    }

    float intersectionDistance = projectedBodyDistanceAlongRay - distanceBetweenProjectedPositionAndIntersections;
    if (intersectionDistance < 0) {
      continue; // No intersection
    }

    if (intersectionDistance < nearestResult.distance) {
      nearestResult.distance = intersectionDistance;
      nearestResult.type = INTERSECTION_BOUNDARY;
      nearestResult.bodyIndex = i;
    }
  }

  // Check for ray intersection with each of the world boundaries
  if (world->boundary.x <= origin.x && direction.x < 0) {
    double distanceToBoundary = (world->boundary.x - origin.x) / direction.x;
    if (distanceToBoundary < nearestResult.distance) {
      nearestResult.distance = distanceToBoundary;
      nearestResult.type = INTERSECTION_BOUNDARY;
      nearestResult.bodyIndex = -1;
    }
  } else if (world->boundary.x + world->boundary.width >= origin.x && direction.x > 0) {
    double distanceToBoundary = (world->boundary.x + world->boundary.width - origin.x) / direction.x;
    if (distanceToBoundary < nearestResult.distance) {
      nearestResult.distance = distanceToBoundary;
      nearestResult.type = INTERSECTION_BOUNDARY;
      nearestResult.bodyIndex = -1;
    }
  } else if (world->boundary.x > origin.x || world->boundary.x + world->boundary.width < origin.x) {
    nearestResult.distance = 0;
    nearestResult.type = INTERSECTION_BOUNDARY;
    nearestResult.bodyIndex = -1;
  }

  if (world->boundary.y <= origin.y && direction.y < 0) {
    double distanceToBoundary = (world->boundary.y - origin.y) / direction.y;
    if (distanceToBoundary < nearestResult.distance) {
      nearestResult.distance = distanceToBoundary;
      nearestResult.type = INTERSECTION_BOUNDARY;
      nearestResult.bodyIndex = -1;
    }
  } else if (world->boundary.y + world->boundary.height >= origin.y && direction.y > 0) {
    double distanceToBoundary = (world->boundary.y + world->boundary.height - origin.y) / direction.y;
    if (distanceToBoundary < nearestResult.distance) {
      nearestResult.distance = distanceToBoundary;
      nearestResult.type = INTERSECTION_BOUNDARY;
      nearestResult.bodyIndex = -1;
    }
  } else if (world->boundary.y > origin.y || world->boundary.y + world->boundary.height < origin.y) {
    nearestResult.distance = 0;
    nearestResult.type = INTERSECTION_BOUNDARY;
    nearestResult.bodyIndex = -1;
  }

  return nearestResult;
}
