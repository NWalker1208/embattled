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


void DrawRobot(const PhysicsWorld* physicsWorld, const Robot* robot, Color baseColor);


int main(void) {
  SimulationArguments simulationArguments = {
    .robotCount = 2,
    .robots = {
      { .physicsBodyIndex = 0, .energyRemaining = ROBOT_INITIAL_ENERGY },
      { .physicsBodyIndex = 1, .energyRemaining = ROBOT_INITIAL_ENERGY },
    },
    .physicsWorld = {
      .boundary = (Rectangle){ .x = -ARENA_WIDTH / 2, .y = -ARENA_HEIGHT / 2, .width = ARENA_WIDTH, .height = ARENA_HEIGHT },
      .bodyCount = 2,
      .bodies = {
        { .radius = ROBOT_RADIUS },
        { .radius = ROBOT_RADIUS },
      }
    },
    .timeScale = 1.0,
    .shouldStop = false,
  };
  pthread_mutex_t* simulationMutex = &simulationArguments.mutex;
  if (pthread_mutex_init(simulationMutex, NULL)) {
    fprintf(stderr, "Failed to initialize simulation.\n");
    exit(1);
  }

  pthread_t simulationThread;
  pthread_create(&simulationThread, NULL, StartSimulation, &simulationArguments);

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
      pthread_mutex_lock(simulationMutex);
      for (unsigned int i = 0; i < simulationArguments.robotCount; i++) {
        DrawRobot(&simulationArguments.physicsWorld, &simulationArguments.robots[i], BLUE);
      }
      pthread_mutex_unlock(simulationMutex);
    } EndMode2D();
    EndDrawing();
  }

  CloseWindow();

  printf("Stopping simulation thread\n");
  simulationArguments.shouldStop = true;
  pthread_join(simulationThread, NULL);
  printf("Physics thread stopped\n");
  pthread_mutex_destroy(simulationMutex);

  return 0;
}


void DrawRobot(const PhysicsWorld* physicsWorld, const Robot* robot, Color baseColor) {
  const PhysicsBody* body = &physicsWorld->bodies[robot->physicsBodyIndex];
  Vector2 position = { body->position.x, body->position.y };
  double rotation = body->rotation;
  DrawCircleV(position, ROBOT_RADIUS, baseColor);
  DrawLineEx(position, (Vector2){ position.x + cos(rotation) * ROBOT_RADIUS, position.y + sin(rotation) * ROBOT_RADIUS }, 3.0, RED);
}
