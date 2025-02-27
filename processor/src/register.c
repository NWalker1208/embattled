#include <stdio.h>
#include "processor/register.h"

const char* REGISTER_NAMES[REGISTER_COUNT] = {
  "nl",
  "ip",
  "sp",
  "ac",
  "x0",
  "x1",
  "x2",
  "x3",
  "x4",
  "x5",
  "x6",
  "x7",
  "x8",
  "x9",
  "x10",
  "x11",
};

enum Register nibbleToRegister(unsigned char nibble) {
  if (nibble >= 0 && nibble < REGISTER_COUNT) {
    return nibble;
  } else {
    return NL;
  }
}

unsigned short* getRegisterPtr(struct RegistersState* state, enum Register reg) {
  switch (reg) {
    case NL:
      return 0;
    case IP:
      return &state->ip;
    case SP:
      return &state->sp;
    case AC:
      return &state->ac;
    case X0:
      return &state->x0;
    case X1:
      return &state->x1;
    case X2:
      return &state->x2;
    case X3:
      return &state->x3;
    case X4:
      return &state->x4;
    case X5:
      return &state->x5;
    case X6:
      return &state->x6;
    case X7:
      return &state->x7;
    case X8:
      return &state->x8;
    case X9:
      return &state->x9;
    case X10:
      return &state->x10;
    case X11:
      return &state->x11;
    default:
      return 0;
  }
}

void printRegistersState(struct RegistersState* state) {
  printf("ip = 0x%04x    sp = 0x%04x    ac = 0x%04x\n", state->ip, state->sp, state->ac);
  printf("x0 = 0x%04x    x1 = 0x%04x    x2 = 0x%04x    x3 = 0x%04x\n", state->x0, state->x1, state->x2, state->x3);
  printf("x4 = 0x%04x    x5 = 0x%04x    x6 = 0x%04x    x7 = 0x%04x\n", state->x4, state->x5, state->x6, state->x7);
  printf("x8 = 0x%04x    x9 = 0x%04x    x10 = 0x%04x   x11 = 0x%04x\n", state->x8, state->x9, state->x10, state->x11);
}
