#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <limits.h>
#include <raylib.h>
#include "utilities/file.h"
#include "utilities/text.h"
#include "assembler/assembly.h"
#include "assembler/assemble.h"
#include "parser/parse.h"
#include "processor/instruction.h"
#include "arena/simulation.h"
#if defined(PLATFORM_WEB)
#include "emscripten.h"
#else
#include "arena/worker.h"
#define USE_SIMULATION_WORKER
#endif


#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION "330"
#else
#define GLSL_VERSION "100"
#endif

#define BACKGROUND_COLOR WHITE
#define LAYER_COUNT 6

#define ARENA_WIDTH 1000.0
#define ARENA_HEIGHT 1000.0
#define ARENA_BORDER_THICKNESS 4
#define ARENA_MARGIN 10
#define ARENA_MIN_SCREEN_WIDTH 100

#define OBSTACLE_WIDTH (ROBOT_RADIUS)
#define OBSTACLE_HEIGHT (ARENA_HEIGHT / 2)

#define STATE_PANEL_WIDTH 250
#define STATE_PANEL_HEIGHT 300
#define STATE_PANEL_MARGIN 10

#define ROBOT_WHEEL_OFFSET 45.0
#define ROBOT_WHEEL_RADIUS 40.0
#define ROBOT_WHEEL_WIDTH 20.0

#define ROBOT_TURRET_LENGTH (ROBOT_RADIUS * 1.2)
#define ROBOT_TURRET_WIDTH 30.0
#define ROBOT_TURRET_OFFSET (ROBOT_RADIUS - ROBOT_TURRET_LENGTH)

#define ROBOT_WEAPON_DRAW_STEPS (ROBOT_WEAPON_COOLDOWN_STEPS / 2)

#define SHADOW_BLUR_SIZE 10

const Color SHADOW_TINT = { .r = 255, .g = 255, .b = 255, .a = 96 };
const Color ROBOT_COLORS[] = {
  PURPLE,
  GREEN
};

float dpi = -1;
Font primaryFont = { 0 };


bool TryReadParseAndAssembleFile(const char* path, TextContents* textOut, AssemblyProgram* assemblyProgramOut, unsigned char* memoryOut);

void UpdateDpiAndMinWindowSize();

void DrawArenaForeground();
void DrawRobot(const PhysicsWorld* physicsWorld, const Robot* robot, Color baseColor, unsigned int layer);
void DrawStaticBody(const PhysicsBody* body, unsigned int layer);
void DrawStatePanel(const Robot* robot, size_t index, Vector2 position);

// TODO:
// - Fix time scale in web build.
// - Load/reload assembly files created from UI inputs. https://emscripten.org/docs/api_reference/Filesystem-API.html#FS.writeFile


