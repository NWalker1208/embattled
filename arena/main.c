#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <raylib.h>
#include "arena/sleep.h"

#define WINDOW_MARGIN 20

#define ARENA_WIDTH 500.0
#define ARENA_HEIGHT 500.0
#define ARENA_BORDER_THICKNESS 4

#define ROBOT_RADIUS 50.0
#define ROBOT_MAX_SPEED 300.0
#define ROBOT_ROT_SPEED 6.0

#define PHYSICS_TIMESTEP 0.001
#define MAX_COLLISION_ITERATIONS 32
#define EPSILON 0.0001


const Rectangle ARENA_DRAW_RECT = {
  .x=-ARENA_WIDTH / 2 - ARENA_BORDER_THICKNESS, .y=-ARENA_HEIGHT / 2 - ARENA_BORDER_THICKNESS,
  .width=ARENA_WIDTH + ARENA_BORDER_THICKNESS * 2, .height=ARENA_HEIGHT + ARENA_BORDER_THICKNESS * 2
};


typedef struct Robot {
  Vector2 position;
  float rotation;
  float forwardVelocity;
} Robot;

typedef enum ArenaCollision {
  COLLISION_NONE = 0,
  COLLISION_RIGHT = 1,
  COLLISION_BOTTOM = 2,
  COLLISION_LEFT = 4,
  COLLISION_TOP = 8
} ArenaCollision;


void drawRobot(Robot robot, Color baseColor);

void* physicsLoop(void* vargp);
void updateRobotPositions(Robot robots[], unsigned int robotCount);
bool checkIfTwoRobotsColliding(Robot robotA, Robot robotB);
ArenaCollision checkIfRobotCollidingWithArena(Robot robot); // Returns bit mask of which walls robot is colliding with


bool stopping = false;
Robot robots[2] = { 0 };
const int robotCount = 2;
pthread_mutex_t robotMutex = PTHREAD_MUTEX_INITIALIZER;


int main(void) {
  pthread_t physicsThread;
  pthread_create(&physicsThread, NULL, physicsLoop, NULL);

  int windowWidth = 800;
  int windowHeight = 450;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(windowWidth, windowHeight, "Hello Raylib");
  SetWindowMinSize(WINDOW_MARGIN, WINDOW_MARGIN);

  Camera2D camera = { 0 };
  camera.target = (Vector2){ 0, 0 };
  camera.offset = (Vector2){ ARENA_DRAW_RECT.width / 2, ARENA_DRAW_RECT.height / 2 };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    windowWidth = GetScreenWidth(); windowHeight = GetScreenHeight();

    // Update camera based on current window size
    camera.offset = (Vector2){ windowWidth / 2, windowHeight / 2 };
    camera.zoom = fmin((windowWidth - WINDOW_MARGIN) / ARENA_DRAW_RECT.width, (windowHeight - WINDOW_MARGIN) / ARENA_DRAW_RECT.height);

    // Draw frame
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(camera); {
      DrawRectangleLinesEx(ARENA_DRAW_RECT, ARENA_BORDER_THICKNESS, GRAY);
      pthread_mutex_lock(&robotMutex);
      for (unsigned int i = 0; i < 2; i++) {
        drawRobot(robots[i], RED);
      }
      pthread_mutex_unlock(&robotMutex);
    } EndMode2D();
    EndDrawing();
  }

  CloseWindow();

  printf("Stopping physics thread\n");
  stopping = true;
  pthread_join(physicsThread, NULL);
  printf("Physics thread stopped\n");

  return 0;
}


void* physicsLoop(void* vargp) {
  while (!stopping) {
    pthread_mutex_lock(&robotMutex);

    // Check controls
    if (IsKeyDown(KEY_LEFT)) {
      robots[0].rotation -= ROBOT_ROT_SPEED * PHYSICS_TIMESTEP;
    }
    
    if (IsKeyDown(KEY_RIGHT)) {
      robots[0].rotation += ROBOT_ROT_SPEED * PHYSICS_TIMESTEP;
    }

    robots[0].forwardVelocity = 0;

    if (IsKeyDown(KEY_UP)) {
      robots[0].forwardVelocity += ROBOT_MAX_SPEED;
    }

    if (IsKeyDown(KEY_DOWN)) {
      robots[0].forwardVelocity -= ROBOT_MAX_SPEED;
    }

    updateRobotPositions(robots, 2);
    pthread_mutex_unlock(&robotMutex);

    psleep(1); // TODO: Make number of physics steps before sleep depend on time elapsed
  }
  return NULL;
}


