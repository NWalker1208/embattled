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
// If there is a valid instruction at the address, outputs the fetched instruction and
// returns the number of bytes read (between 1 and 4, inclusive).
// Otherwise, outputs a NOP instruction and returns 1.
uint16_t fetchInstruction(const uint8_t* memory, uint16_t addr, Instruction* instructionOut);

// Writes the given instruction to memory at the specified address.
// If the instruction is valid, returns the number of bytes written (between 1 and 4, inclusive).
// Otherwise, returns 0.
uint16_t writeInstruction(uint8_t* memory, uint16_t addr, Instruction instruction);

// Prints the instruction to stdout in a human-readable format.
void printInstruction(Instruction instruction);
