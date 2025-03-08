#pragma once

// TODO: Add prefix to enum value names

// A value representing a particular register to be used in an instruction.
enum Register {
  // Special registers
  NL, // Null register. Always 0.
  IP, // Instruction pointer. Address of the next instruction to execute.
  SP, // Stack pointer. Address of the top of the stack.
  AC, // Accumulator. Holds the result of most instructions.
  // General registers
  X0,
  X1,
  X2,
  X3,
  X4,
  X5,
  X6,
  X7,
  X8,
  X9,
  X10,
  X11,

  REGISTER_COUNT, // The number of registers. Not a valid register itself.
};

// Holds the state of all registers.
struct RegistersState {
  // Special registers
  unsigned short ip;
  unsigned short sp;
  unsigned short ac;
  // General registers
  unsigned short x0;
  unsigned short x1;
  unsigned short x2;
  unsigned short x3;
  unsigned short x4;
  unsigned short x5;
  unsigned short x6;
  unsigned short x7;
  unsigned short x8;
  unsigned short x9;
  unsigned short x10;
  unsigned short x11;
};

// The name of each register.
extern const char* REGISTER_NAMES[];

// Converts the lower 4-bits of a byte to a register.
// Returns NL if any of the upper 4-bits are set.
// Note: A valid register can safely be cast to an unsigned char to obtain its nibble value.
enum Register nibbleToRegister(unsigned char nibble);

// Gets the pointer to a register's value within a ReigstersState struct.
// Returns NULL if the register is NL or if the register is not recognized.
unsigned short* getRegisterPtr(struct RegistersState* state, enum Register reg);

// Prints the state of the registers to stdout.
void printRegistersState(struct RegistersState* state);
