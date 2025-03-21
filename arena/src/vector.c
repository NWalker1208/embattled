#include <math.h>
#include "arena/vector.h"

#define EPSILON 0.0001

double VectorGetSqrMagnitude(Vector2D vector) {
  return vector.x * vector.x + vector.y * vector.y;
}

double VectorGetMagnitude(Vector2D vector) {
  return sqrt(VectorGetSqrMagnitude(vector));
}

Vector2D VectorNormalize(Vector2D vector) {
  double magnitude = VectorGetMagnitude(vector);
  if (magnitude < EPSILON) { return (Vector2D) { 0, 0 }; }
  return (Vector2D) { vector.x / magnitude, vector.y / magnitude };
}

Vector2D VectorSum(Vector2D vectorA, Vector2D vectorB) {
  return (Vector2D) { vectorA.x + vectorB.x, vectorA.y + vectorB.y };
}

Vector2D VectorDifference(Vector2D vectorA, Vector2D vectorB) {
  return (Vector2D) { vectorA.x - vectorB.x, vectorA.y - vectorB.y };
}

bool VectorIsZero(Vector2D vector) {
  return vector.x == 0 && vector.y == 0;
}
