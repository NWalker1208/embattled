#pragma once
#include <stdint.h>
#include "processor/register.h"

// The operands for an instruction to execute.
typedef struct InstructionOperands {
  Register regA;
  Register regB;
  Register regC;
  uint16_t immA;
  uint16_t immB;
} InstructionOperands;
