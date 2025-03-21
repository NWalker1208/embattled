#include <math.h>
#include "arena/raycast.h"

RaycastResult ComputeRaycast(PhysicsWorld* world, Vector2D origin, Vector2D direction) {
  RaycastResult nearestResult = { .distance = INFINITY, .type = INTERSECTION_NONE };
  
  direction = VectorNormalize(direction);
  if (VectorIsZero(direction)) {
    return nearestResult;
  }

  // Check for ray intersection with each of the physics bodies
  for (unsigned int i = 0; i < world->bodyCount; i++) {
    // Compute the intersection point between the ray and the circular physics body
    Vector2D relativeBodyPosition = VectorDifference(world->bodies[i].position, origin);
    double projectedBodyDistanceAlongRay = relativeBodyPosition.x * direction.x + relativeBodyPosition.y * direction.y;
    double sqrDistanceBetweenProjectedPositionAndRay = VectorGetSqrMagnitude(relativeBodyPosition) - projectedBodyDistanceAlongRay * projectedBodyDistanceAlongRay;
    double bodyRadius = world->bodies[i].radius;
    double sqrDistanceBetweenProjectedPositionAndIntersections = bodyRadius * bodyRadius - sqrDistanceBetweenProjectedPositionAndRay;

    if (sqrDistanceBetweenProjectedPositionAndIntersections < 0) {
      continue; // No intersection
    }

    double distanceBetweenProjectedPositionAndIntersections = sqrt(sqrDistanceBetweenProjectedPositionAndIntersections);
    if (distanceBetweenProjectedPositionAndIntersections > fabs(projectedBodyDistanceAlongRay)) {
      // The ray origin is inside the body
      nearestResult.distance = 0;
      nearestResult.type = INTERSECTION_BODY;
      continue;
    }

    double intersectionDistance = projectedBodyDistanceAlongRay - distanceBetweenProjectedPositionAndIntersections;
    if (intersectionDistance < 0) {
      continue; // No intersection
    }

    if (intersectionDistance < nearestResult.distance) {
      nearestResult.distance = intersectionDistance;
      nearestResult.type = INTERSECTION_BOUNDARY;
    }
  }

  // Check for ray intersection with each of the world boundaries
  if (world->upperLeftBound.x <= origin.x && direction.x < 0) {
    double distanceToBoundary = (world->upperLeftBound.x - origin.x) / direction.x;
    if (distanceToBoundary < nearestResult.distance) {
      nearestResult.distance = distanceToBoundary;
      nearestResult.type = INTERSECTION_BOUNDARY;
    }
  } else if (world->lowerRightBound.x >= origin.x && direction.x > 0) {
    double distanceToBoundary = (world->lowerRightBound.x - origin.x) / direction.x;
    if (distanceToBoundary < nearestResult.distance) {
      nearestResult.distance = distanceToBoundary;
      nearestResult.type = INTERSECTION_BOUNDARY;
    }
  } else if (world->upperLeftBound.x > origin.x || world->lowerRightBound.x < origin.x) {
    nearestResult.distance = 0;
    nearestResult.type = INTERSECTION_BOUNDARY;
  }

  if (world->upperLeftBound.y <= origin.y && direction.y < 0) {
    double distanceToBoundary = (world->upperLeftBound.y - origin.y) / direction.y;
    if (distanceToBoundary < nearestResult.distance) {
      nearestResult.distance = distanceToBoundary;
      nearestResult.type = INTERSECTION_BOUNDARY;
    }
  } else if (world->lowerRightBound.y >= origin.y && direction.y > 0) {
    double distanceToBoundary = (world->lowerRightBound.y - origin.y) / direction.y;
    if (distanceToBoundary < nearestResult.distance) {
      nearestResult.distance = distanceToBoundary;
      nearestResult.type = INTERSECTION_BOUNDARY;
    }
  } else if (world->upperLeftBound.y > origin.y || world->lowerRightBound.y < origin.y) {
    nearestResult.distance = 0;
    nearestResult.type = INTERSECTION_BOUNDARY;
  }

  return nearestResult;
}
