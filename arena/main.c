#include "raylib.h"

int main(void) {
  const int windowWidth = 800;
  const int windowHeight = 450;

  InitWindow(windowWidth, windowHeight, "Hello Raylib");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Hello Raylib!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
