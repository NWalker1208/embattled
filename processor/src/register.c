#include <stdio.h>
#include "processor/register.h"

const char* REGISTER_NAMES[REGISTER_COUNT] = {
  "nl",
  "ip",
  "sp",
  "rt",
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

Register nibbleToRegister(uint8_t nibble) {
  if (nibble < REGISTER_COUNT) {
    return nibble;
  } else {
    return REGISTER_NL;
  }
}

uint16_t* getRegisterPtr(RegistersState* state, Register reg) {
  switch (reg) {
    case REGISTER_NL:
      return NULL;
    case REGISTER_IP:
      return &state->ip;
    case REGISTER_SP:
      return &state->sp;
    case REGISTER_RT:
      return &state->rt;
    case REGISTER_X0:
      return &state->x0;
    case REGISTER_X1:
      return &state->x1;
    case REGISTER_X2:
      return &state->x2;
    case REGISTER_X3:
      return &state->x3;
    case REGISTER_X4:
      return &state->x4;
    case REGISTER_X5:
      return &state->x5;
    case REGISTER_X6:
      return &state->x6;
    case REGISTER_X7:
      return &state->x7;
    case REGISTER_X8:
      return &state->x8;
    case REGISTER_X9:
      return &state->x9;
    case REGISTER_X10:
      return &state->x10;
    case REGISTER_X11:
      return &state->x11;
    default:
      return NULL;
  }
}

void printRegistersState(const RegistersState* state) {
  printf("ip = 0x%04x    sp = 0x%04x    rt = 0x%04x\n", state->ip, state->sp, state->rt);
  printf("x0 = 0x%04x    x1 = 0x%04x    x2 = 0x%04x    x3 = 0x%04x\n", state->x0, state->x1, state->x2, state->x3);
  printf("x4 = 0x%04x    x5 = 0x%04x    x6 = 0x%04x    x7 = 0x%04x\n", state->x4, state->x5, state->x6, state->x7);
  printf("x8 = 0x%04x    x9 = 0x%04x    x10 = 0x%04x   x11 = 0x%04x\n", state->x8, state->x9, state->x10, state->x11);
}
