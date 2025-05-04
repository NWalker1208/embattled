#pragma once

// TODO: Add prefix to enum value names

// A value representing a particular register to be used in an instruction.
typedef enum Register {
  // Special registers
  REGISTER_NL, // Null register. Always 0.
  REGISTER_IP, // Instruction pointer. Address of the next instruction to execute.
  REGISTER_SP, // Stack pointer. Address of the top of the stack.
  REGISTER_RT, // Return pointer. Address of the next instruction prior to performing a jump.
  // General registers
  REGISTER_X0,
  REGISTER_X1,
  REGISTER_X2,
  REGISTER_X3,
  REGISTER_X4,
  REGISTER_X5,
  REGISTER_X6,
  REGISTER_X7,
  REGISTER_X8,
  REGISTER_X9,
  REGISTER_X10,
  REGISTER_X11,

  REGISTER_COUNT, // The number of registers.
} Register;

// Holds the state of all registers.
typedef struct RegistersState {
  // Special registers
  unsigned short ip;
  unsigned short sp;
  unsigned short rt;
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
} RegistersState;

// The name of each register.
extern const char* REGISTER_NAMES[];

// Converts the lower 4-bits of a byte to a register.
// Returns NL if any of the upper 4-bits are set.
// Note: A valid register can safely be cast to an unsigned char to obtain its nibble value.
Register nibbleToRegister(unsigned char nibble);

// Gets the pointer to a register's value within a ReigstersState struct.
// Returns NULL if the register is NL or if the register is not recognized.
unsigned short* getRegisterPtr(RegistersState* state, Register reg);

// Prints the state of the registers to stdout in a human-readable format.
void printRegistersState(const RegistersState* state);
