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

// Represents the parameters of an instruction as fetched from memory.
struct InstructionParameters {
  enum Register registerA;
  enum Register registerB;
  unsigned short immediateValue;
};

// An instruction that has been fetched from memory.
struct Instruction {
  enum Opcode opcode;
  struct InstructionParameters parameters;
};
