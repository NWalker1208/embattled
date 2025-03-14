#include "raylib.h"
#include "math.h"

#define WINDOW_MARGIN 20

#define ARENA_WIDTH 500.0
#define ARENA_HEIGHT 500.0

#define ROBOT_RADIUS 50.0
#define ROBOT_MAX_SPEED 5.0

#define MAX_COLLISION_ITERATIONS 32


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


void updateRobotPositions(Robot robots[], unsigned int robotCount);
bool checkIfTwoRobotsColliding(Robot robotA, Robot robotB);
ArenaCollision checkIfRobotCollidingWithArena(Robot robot); // Returns bit mask of which walls robot is colliding with


int main(void) {
  int windowWidth = 800;
  int windowHeight = 450;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(windowWidth, windowHeight, "Hello Raylib");
  SetWindowMinSize(WINDOW_MARGIN, WINDOW_MARGIN);

  Camera2D camera = { 0 };
  camera.target = (Vector2){ 0, 0 };
  camera.offset = (Vector2){ ARENA_WIDTH / 2, ARENA_HEIGHT / 2 };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  SetTargetFPS(60);

  Robot robot = { 0 };

  const Rectangle arenaRect = {
    .x=-ARENA_WIDTH / 2, .y=-ARENA_HEIGHT / 2,
    .width=ARENA_WIDTH, .height=ARENA_HEIGHT
  };

  while (!WindowShouldClose()) {
    windowWidth = GetScreenWidth(); windowHeight = GetScreenHeight();

    // Update camera based on current window size
    camera.offset = (Vector2){ windowWidth / 2, windowHeight / 2 };
    camera.zoom = fmin((windowWidth - WINDOW_MARGIN) / ARENA_WIDTH, (windowHeight - WINDOW_MARGIN) / ARENA_HEIGHT);

    // Check controls
    if (IsKeyDown(KEY_LEFT)) {
      robot.rotation -= 0.1;
    }
    
    if (IsKeyDown(KEY_RIGHT)) {
      robot.rotation += 0.1;
    }

    robot.forwardVelocity = 0;

    if (IsKeyDown(KEY_UP)) {
      robot.forwardVelocity += ROBOT_MAX_SPEED;
    }

    if (IsKeyDown(KEY_DOWN)) {
      robot.forwardVelocity -= ROBOT_MAX_SPEED;
    }

    updateRobotPositions(&robot, 1);

    // Draw frame
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(camera); {
      DrawRectangleLinesEx(arenaRect, 4, GRAY);
      DrawCircleV(robot.position, ROBOT_RADIUS, BLUE);
      DrawLineEx(robot.position, (Vector2){ robot.position.x + cos(robot.rotation) * ROBOT_RADIUS, robot.position.y + sin(robot.rotation) * ROBOT_RADIUS }, 3.0, RED);
    } EndMode2D();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}


void updateRobotPositions(Robot robots[], unsigned int robotCount) {
  // Update robot positions based on current velocity
  for (int i = 0; i < robotCount; i++) {
    robots[i].position.x += cos(robots[i].rotation) * robots[i].forwardVelocity;
    robots[i].position.y += sin(robots[i].rotation) * robots[i].forwardVelocity;
  }

  // Check for and resolve collisions iteratively
  bool foundCollision = true;
  for (int iterations = 0; foundCollision && iterations < MAX_COLLISION_ITERATIONS; iterations++) {
    foundCollision = false;

    // Collisions between robots
    for (int i = 0; i < robotCount - 1; i++) {
      for (int j = i + 1; j < robotCount; j++) {
        if (checkIfTwoRobotsColliding(robots[i], robots[j])) {
          foundCollision = true;

          // Resolve collision
          float deltaX = robots[i].position.x - robots[j].position.x;
          float deltaY = robots[i].position.y - robots[j].position.y;
          float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
          float penetrationDepth = 2 * ROBOT_RADIUS - distance;
          // Normalize vector between robots
          deltaX /= distance;
          deltaY /= distance;
          // Move robots apart each by half of the penetration depth
          robots[i].position.x -= deltaX * penetrationDepth / 2.0f;
          robots[i].position.y -= deltaY * penetrationDepth / 2.0f;
          robots[j].position.x += deltaX * penetrationDepth / 2.0f;
          robots[j].position.y += deltaY * penetrationDepth / 2.0f;
        }
      }
    }

    // Collisions between robots and the arena
    for (int i = 0; i < robotCount; i++) {
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
  } while (foundCollision);
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
