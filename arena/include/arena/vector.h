#pragma once
#include <stdbool.h>

// A 2-dimension vector of doubles.
typedef struct {
  double x;
  double y;
} Vector2D;

// Gets the square magnitude of the vector.
double VectorGetSqrMagnitude(Vector2D vector);

// Gets the magnitude of the vector.
double VectorGetMagnitude(Vector2D vector);

// Normalizes the vector to a magnitude of 1.
// If the vector has a magnitude close to 0, returns the zero vector.
Vector2D VectorNormalize(Vector2D vector);

// Computes the sum of two vectors.
Vector2D VectorSum(Vector2D vectorA, Vector2D vectorB);

// Computes the difference between two vectors (vectorA - vectorB).
Vector2D VectorDifference(Vector2D vectorA, Vector2D vectorB);

// Checks if the vector is the zero vector.
bool VectorIsZero(Vector2D vector);
