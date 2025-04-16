#include "arena/raycast.h"
#include <raymath.h>


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
