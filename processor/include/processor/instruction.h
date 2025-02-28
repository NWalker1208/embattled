#pragma once
#include "processor/opcode.h"
#include "processor/register.h"

// Example layouts:
// [ opcode (8 bits) ]
// [ opcode (8 bits) | imm[0:7] (8 bits)                       ]
// [ opcode (8 bits) | reg A (4 bits)     | imm[0:3] (4 bits)  ]
// [ opcode (8 bits) | reg A (4 bits)     | reg B (4 bits)     ]
// [ opcode (8 bits) | imm[0:15] (16 bits)                                                               ]
// [ opcode (8 bits) | imm[0:7] (8 bits)                       | reg A (4 bits)     | imm[8:11] (4 bits) ]
// [ opcode (8 bits) | imm[0:7] (8 bits)                       | reg A (4 bits)     | reg B (4 bits)     ]
// [ opcode (8 bits) | imm[0:15] (16 bits)                                                               | reg A (4 bits)     | reg B (4 bits)     ]

// The value of the immediate parameter encoded in an instruction.
union ImmediateValue {
  unsigned short u16 : 16;
  unsigned short u12 : 12;
  unsigned short u8 : 8;
  unsigned short u4 : 4;

  signed short s16 : 16;
  signed short s12 : 12;
  signed short s8 : 8;
  signed short s4 : 4;
};

// Represents the parameters of an instruction as fetched from memory.
struct InstructionParameters {
  enum Register registerA : 4;
  enum Register registerB : 4;
  union ImmediateValue immediate;
};

// An instruction that has been fetched from memory.
struct Instruction {
  enum Opcode opcode;
  struct InstructionParameters parameters;
};

// Fetches an instruction from memory at the given instruction pointer.
// Increments the instruction pointer to the next instruction.
struct Instruction fetchInstruction(unsigned char* memory, unsigned short* ip);

// Saves the given instruction to memory at the specified address.
// Returns the number of bytes written (between 0 and 4, inclusive).
// Returns 0 if the instruction is invalid.
int storeInstruction(unsigned char* memory, unsigned short addr, struct Instruction instruction);
