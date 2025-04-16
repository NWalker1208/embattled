#pragma once
#include "arena/physics.h"

// The type of intersection that occurred as the result of a raycast.
typedef enum {
  INTERSECTION_NONE = 0,
  INTERSECTION_BODY = 1,
  INTERSECTION_BOUNDARY = 2,
} IntersectionType;

// The result of performing a raycast.
typedef struct {
  // The distance from the ray's origin to the point of intersection.
  // +inf if no intersection occurred.
  float distance;
  // The type of intersection that occurred.
  IntersectionType type;
  // The index of the body that was intersected, if type == INTERSECTION_BODY.
  // Otherwise, -1.
  int bodyIndex;
} RaycastResult;

RaycastResult ComputeRaycast(const PhysicsWorld* world, Vector2 origin, Vector2 direction);