int main(int argc, char* argv[]) {
  // Get command line arguments
  if (argc < 1 || argc > 3) {
    fprintf(stderr, "Usage: %s [<assembly file A> [<assembly file B>]]\n", argv[0]);
    return 1;
  }
  char* assemblyFilePathA = argc >= 2 ? argv[1] : NULL;
  char* assemblyFilePathB = argc >= 3 ? argv[2] : NULL;

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
  Simulation simulation = {
    .physicsWorld.boundary = (Rectangle){
      .x = -ARENA_WIDTH / 2, .y = -ARENA_HEIGHT / 2,
      .width = ARENA_WIDTH, .height = ARENA_HEIGHT
    },
    .timer = InitTimer(SIMULATION_DEFAULT_TICKS_PER_SECOND, 100),
  };

  AddRobotToSimulation(&simulation, (Vector2){ -ARENA_WIDTH / 2 + ROBOT_RADIUS * 2, 0 }, 0);
  AddRobotToSimulation(&simulation, (Vector2){ ARENA_WIDTH / 2 - ROBOT_RADIUS * 2, 0 }, M_PI);

  AddObstacleToSimulation(&simulation, (Vector2){ -ARENA_WIDTH / 4, 0 }, 0, (PhysicsCollider){
    .kind = PHYSICS_COLLIDER_RECTANGLE,
    .widthHeight = { OBSTACLE_WIDTH, OBSTACLE_HEIGHT }
  });
  AddObstacleToSimulation(&simulation, (Vector2){ ARENA_WIDTH / 4, 0 }, 0, (PhysicsCollider){
    .kind = PHYSICS_COLLIDER_RECTANGLE,
    .widthHeight = { OBSTACLE_WIDTH, OBSTACLE_HEIGHT }
  });

  // Load assembly programs into robot memory
  memcpy(simulation.robots[0].processState.memory, initialMemoryA, sizeof(initialMemoryA));
  memcpy(simulation.robots[1].processState.memory, initialMemoryB, sizeof(initialMemoryB));

  PrepSimulation(&simulation);

  #ifdef USE_SIMULATION_WORKER
  // Setup worker to run simulation
  Worker simulationWorker = { 0 };
  if (!TryInitWorker(&simulationWorker, (void (*)(void*))UpdateSimulation, &simulation)) {
    fprintf(stderr, "Failed to initialize simulation worker.\n");
    exit(1);
  }

  StartWorker(&simulationWorker);
  #endif

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
  if (!IsFontValid(primaryFont)) {
    fprintf(stderr, "Failed to load font.\n");
    exit(1);
  }

  Shader hBlurShader = LoadShader(NULL, "resources/shaders/glsl"GLSL_VERSION"/hblur.glsl");
  if (!IsShaderValid(hBlurShader)) {
    fprintf(stderr, "Failed to load hblur shader.\n");
    exit(1);
  }
  int hBlurRenderWidthLocation = GetShaderLocation(hBlurShader, "renderWidth");
  int hBlurRenderHeightLocation = GetShaderLocation(hBlurShader, "renderHeight");
  int hBlurSizeLocation = GetShaderLocation(hBlurShader, "blurSize");

  Shader vBlurShader = LoadShader(NULL, "resources/shaders/glsl"GLSL_VERSION"/vblur.glsl");
  if (!IsShaderValid(vBlurShader)) {
    fprintf(stderr, "Failed to load vblur shader.\n");
    exit(1);
  }
  int vBlurRenderWidthLocation = GetShaderLocation(vBlurShader, "renderWidth");
  int vBlurRenderHeightLocation = GetShaderLocation(vBlurShader, "renderHeight");
  int vBlurSizeLocation = GetShaderLocation(vBlurShader, "blurSize");

  // Initialize render texture for shadows
  RenderTexture2D shadowsTarget0 = LoadRenderTexture(windowWidth, windowHeight);
  RenderTexture2D shadowsTarget1 = LoadRenderTexture(windowWidth, windowHeight);

  // Enter main loop
  while (!WindowShouldClose()) {
    windowWidth = GetScreenWidth(); windowHeight = GetScreenHeight();
    UpdateDpiAndMinWindowSize();

    // Update shadow render textures
    if (windowWidth != shadowsTarget0.texture.width || windowHeight != shadowsTarget0.texture.height) {
      UnloadRenderTexture(shadowsTarget0);
      UnloadRenderTexture(shadowsTarget1);
      shadowsTarget0 = LoadRenderTexture(windowWidth, windowHeight);
      shadowsTarget1 = LoadRenderTexture(windowWidth, windowHeight);
    }
    
    // Update camera based on current window size
    uiCamera.zoom = dpi;

    float statePanelScreenWidth = dpi * STATE_PANEL_WIDTH;
    float arenaScreenWidth = windowWidth - statePanelScreenWidth;
    arenaCamera.offset = (Vector2){ statePanelScreenWidth + (arenaScreenWidth / 2), windowHeight / 2 };
    arenaCamera.zoom = fmin((arenaScreenWidth - 2 * ARENA_MARGIN) / (ARENA_WIDTH + ARENA_BORDER_THICKNESS * 2),
                            (windowHeight - 2 * ARENA_MARGIN) / (ARENA_HEIGHT + ARENA_BORDER_THICKNESS * 2));

    // Update blur shader uniforms
    float blurSize = SHADOW_BLUR_SIZE * arenaCamera.zoom;

    SetShaderValue(hBlurShader, hBlurRenderWidthLocation, &windowWidth, SHADER_UNIFORM_INT);
    SetShaderValue(hBlurShader, hBlurRenderHeightLocation, &windowHeight, SHADER_UNIFORM_INT);
    SetShaderValue(hBlurShader, hBlurSizeLocation, &blurSize, SHADER_UNIFORM_FLOAT);

    SetShaderValue(vBlurShader, vBlurRenderWidthLocation, &windowWidth, SHADER_UNIFORM_INT);
    SetShaderValue(vBlurShader, vBlurRenderHeightLocation, &windowHeight, SHADER_UNIFORM_INT);
    SetShaderValue(vBlurShader, vBlurSizeLocation, &blurSize, SHADER_UNIFORM_FLOAT);

    // Handle input
    #ifdef USE_SIMULATION_WORKER
    pthread_mutex_lock(&simulationWorker.stateMutex); {
    #endif
      // Temporary code for manipulating time scale
      if (IsKeyPressed(KEY_ZERO)) {
        simulation.timer.ticksPerSec = 0;
      } else if (IsKeyPressed(KEY_ONE)) {
        simulation.timer.ticksPerSec = 1;
      } else if (IsKeyPressed(KEY_TWO)) {
        simulation.timer.ticksPerSec = SIMULATION_DEFAULT_TICKS_PER_SECOND / 4;
      } else if (IsKeyPressed(KEY_THREE)) {
        simulation.timer.ticksPerSec = SIMULATION_DEFAULT_TICKS_PER_SECOND / 2;
      } else if (IsKeyPressed(KEY_FOUR)) {
        simulation.timer.ticksPerSec = SIMULATION_DEFAULT_TICKS_PER_SECOND;
      } else if (IsKeyPressed(KEY_FIVE)) {
        simulation.timer.ticksPerSec = SIMULATION_DEFAULT_TICKS_PER_SECOND * 2;
      } else if (IsKeyPressed(KEY_SIX)) {
        simulation.timer.ticksPerSec = SIMULATION_DEFAULT_TICKS_PER_SECOND * 4;
      } else if (IsKeyPressed(KEY_SEVEN)) {
        simulation.timer.ticksPerSec = INT64_MAX;
      }

      if (IsKeyPressed(KEY_TAB) || IsKeyPressedRepeat(KEY_TAB)) {
        simulation.forceStep = true;
      }
    #ifdef USE_SIMULATION_WORKER
    } pthread_mutex_unlock(&simulationWorker.stateMutex);
    #endif

    #ifndef USE_SIMULATION_WORKER
    // Update simulation
    UpdateSimulation(&simulation);
    #endif

    // Draw frame
    BeginDrawing();
    #ifdef USE_SIMULATION_WORKER
    pthread_mutex_lock(&simulationWorker.stateMutex); {
    #endif
      // Draw shadows to first stage shadow texture
      BeginTextureMode(shadowsTarget0); {
        ClearBackground(WHITE);

        BeginMode2D(arenaCamera); {
          // Draw layer 0 (shadows)
          for (unsigned int i = 0; i < simulation.robotCount; i++) {
            DrawRobot(&simulation.physicsWorld, &simulation.robots[i], ROBOT_COLORS[i], 0);
          }
          for (unsigned int i = 0; i < simulation.physicsWorld.bodyCount; i++) {
            const PhysicsBody* body = &simulation.physicsWorld.bodies[i];
            if (body->isStatic) {
              DrawStaticBody(body, 0);
            }
          }
        } EndMode2D();
      } EndTextureMode();

      // Draw first stage shadow texture to second stage with partial blur
      BeginTextureMode(shadowsTarget1); BeginShaderMode(hBlurShader); {
        DrawTextureRec(shadowsTarget0.texture, (Rectangle){ 0, 0, shadowsTarget0.texture.width, -shadowsTarget0.texture.height }, (Vector2){ 0, 0 }, WHITE);
      } EndShaderMode(); EndTextureMode();

      // Clear screen
      ClearBackground(BACKGROUND_COLOR);

      // Draw second stage shadow texture to screen with full blur
      BeginShaderMode(vBlurShader); {
        DrawTextureRec(shadowsTarget1.texture, (Rectangle){ 0, 0, shadowsTarget1.texture.width, -shadowsTarget1.texture.height }, (Vector2){ 0, 0 }, SHADOW_TINT);
      } EndShaderMode();

      // Draw scene
      BeginMode2D(arenaCamera); {
        // TODO: Draw some kind of arena backdrop
        // Draw layers 1+
        for (unsigned int layer = 1; layer < LAYER_COUNT; layer++) {
          for (unsigned int i = 0; i < simulation.robotCount; i++) {
            DrawRobot(&simulation.physicsWorld, &simulation.robots[i], ROBOT_COLORS[i], layer);
          }
          for (unsigned int i = 0; i < simulation.physicsWorld.bodyCount; i++) {
            const PhysicsBody* body = &simulation.physicsWorld.bodies[i];
            if (body->isStatic) {
              DrawStaticBody(body, layer);
            }
          }
        }
        DrawArenaForeground();
      } EndMode2D();

      // Draw user interface
      BeginMode2D(uiCamera); {
        DrawRectangleRec((Rectangle){ 0.0f, 0.0f, STATE_PANEL_WIDTH, windowHeight / dpi }, LIGHTGRAY);
        for (unsigned int i = 0; i < simulation.robotCount; i++) {
          DrawStatePanel(&simulation.robots[i], i, (Vector2){ 0, STATE_PANEL_HEIGHT * i });
        }
      } EndMode2D();
    #ifdef USE_SIMULATION_WORKER
    } pthread_mutex_unlock(&simulationWorker.stateMutex);
    #endif
    EndDrawing();
  }

  UnloadFont(primaryFont);
  UnloadShader(hBlurShader);
  UnloadShader(vBlurShader);
  UnloadRenderTexture(shadowsTarget0);
  UnloadRenderTexture(shadowsTarget1);
  CloseWindow();

  #ifdef USE_SIMULATION_WORKER
  printf("Stopping simulation thread\n");
  StopWorker(&simulationWorker);
  printf("Simulation thread stopped\n");
  DestroyWorker(&simulationWorker);
  #endif

  return 0;
}


