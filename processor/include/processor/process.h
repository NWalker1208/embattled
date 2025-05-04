#pragma once
#include <stdint.h>
#include "processor/register.h"

#define MEMORY_SIZE 65536

typedef struct ProcessState {
  RegistersState registers;
  uint8_t memory[MEMORY_SIZE];
} ProcessState;

void stepProcess(ProcessState* state);
