#include "raylib.h"
#include "math.h"

#define WINDOW_MARGIN 20

#define ARENA_WIDTH 500.0
#define ARENA_HEIGHT 500.0

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

  while (!WindowShouldClose()) {
    windowWidth = GetScreenWidth(); windowHeight = GetScreenHeight();

    // Update camera based on current window size
    camera.offset = (Vector2){ windowWidth / 2, windowHeight / 2 };
    camera.zoom = fmin((windowWidth - WINDOW_MARGIN) / ARENA_WIDTH, (windowHeight - WINDOW_MARGIN) / ARENA_HEIGHT);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Hello Raylib!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
