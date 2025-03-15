#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <raylib.h>
#include "arena/simulation.h"

#define WINDOW_MARGIN 20

#define ARENA_WIDTH 500.0
#define ARENA_HEIGHT 500.0
#define ARENA_BORDER_THICKNESS 4

#define ROBOT_RADIUS 50.0


const Rectangle ARENA_DRAW_RECT = {
  .x=-ARENA_WIDTH / 2 - ARENA_BORDER_THICKNESS, .y=-ARENA_HEIGHT / 2 - ARENA_BORDER_THICKNESS,
  .width=ARENA_WIDTH + ARENA_BORDER_THICKNESS * 2, .height=ARENA_HEIGHT + ARENA_BORDER_THICKNESS * 2
};


void DrawRobot(const PhysicsWorldState* state, Robot robot, Color baseColor);


int main(void) {
  const int robotCount = 2;
  Robot robots[2] = {
    { .physicsBodyIndex = 0 },
    { .physicsBodyIndex = 1 },
  };

  PhysicsBodyDefinition physicsBodyDefinitions[] = {
    { .radius = ROBOT_RADIUS },
    { .radius = ROBOT_RADIUS },
  };
  PhysicsBodyState physicsBodyStates[2] = { 0 };
  SimulationArguments simulationArguments = {
    .robotCount = robotCount,
    .robots = robots,
    .physicsWorldDefinition = {
      .upperLeftBound = (Vector2D){ -ARENA_WIDTH / 2, -ARENA_HEIGHT / 2 },
      .lowerRightBound = (Vector2D){ ARENA_WIDTH / 2, ARENA_HEIGHT / 2 },
      .bodyCount = robotCount,
      .bodies = physicsBodyDefinitions,
    },
    .physicsWorldState = {
      .bodyStates = physicsBodyStates,
    },
    .stateMutex = PTHREAD_MUTEX_INITIALIZER,
    .shouldStop = false,
  };

  pthread_t physicsThread;
  pthread_create(&physicsThread, NULL, StartSimulation, &simulationArguments);

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
      pthread_mutex_lock(&simulationArguments.stateMutex);
      for (unsigned int i = 0; i < 2; i++) {
        DrawRobot(&simulationArguments.physicsWorldState, robots[i], BLUE);
      }
      pthread_mutex_unlock(&simulationArguments.stateMutex);
    } EndMode2D();
    EndDrawing();
  }

  CloseWindow();

  printf("Stopping physics thread\n");
  simulationArguments.shouldStop = true;
  pthread_join(physicsThread, NULL);
  printf("Physics thread stopped\n");

  return 0;
}


void DrawRobot(const PhysicsWorldState* state, Robot robot, Color baseColor) {
  const PhysicsBodyState* bodyState = &state->bodyStates[robot.physicsBodyIndex];
  Vector2 position = { bodyState->position.x, bodyState->position.y };
  DrawCircleV(position, ROBOT_RADIUS, baseColor);
  DrawLineEx(position, (Vector2){ position.x + cos(bodyState->rotation) * ROBOT_RADIUS, position.y + sin(bodyState->rotation) * ROBOT_RADIUS }, 3.0, RED);
}