void drawRobot(Robot robot, Color baseColor) {
  DrawCircleV(robot.position, ROBOT_RADIUS, BLUE);
  DrawLineEx(robot.position, (Vector2){ robot.position.x + cos(robot.rotation) * ROBOT_RADIUS, robot.position.y + sin(robot.rotation) * ROBOT_RADIUS }, 3.0, baseColor);
}


void updateRobotPositions(Robot robots[], unsigned int robotCount) {
  // Update robot positions based on current velocity
  for (unsigned int i = 0; i < robotCount; i++) {
    robots[i].position.x += cos(robots[i].rotation) * robots[i].forwardVelocity * PHYSICS_TIMESTEP;
    robots[i].position.y += sin(robots[i].rotation) * robots[i].forwardVelocity * PHYSICS_TIMESTEP;
  }

  // Check for and resolve collisions iteratively
  bool foundCollision = true;
  for (unsigned int iterations = 0; foundCollision && iterations < MAX_COLLISION_ITERATIONS; iterations++) {
    foundCollision = false;

    // Collisions between robots
    for (unsigned int i = 0; i < robotCount - 1; i++) {
      for (unsigned int j = i + 1; j < robotCount; j++) {
        if (checkIfTwoRobotsColliding(robots[i], robots[j])) {
          foundCollision = true;

          // Resolve collision
          float deltaX = robots[j].position.x - robots[i].position.x;
          float deltaY = robots[j].position.y - robots[i].position.y;
          float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
          float penetrationDepth = 2 * ROBOT_RADIUS - distance;
          // Normalize vector between robots, avoiding division by zero
          if (distance > EPSILON) {
            deltaX /= distance;
            deltaY /= distance;
          } else {
            deltaX = 1.0;
            deltaY = 0.0;
          }
          // Move robots apart each by half of the penetration depth
          robots[i].position.x -= deltaX * penetrationDepth / 2.0f;
          robots[i].position.y -= deltaY * penetrationDepth / 2.0f;
          robots[j].position.x += deltaX * penetrationDepth / 2.0f;
          robots[j].position.y += deltaY * penetrationDepth / 2.0f;
        }
      }
    }

    // Collisions between robots and the arena
    for (unsigned int i = 0; i < robotCount; i++) {
      ArenaCollision collisionMask = checkIfRobotCollidingWithArena(robots[i]);
      if (collisionMask != COLLISION_NONE) {
        foundCollision = true;

        // Resolve collision
        if (collisionMask & COLLISION_RIGHT) {
          robots[i].position.x = ARENA_WIDTH / 2 - ROBOT_RADIUS;
        } else if (collisionMask & COLLISION_LEFT) {
          robots[i].position.x = -ARENA_WIDTH / 2 + ROBOT_RADIUS;
        }
        if (collisionMask & COLLISION_BOTTOM) {
          robots[i].position.y = ARENA_HEIGHT / 2 - ROBOT_RADIUS;
        } else if (collisionMask & COLLISION_TOP) {
          robots[i].position.y = -ARENA_HEIGHT / 2 + ROBOT_RADIUS;
        }
      }
    }
  }
}

bool checkIfTwoRobotsColliding(Robot robotA, Robot robotB) {
  const float minSquareDistance = (2 * ROBOT_RADIUS) * (2 * ROBOT_RADIUS);
  float deltaX = robotA.position.x - robotB.position.x;
  float deltaY = robotA.position.y - robotB.position.y;
  float squareDistance = deltaX * deltaX + deltaY * deltaY;
  return squareDistance < minSquareDistance;
}

ArenaCollision checkIfRobotCollidingWithArena(Robot robot) {
  ArenaCollision collisionMask = COLLISION_NONE;
  if (robot.position.x + ROBOT_RADIUS > ARENA_WIDTH / 2) {
    collisionMask |= COLLISION_RIGHT;
  } else if (robot.position.x - ROBOT_RADIUS < -ARENA_WIDTH / 2) {
    collisionMask |= COLLISION_LEFT;
  }
  if (robot.position.y + ROBOT_RADIUS > ARENA_HEIGHT / 2) {
    collisionMask |= COLLISION_BOTTOM;
  } else if (robot.position.y - ROBOT_RADIUS < -ARENA_HEIGHT / 2) {
    collisionMask |= COLLISION_TOP;
  }
  return collisionMask;
}