bool TryReadParseAndAssembleFile(const char* path, TextContents* textOut, AssemblyProgram* assemblyProgramOut, unsigned char* memoryOut) {
  char* chars;
  size_t fileLength;
  if (path != NULL) {
    if ((chars = ReadAllText(path, &fileLength)) == NULL) {
      fprintf(stderr, "Failed to read assembly file.\n");
      return false;
    }
  } else {
    chars = (char*)malloc(1);
    chars[0] = '\0';
    fileLength = 0;
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
  #if defined(PLATFORM_WEB)
  float newDpi = emscripten_get_device_pixel_ratio();
  #else
  float newDpi = GetWindowScaleDPI().x;
  #endif
  if (newDpi != dpi) {
    dpi = newDpi;
    SetWindowMinSize(dpi * (ARENA_MARGIN * 2 + STATE_PANEL_WIDTH + ARENA_MIN_SCREEN_WIDTH), dpi * fmax(ARENA_MARGIN * 2, STATE_PANEL_HEIGHT * 2));
  }
}


void DrawArenaForeground() {
  // Mask outside of arena in white
  DrawRectangleRec((Rectangle){
    .x=-ARENA_WIDTH / 2 - 100, .y=-ARENA_HEIGHT / 2 - 100,
    .width=ARENA_WIDTH + 200, .height=100
  }, BACKGROUND_COLOR);
  DrawRectangleRec((Rectangle){
    .x=-ARENA_WIDTH / 2 - 100, .y=ARENA_HEIGHT / 2,
    .width=ARENA_WIDTH + 200, .height=100
  }, BACKGROUND_COLOR);
  DrawRectangleRec((Rectangle){
    .x=-ARENA_WIDTH / 2 - 100, .y=-ARENA_HEIGHT / 2 - 100,
    .width=100, .height=ARENA_HEIGHT + 200
  }, BACKGROUND_COLOR);
  DrawRectangleRec((Rectangle){
    .x=ARENA_WIDTH / 2, .y=-ARENA_HEIGHT / 2 - 100,
    .width=100, .height=ARENA_HEIGHT + 200
  }, BACKGROUND_COLOR);

  // Draw border
  DrawRectangleLinesEx((Rectangle){
    .x=-ARENA_WIDTH / 2 - ARENA_BORDER_THICKNESS, .y=-ARENA_HEIGHT / 2 - ARENA_BORDER_THICKNESS,
    .width=ARENA_WIDTH + ARENA_BORDER_THICKNESS * 2, .height=ARENA_HEIGHT + ARENA_BORDER_THICKNESS * 2
  }, ARENA_BORDER_THICKNESS, GRAY);
}


void DrawRobot(const PhysicsWorld* physicsWorld, const Robot* robot, Color baseColor, unsigned int layer) {
  const PhysicsBody* body = &physicsWorld->bodies[robot->physicsBodyIndex];
  Vector2 position = { body->position.x, body->position.y };
  double rotation = body->rotation;

  switch (layer) {
    case 0: {
      // Shadows
      DrawRectanglePro(
        (Rectangle){ .x=position.x, .y=position.y + 7, .width=ROBOT_WHEEL_RADIUS * 2 + 6, .height=ROBOT_WHEEL_WIDTH + 6 },
        (Vector2){ ROBOT_WHEEL_RADIUS + 3, ROBOT_WHEEL_WIDTH / 2 - ROBOT_WHEEL_OFFSET + 3 },
        rotation * RAD2DEG, BLACK);
      DrawRectanglePro(
        (Rectangle){ .x=position.x, .y=position.y + 7, .width=ROBOT_WHEEL_RADIUS * 2 + 6, .height=ROBOT_WHEEL_WIDTH + 6 },
        (Vector2){ ROBOT_WHEEL_RADIUS + 3, ROBOT_WHEEL_WIDTH / 2 + ROBOT_WHEEL_OFFSET + 3 },
        rotation * RAD2DEG, BLACK);
      DrawCircleV((Vector2){ position.x, position.y + 2 }, ROBOT_RADIUS + 3, BLACK);
    } break;
    case 1: {
      // Wheels
      DrawRectanglePro(
        (Rectangle){ .x=position.x, .y=position.y + 5, .width=ROBOT_WHEEL_RADIUS * 2, .height=ROBOT_WHEEL_WIDTH },
        (Vector2){ ROBOT_WHEEL_RADIUS, ROBOT_WHEEL_WIDTH / 2 - ROBOT_WHEEL_OFFSET },
        rotation * RAD2DEG, DARKGRAY);
      DrawRectanglePro(
        (Rectangle){ .x=position.x, .y=position.y + 5, .width=ROBOT_WHEEL_RADIUS * 2, .height=ROBOT_WHEEL_WIDTH },
        (Vector2){ ROBOT_WHEEL_RADIUS, ROBOT_WHEEL_WIDTH / 2 + ROBOT_WHEEL_OFFSET },
        rotation * RAD2DEG, DARKGRAY);
    } break;
    case 2: {
      // Weapon fire
      int weaponDrawStepsRemaining = robot->weaponCooldownRemaining - (ROBOT_WEAPON_COOLDOWN_STEPS - ROBOT_WEAPON_DRAW_STEPS);
      if (weaponDrawStepsRemaining > 0) {
        DrawLineEx(robot->lastWeaponFire.start, robot->lastWeaponFire.end, 4.0, ColorAlpha(RED, weaponDrawStepsRemaining / (float)ROBOT_WEAPON_DRAW_STEPS));
      }
    } break;
    case 3: {
      // Sensor
      DrawLineEx(robot->lastSensorReading.start, robot->lastSensorReading.end, 1.0, ColorAlpha(BLUE, 0.5f));
    } break;
    case 4: {
      // Body
      DrawCircleV(position, ROBOT_RADIUS, robot->energyRemaining > 0 ? baseColor : ColorLerp(baseColor, GRAY, 0.75f));
    } break;
    case 5: {
      // Turret
      Color turretColor = ColorLerp(baseColor, BLACK, 0.75f);
      DrawRectanglePro(
        (Rectangle){ .x=position.x, .y=position.y, .width=ROBOT_TURRET_LENGTH, .height=ROBOT_TURRET_WIDTH},
        (Vector2){ -ROBOT_TURRET_OFFSET, ROBOT_TURRET_WIDTH / 2 },
        rotation * RAD2DEG, turretColor);
      DrawCircleV(
        (Vector2){ position.x + ROBOT_TURRET_OFFSET * cos(rotation), position.y + ROBOT_TURRET_OFFSET * sin(rotation) },
        ROBOT_TURRET_WIDTH / 2, turretColor);
      DrawCircleV(
        (Vector2){ position.x + ROBOT_TURRET_OFFSET * cos(rotation), position.y + ROBOT_TURRET_OFFSET * sin(rotation) },
        ROBOT_TURRET_WIDTH / 4, robot->weaponCooldownRemaining > 0 || robot->energyRemaining <= 0 ? GRAY : RED);
    } break;
  }
}

void DrawStaticBody(const PhysicsBody* body, unsigned int layer) {
  Vector2 position = body->position;

  if (body->collider.kind == PHYSICS_COLLIDER_RECTANGLE) {
    float width = body->collider.widthHeight.x;
    float height = body->collider.widthHeight.y;
    
    switch (layer) {
      case 0: {
        // Shadow
        DrawRectanglePro(
          (Rectangle){ .x=body->position.x, .y=body->position.y + 2, .width = width + 6, .height = height + 6 },
          (Vector2){ .x = width/2 + 3, .y = height/2 + 3 },
          body->rotation * RAD2DEG, BLACK);
      } break;
      case 5: {
        // Body
        DrawRectanglePro(
          (Rectangle){ .x=body->position.x, .y=body->position.y, .width = width, .height = height },
          (Vector2){ .x = width/2, .y = height/2 },
          body->rotation * RAD2DEG, DARKGRAY);
      } break;
    }
  } else if (body->collider.kind == PHYSICS_COLLIDER_CIRCLE) {
    switch (layer) {
      case 0: {
        // Shadow
        DrawCircleV((Vector2){ position.x, position.y + 2 }, body->collider.radius + 3, BLACK);
      } break;
      case 5: {
        // Body
        DrawCircleV(position, body->collider.radius, DARKGRAY);
      } break;
    }
  }
}

void DrawStatePanel(const Robot* robot, size_t index, Vector2 position) {
  char buffer[1024];
  Vector2 topLeft = { position.x + STATE_PANEL_MARGIN, position.y + STATE_PANEL_MARGIN };
  Vector2 bottomRight = { position.x + STATE_PANEL_WIDTH - STATE_PANEL_MARGIN, position.y + STATE_PANEL_HEIGHT - STATE_PANEL_MARGIN };
  
  // Compute state info
  float energyPercent = fmax(0.0f, fmin(1.0f, (float)robot->energyRemaining / ROBOT_INITIAL_ENERGY));
  const struct ProcessState* processState = &robot->processState;

  struct Instruction nextInstruction;
  fetchInstruction(processState->memory, processState->registers.ip, &nextInstruction);
  
  const struct OpcodeInfo* opcodeInfo = getOpcodeInfo(nextInstruction.opcode);
  bool hasRegA = opcodeInfo->layout.hasRegA;
  bool hasRegB = opcodeInfo->layout.hasRegB;
  bool hasRegC = opcodeInfo->layout.hasRegC;
  unsigned char numImmABits = opcodeInfo->layout.numImmABits;
  bool hasImmB = opcodeInfo->layout.hasImmB;

  // Draw title
  snprintf(buffer, sizeof(buffer), "Robot %zu", index + 1);
  DrawTextEx(primaryFont, buffer, topLeft, 22, 1.0, BLACK);
  topLeft.y += 30;

  // Draw energy remaining
  float labelWidth = MeasureTextEx(primaryFont, "Energy", 18, 1.0).x;
  DrawTextEx(primaryFont, "Energy", (Vector2){ topLeft.x, topLeft.y }, 18, 1.0, BLACK);
  
  float energyBarY =  topLeft.y + 9;
  float energyBarLeftX = topLeft.x + labelWidth + 10;
  float energyBarRightX = bottomRight.x;
  DrawLineEx((Vector2){ energyBarLeftX, energyBarY }, (Vector2){ energyBarRightX, energyBarY }, 4, energyPercent > 0 ? BLACK : RED);
  DrawLineEx((Vector2){ energyBarLeftX, energyBarY }, (Vector2){ energyBarLeftX + (energyBarRightX - energyBarLeftX) * energyPercent, energyBarY }, 4, GREEN);
  topLeft.y += 20;

  // Draw registers
  DrawTextEx(primaryFont, "Registers:", (Vector2){ topLeft.x, topLeft.y }, 18, 1.0, BLACK);
  topLeft.y += 20;

  snprintf(buffer, sizeof(buffer),
    "ip: %04x  sp: %04x  rt: %04x\n"
    "x0: %04x  x1: %04x  x2: %04x\n"
    "x3: %04x  x4: %04x  x5: %04x\n"
    "x6: %04x  x7: %04x  x8: %04x\n"
    "x9: %04x  x10:%04x  x11:%04x",
    processState->registers.ip, processState->registers.sp, processState->registers.rt,
    processState->registers.x0, processState->registers.x1, processState->registers.x2,
    processState->registers.x3, processState->registers.x4, processState->registers.x5,
    processState->registers.x6, processState->registers.x7, processState->registers.x8,
    processState->registers.x9, processState->registers.x10, processState->registers.x11
  );
  DrawTextEx(primaryFont, buffer, (Vector2){ topLeft.x + 10, topLeft.y }, 15, 1.0, BLACK);
  topLeft.y += 5*15 + 4*2 + 2;

  // Draw next instruction
  DrawTextEx(primaryFont, "Next instruction:", (Vector2){ topLeft.x, topLeft.y }, 18, 1.0, BLACK);
  topLeft.y += 20;

  char immValueA[5] = "N/A";
  if (numImmABits > 8) {
    snprintf(immValueA, sizeof(immValueA), "%04x", nextInstruction.operands.immediateA.u16);
  } else if (numImmABits > 4) {
    snprintf(immValueA, sizeof(immValueA), "%02x", nextInstruction.operands.immediateA.u8);
  } else if (numImmABits > 0) {
    snprintf(immValueA, sizeof(immValueA), "%01x", nextInstruction.operands.immediateA.u4);
  }

  char immValueB[5] = "N/A";
  if (hasImmB) {
    snprintf(immValueB, sizeof(immValueB), "%04x", nextInstruction.operands.immediateB.u16);
  }

  snprintf(buffer, sizeof(buffer),
    "Opcode:       %s\n"
    "Reg. A:       %s\n"
    "Reg. B:       %s\n"
    "Reg. C:       %s\n"
    "Imm. Value A: %s\n"
    "Imm. Value B: %s",
    opcodeInfo->identifier,
    hasRegA ? getRegisterIdentifier(nextInstruction.operands.registerA) : "N/A",
    hasRegB ? getRegisterIdentifier(nextInstruction.operands.registerB) : "N/A",
    hasRegC ? getRegisterIdentifier(nextInstruction.operands.registerC) : "N/A",
    immValueA, immValueB
  );
  DrawTextEx(primaryFont, buffer, (Vector2){ topLeft.x + 10, topLeft.y }, 15, 1.0, BLACK);
  //topLeft.y += 6*15 + 5*2 + STATE_PANEL_MARGIN; // Uncomment to measure height of panel
}
