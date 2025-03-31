#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <raylib.h>
#include "utilities/file.h"
#include "utilities/text.h"
#include "assembler/assembly.h"
#include "assembler/assemble.h"
#include "parser/parse.h"
#include "processor/instruction.h"
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


bool TryReadParseAndAssembleFile(const char* path, TextContents* textOut, AssemblyProgram* assemblyProgramOut, unsigned char* memoryOut);

void DrawRobot(const PhysicsWorld* physicsWorld, const Robot* robot, Color baseColor);
void DrawRobotWeapon(const Robot* robot);
void DrawRobotSensors(const Robot* robot);


int main(int argc, char* argv[]) {
  // Get command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <assembly file A> <assembly file B>\n", argv[0]);
    return 1;
  }
  char* assemblyFilePathA = argv[1];
  char* assemblyFilePathB = argv[2];

  // Load, parse, and assemble assembly files
  TextContents textA, textB;
  AssemblyProgram programA, programB;
  unsigned char initialMemoryA[MEMORY_SIZE], initialMemoryB[MEMORY_SIZE];

  if (!TryReadParseAndAssembleFile(assemblyFilePathA, &textA, &programA, initialMemoryA)) {
    return 1;
  }

  if (!TryReadParseAndAssembleFile(assemblyFilePathB, &textB, &programB, initialMemoryB)) {
    return 1;
  }

  // Start simulation
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

  memcpy(simulationArguments.robots[0].processState.memory, initialMemoryA, sizeof(initialMemoryA));
  memcpy(simulationArguments.robots[1].processState.memory, initialMemoryB, sizeof(initialMemoryB));

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
        DrawRobotSensors(&simulationArguments.robots[i]);
      }
      for (unsigned int i = 0; i < simulationArguments.robotCount; i++) {
        DrawRobotWeapon(&simulationArguments.robots[i]);
      }
      for (unsigned int i = 0; i < simulationArguments.robotCount; i++) {
        DrawRobot(&simulationArguments.physicsWorld, &simulationArguments.robots[i], PURPLE);
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


bool TryReadParseAndAssembleFile(const char* path, TextContents* textOut, AssemblyProgram* assemblyProgramOut, unsigned char* memoryOut) {
  char* chars;
  size_t fileLength;
  if ((chars = ReadAllText(path, &fileLength)) == NULL) {
    fprintf(stderr, "Failed to read assembly file.\n");
    return false;
  }

  *textOut = InitTextContents(&chars, fileLength);
  ParsingErrorList parsingErrors = { 0 };
  if (!TryParseAssemblyProgram(textOut, assemblyProgramOut, &parsingErrors)) {
    fprintf(stderr, "Failed to parse assembly file.\n");
    for (size_t i = 0; i < parsingErrors.errorCount; i++) {
      fprintf(stderr, "Line %zu, column %zu: %s.\n",
        parsingErrors.errors[i].sourceSpan.start.line + 1,
        parsingErrors.errors[i].sourceSpan.start.column + 1,
        parsingErrors.errors[i].message);
    }
    return false;
  }

  AssemblingError assemblingError = { 0 };
  if (!TryAssembleProgram(textOut, assemblyProgramOut, memoryOut, &assemblingError)) {
    fprintf(stderr, "Failed to assemble program from file.\n");
    fprintf(stderr, "Line %zu, column %zu: %s.",
      assemblingError.sourceSpan.start.line + 1,
      assemblingError.sourceSpan.start.column + 1,
      assemblingError.message);
    return false;
  }

  return true;
}


void DrawRobot(const PhysicsWorld* physicsWorld, const Robot* robot, Color baseColor) {
  const PhysicsBody* body = &physicsWorld->bodies[robot->physicsBodyIndex];
  Vector2 position = { body->position.x, body->position.y };
  double rotation = body->rotation;
  DrawCircleV(position, ROBOT_RADIUS, robot->energyRemaining > 0 ? baseColor : ColorLerp(baseColor, GRAY, 0.75f));
  DrawLineEx(position, (Vector2){ position.x + cos(rotation) * ROBOT_RADIUS, position.y + sin(rotation) * ROBOT_RADIUS }, 3.0, BLACK);
}

void DrawRobotWeapon(const Robot* robot) {
  if (robot->weaponCooldownRemaining > 0) {
    DrawLineEx(robot->lastWeaponFire.start, robot->lastWeaponFire.end, 4.0, ColorAlpha(RED, robot->weaponCooldownRemaining / (float)ROBOT_WEAPON_COOLDOWN_STEPS));
  }
}

void DrawRobotSensors(const Robot* robot) {
  for (unsigned int i = 0; i < ROBOT_NUM_SENSORS; i++) {
    DrawLineEx(robot->sensors[i].start, robot->sensors[i].end, 1.0, BLUE);
  }
}
