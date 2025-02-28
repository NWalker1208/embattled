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
  enum Register registerA : 4;
  enum Register registerB : 4;
  union {
    unsigned short u16 : 16;
    unsigned short u12 : 12;
    unsigned short u8 : 8;
    unsigned short u4 : 4;

    signed short s16 : 16;
    signed short s12 : 12;
    signed short s8 : 8;
    signed short s4 : 4;
  } immediate;
};

// An instruction that has been fetched from memory.
struct Instruction {
  enum Opcode opcode;
  struct InstructionParameters parameters;
};

// Fetches an instruction from memory at the given instruction pointer.
// Increments the instruction pointer to the next instruction.
struct Instruction fetchInstruction(unsigned char* memory, unsigned short* ip);
