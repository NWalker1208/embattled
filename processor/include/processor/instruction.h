#pragma once
#include <stdint.h>
#include "processor/opcode.h"
#include "processor/operand.h"
#include "processor/register.h"

// An instruction consisting of an opcode and zero or more operands.
typedef struct Instruction {
  Opcode opcode;
  InstructionOperands operands;
} Instruction;

// Fetches an instruction from memory at the specified address.
// Outputs the fetched instruction through instruction.
// Returns the number of bytes read (between 1 and 4, inclusive).
unsigned short fetchInstruction(const unsigned char* memory, unsigned short addr, struct Instruction* instruction);

// Stores the given instruction to memory at the specified address.
// Returns the number of bytes written (between 0 and 4, inclusive).
// Returns 0 if the instruction is invalid.
unsigned short storeInstruction(unsigned char* memory, unsigned short addr, struct Instruction instruction);

// Prints the instruction to stdout in a human-readable format.
void printInstruction(struct Instruction instruction);
