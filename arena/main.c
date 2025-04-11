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

#define ARENA_WIDTH 500.0
#define ARENA_HEIGHT 500.0
#define ARENA_BORDER_THICKNESS 4
#define ARENA_MARGIN 10
#define ARENA_MIN_SCREEN_WIDTH 100

#define STATE_PANEL_WIDTH 200
#define STATE_PANEL_HEIGHT 200
#define STATE_PANEL_MARGIN 10

#define ROBOT_RADIUS 50.0


const Rectangle ARENA_DRAW_RECT = {
  .x=-ARENA_WIDTH / 2 - ARENA_BORDER_THICKNESS, .y=-ARENA_HEIGHT / 2 - ARENA_BORDER_THICKNESS,
  .width=ARENA_WIDTH + ARENA_BORDER_THICKNESS * 2, .height=ARENA_HEIGHT + ARENA_BORDER_THICKNESS * 2
};

float dpi = -1;
Font primaryFont = { 0 };


bool TryReadParseAndAssembleFile(const char* path, TextContents* textOut, AssemblyProgram* assemblyProgramOut, unsigned char* memoryOut);

void UpdateDpiAndMinWindowSize();

void DrawRobot(const PhysicsWorld* physicsWorld, const Robot* robot, Color baseColor);
void DrawRobotWeapon(const Robot* robot);
void DrawRobotSensors(const Robot* robot);
void DrawRobotProcessState(const Robot* robot, size_t index, Vector2 position);


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

  // Setup simulation
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
    .timeScale = 0,
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

  // Setup window
  int windowWidth = 800, windowHeight = 450;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  InitWindow(windowWidth, windowHeight, "Hello Raylib");
  UpdateDpiAndMinWindowSize();
  SetWindowSize(dpi * (STATE_PANEL_WIDTH + STATE_PANEL_HEIGHT * 2), dpi * (STATE_PANEL_HEIGHT * 2));
  SetTargetFPS(60);

  // Setup cameras
  Camera2D uiCamera = { 0 };
  Camera2D arenaCamera = { 0 };

  // Load resources
  primaryFont = LoadFontEx("resources/fonts/Roboto_Mono/static/RobotoMono-SemiBold.ttf", 100, NULL, 0);

  // Enter main loop
  while (!WindowShouldClose()) {
    windowWidth = GetScreenWidth(); windowHeight = GetScreenHeight();
    UpdateDpiAndMinWindowSize();
    
    // Update camera based on current window size
    uiCamera.zoom = dpi;

    float statePanelScreenWidth = dpi * STATE_PANEL_WIDTH;
    float arenaScreenWidth = windowWidth - statePanelScreenWidth;
    arenaCamera.offset = (Vector2){ statePanelScreenWidth + (arenaScreenWidth / 2), windowHeight / 2 };
    arenaCamera.zoom = fmin((arenaScreenWidth - 2 * ARENA_MARGIN) / ARENA_DRAW_RECT.width, (windowHeight - 2 * ARENA_MARGIN) / ARENA_DRAW_RECT.height);

    // Handle input
    pthread_mutex_lock(simulationMutex); {
      // Temporary code for manipulating time scale
      if (IsKeyPressed(KEY_ZERO)) {
        simulationArguments.timeScale = 0;
      } else if (IsKeyPressed(KEY_ONE)) {
        simulationArguments.timeScale = 1;
      } else if (IsKeyPressed(KEY_TWO)) {
        simulationArguments.timeScale = NEUTRAL_TIME_SCALE / 4;
      } else if (IsKeyPressed(KEY_THREE)) {
        simulationArguments.timeScale = NEUTRAL_TIME_SCALE / 2;
      } else if (IsKeyPressed(KEY_FOUR)) {
        simulationArguments.timeScale = NEUTRAL_TIME_SCALE;
      } else if (IsKeyPressed(KEY_FIVE)) {
        simulationArguments.timeScale = NEUTRAL_TIME_SCALE * 2;
      } else if (IsKeyPressed(KEY_SIX)) {
        simulationArguments.timeScale = NEUTRAL_TIME_SCALE * 4;
      } else if (IsKeyPressed(KEY_SEVEN)) {
        simulationArguments.timeScale = UINT_MAX;
      }

      if (IsKeyPressed(KEY_SPACE)) {
        simulationArguments.forceStep = true;
      }
    } pthread_mutex_unlock(simulationMutex);

    // Draw frame
    BeginDrawing();
    pthread_mutex_lock(simulationMutex); {
      ClearBackground(RAYWHITE);

      // Draw arena and robots
      BeginMode2D(arenaCamera); {
        DrawRectangleLinesEx(ARENA_DRAW_RECT, ARENA_BORDER_THICKNESS, GRAY);
        for (unsigned int i = 0; i < simulationArguments.robotCount; i++) {
          DrawRobotSensors(&simulationArguments.robots[i]);
        }
        for (unsigned int i = 0; i < simulationArguments.robotCount; i++) {
          DrawRobotWeapon(&simulationArguments.robots[i]);
        }
        for (unsigned int i = 0; i < simulationArguments.robotCount; i++) {
          DrawRobot(&simulationArguments.physicsWorld, &simulationArguments.robots[i], PURPLE);
        }
      } EndMode2D();

      // Draw user interface
      BeginMode2D(uiCamera); {
        DrawRectangleRec((Rectangle){ 0.0f, 0.0f, STATE_PANEL_WIDTH, windowHeight / dpi }, LIGHTGRAY);
        for (unsigned int i = 0; i < simulationArguments.robotCount; i++) {
          DrawRobotProcessState(&simulationArguments.robots[i], i, (Vector2){ 0, STATE_PANEL_HEIGHT * i });
        }
      } EndMode2D();
    } pthread_mutex_unlock(simulationMutex);
    EndDrawing();
  }

  UnloadFont(primaryFont);
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


