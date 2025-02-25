#pragma once
#include "processor/opcode.h"
#include "processor/register.h"

// Layout:
// 0: | opcode (8 bits) |
// 1: | opcode (8 bits) | reg A (4 bits) | reg B (4 bits) |
// 2: | opcode (8 bits) | reg A (4 bits) | imm (4 bits)   |
// 3: | opcode (8 bits) | imm (8 bits)                    |
// 4: | opcode (8 bits) | imm (16 bits)                                                     |
// (last 2 bits of opcode represent number of bytes in addition to opcode)

struct Instruction {
  enum Opcode opcode;
  enum Register registerA;
  enum Register registerB;
  unsigned short immediateValue;
};

// Next step: Refactor opcodes to store info for each opcode in a struct, remove hand-written values
