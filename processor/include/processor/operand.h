#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "processor/register.h"

// Describes how the operands are laid out in memory for a given opcode.
typedef struct OperandLayout {
  unsigned char numBytes : 3; // The number of bytes used to store all parameters (0-4).
  bool hasRegA : 1; // Whether register A is present.
  bool hasRegB : 1; // Whether register B is present.
  bool hasRegC : 1; // Whether register C is present.
  unsigned char numImmABits : 5; // The number of bits used to store immediate value A (0, 4, 8, or 16).
  bool hasImmB : 1; // Whether immediate value B (16-bit) is present.
} OperandLayout;

#define _OPERAND_LAYOUT(_numBytes, _hasRegA, _hasRegB, _hasRegC, _numImmABits, _hasImmB) \
  (struct ProcessorOperandLayout){ \
    .numBytes=(_numBytes), \
    .hasRegA=(_hasRegA), \
    .hasRegB=(_hasRegB), \
    .hasRegC=(_hasRegC), \
    .numImmABits=(_numImmABits), \
    .hasImmB=(_hasImmB) \
  }
// 0 bytes
#define OPERAND_LAYOUT_NONE             _OPERAND_LAYOUT(0, false, false, false, 0, false)
// 1 byte
#define OPERAND_LAYOUT_REGA             _OPERAND_LAYOUT(1, true, false, false, 0, false)
#define OPERAND_LAYOUT_REGA_REGB        _OPERAND_LAYOUT(1, true, true, false, 0, false)
// 2 bytes
#define OPERAND_LAYOUT_IMMA16           _OPERAND_LAYOUT(2, false, false, false, 16, false)
#define OPERAND_LAYOUT_REGA_IMMA8       _OPERAND_LAYOUT(2, true, false, false, 8, false)
#define OPERAND_LAYOUT_REGA_REGB_IMMA4  _OPERAND_LAYOUT(2, true, true, false, 4, false)
#define OPERAND_LAYOUT_REGA_REGB_REGC   _OPERAND_LAYOUT(2, true, true, true, 0, false)
// 3 bytes
#define OPERAND_LAYOUT_IMMA8_IMMB16     _OPERAND_LAYOUT(3, false, false, false, 8, true)
#define OPERAND_LAYOUT_REGA_IMMA16      _OPERAND_LAYOUT(3, true, false, false, 16, false)
#define OPERAND_LAYOUT_REGA_REGB_IMMA16 _OPERAND_LAYOUT(3, true, true, false, 16, false)
// 4 bytes
#define OPERAND_LAYOUT_IMMA16_IMMB16    _OPERAND_LAYOUT(4, false, false, false, 16, true)

// The operands for an instruction to execute.
typedef struct InstructionOperands {
  Register regA;
  Register regB;
  Register regC;
  uint16_t immA;
  uint16_t immB;
} InstructionOperands;
