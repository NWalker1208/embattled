#pragma once
#include <stdint.h>

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
  uint16_t ip;
  uint16_t sp;
  uint16_t rt;
  // General registers
  uint16_t x0;
  uint16_t x1;
  uint16_t x2;
  uint16_t x3;
  uint16_t x4;
  uint16_t x5;
  uint16_t x6;
  uint16_t x7;
  uint16_t x8;
  uint16_t x9;
  uint16_t x10;
  uint16_t x11;
} RegistersState;

// The name of each register.
extern const char* REGISTER_NAMES[];

// Converts the lower 4-bits of a byte to a register.
// Returns NL if any of the upper 4-bits are set.
// Note: A valid register can safely be cast to an unsigned char to obtain its nibble value.
Register nibbleToRegister(uint8_t nibble);

// Gets the pointer to a register's value within a ReigstersState struct.
// Returns NULL if the register is NL or if the register is not recognized.
uint16_t* getRegisterPtr(RegistersState* state, Register reg);

// Prints the state of the registers to stdout in a human-readable format.
void printRegistersState(const RegistersState* state);
