#pragma once
#include "processor/register.h"

#define MEMORY_SIZE 65536

struct ProcessState {
  struct RegistersState registers;
  unsigned char memory[MEMORY_SIZE];
};

void stepProcess(struct ProcessState* state);