void UpdateDpiAndMinWindowSize() {
  float newDpi = GetWindowScaleDPI().x;
  if (newDpi != dpi) {
    dpi = newDpi;
    SetWindowMinSize(dpi * (ARENA_MARGIN * 2 + STATE_PANEL_WIDTH + ARENA_MIN_SCREEN_WIDTH), dpi * fmax(ARENA_MARGIN * 2, STATE_PANEL_HEIGHT * 2));
  }
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

void DrawRobotProcessState(const Robot* robot, size_t index, Vector2 position) {
  Vector2 topLeft = { position.x + STATE_PANEL_MARGIN, position.y + STATE_PANEL_MARGIN };
  Vector2 bottomRight = { position.x + STATE_PANEL_WIDTH - STATE_PANEL_MARGIN, position.y + STATE_PANEL_HEIGHT - STATE_PANEL_MARGIN };

  // Draw title
  char title[100];
  sprintf_s(title, sizeof(title), "Robot %zu", index + 1);
  DrawTextEx(primaryFont, title, topLeft, 20, 1.0, BLACK);
  topLeft.y += 25;
  
  // Compute state info
  float energyPercent = fmax(0.0f, fmin(1.0f, (float)robot->energyRemaining / ROBOT_INITIAL_ENERGY));
  struct Instruction nextInstruction;
  fetchInstruction(robot->processState.memory, robot->processState.registers.ip, &nextInstruction);
  const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[nextInstruction.opcode];

  // Draw energy remaining
  Vector2 labelSize = MeasureTextEx(primaryFont, "Energy", 15, 1.0);
  DrawTextEx(primaryFont, "Energy", (Vector2){ topLeft.x, topLeft.y }, 15, 1.0, BLACK);
  
  float energyBarY =  topLeft.y + (labelSize.y / 2);
  float energyBarLeftX = topLeft.x + labelSize.x + 10;
  float energyBarRightX = bottomRight.x;
  DrawLineEx((Vector2){ energyBarLeftX, energyBarY }, (Vector2){ energyBarRightX, energyBarY }, 4, energyPercent > 0 ? BLACK : RED);
  DrawLineEx((Vector2){ energyBarLeftX, energyBarY }, (Vector2){ energyBarLeftX + (energyBarRightX - energyBarLeftX) * energyPercent, energyBarY }, 4, GREEN);
  topLeft.y += 20;

  // Draw next instruction
  labelSize = MeasureTextEx(primaryFont, "Next instruction", 15, 1.0);
  DrawTextEx(primaryFont, "Next instruction:", (Vector2){ topLeft.x, topLeft.y }, 15, 1.0, BLACK);
  topLeft.y += 20;

  float maxLabelWidth = 0;
  maxLabelWidth = fmax(maxLabelWidth, MeasureTextEx(primaryFont, "Opcode", 12, 1.0).x);
  DrawTextEx(primaryFont, "Opcode:", (Vector2){ topLeft.x + 10, topLeft.y }, 12, 1.0, BLACK);
  maxLabelWidth = fmax(maxLabelWidth, MeasureTextEx(primaryFont, "Reg. A", 12, 1.0).x);
  DrawTextEx(primaryFont, "Reg. A:", (Vector2){ topLeft.x + 10, topLeft.y + 15 }, 12, 1.0, BLACK);
  maxLabelWidth = fmax(maxLabelWidth, MeasureTextEx(primaryFont, "Reg. B", 12, 1.0).x);
  DrawTextEx(primaryFont, "Reg. B:", (Vector2){ topLeft.x + 10, topLeft.y + 30 }, 12, 1.0, BLACK);
  maxLabelWidth = fmax(maxLabelWidth, MeasureTextEx(primaryFont, "Imm. Value:", 12, 1.0).x);
  DrawTextEx(primaryFont, "Imm. Value:", (Vector2){ topLeft.x + 10, topLeft.y + 45 }, 12, 1.0, BLACK);

  DrawTextEx(primaryFont, opcodeInfo->name, (Vector2){ topLeft.x + 10 + maxLabelWidth + 10, topLeft.y }, 12, 1.0, BLACK);
  DrawTextEx(primaryFont, opcodeInfo->parameterLayout.hasRegA ? REGISTER_NAMES[nextInstruction.parameters.registerA] : "N/A", (Vector2){ topLeft.x + 10 + maxLabelWidth + 10, topLeft.y + 15 }, 12, 1.0, BLACK);
  DrawTextEx(primaryFont, opcodeInfo->parameterLayout.hasRegB ? REGISTER_NAMES[nextInstruction.parameters.registerB] : "N/A", (Vector2){ topLeft.x + 10 + maxLabelWidth + 10, topLeft.y + 30 }, 12, 1.0, BLACK);
  char immValue[5] = "N/A";
  if (opcodeInfo->parameterLayout.numImmBits > 0) {
    sprintf_s(immValue, sizeof(immValue), "%04x", nextInstruction.parameters.immediate.u16);
  }
  DrawTextEx(primaryFont, immValue, (Vector2){ topLeft.x + 10 + maxLabelWidth + 10, topLeft.y + 45 }, 12, 1.0, BLACK);
}
