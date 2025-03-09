#pragma once
#include <stdbool.h>

// A code indicating which action should be performed in a given instruction.
enum Opcode {
  // Control flow
  NOP,  // no params   | no effect
  JMP,  // imm[16]     | ac = ip; ip = imm
  JMZ,  // imm[16]     | if ac == 0, then ip = imm, else no effect
  SLP,  // reg, imm[4] | sleep for reg + (unsigned) imm cycles
  // Memory
  MOV,  // reg, reg    | regA = regB
  LDIB, // imm[8]      | ac = 0:imm
  LDIW, // imm[16]     | ac = imm
  LDMB, // reg, imm[4] | ac = 0:mem[addr]             where addr = regA + imm (signed)
  LDMW, // reg, imm[4] | ac = mem[addr + 1]:mem[addr] where addr = regA + imm (signed)
  STMB, // reg, imm[4] | mem[addr] = ac (low)         where addr = regA + imm (signed)
  STMW, // reg, imm[4] | mem[addr + 1]:mem[addr] = ac where addr = regA + imm (signed)
  PSHB, // reg         | mem[sp - 1] = regA (low); sp -= 1
  PSHW, // reg         | mem[sp - 1]:mem[sp - 2] = regA; sp -= 2
  POPB, // reg         | sp += 1; regA = 0:mem[sp - 1]
  POPW, // reg         | sp += 2; regA = mem[sp - 1]:mem[sp - 2]; 
  // Math and logic
  INC,  // reg, imm[4] | regA += imm (unsigned)
  DEC,  // reg, imm[4] | regA -= imm (unsigned)
  ADD,  // reg, reg    | ac = regA + regB
  SUB,  // reg, reg    | ac = regA - regB
  MUL,  // reg, reg    | ac = regA * regB
  DIVS, // reg, reg    | ac = regA (signed)   / regB (signed)
  DIVU, // reg, reg    | ac = regA (unsigned) / regB (unsigned)
  REMS, // reg, reg    | ac = regA (signed)   % regB (signed)
  REMU, // reg, reg    | ac = regA (unsigned) % regB (unsigned)
  LSH,  // reg, reg    | ac = regA << regB (unsigned)
  RSHS, // reg, reg    | ac = regA (signed)   >> regB (unsigned)
  RSHU, // reg, reg    | ac = regA (unsigned) >> regB (unsigned)
  LSI,  // reg, imm[4] | ac = regA << imm (unsigned)
  RSIS, // reg, imm[4] | ac = regA (signed)   >> imm (unsigned)
  RSIU, // reg, imm[4] | ac = regA (unsigned) >> imm (unsigned)
  AND,  // reg, reg    | ac = regA & regB
  IOR,  // reg, reg    | ac = regA | regB
  XOR,  // reg, reg    | ac = regA ^ regB
  CEQ,  // reg, reg    | ac = (regA == regB) ? 1 : 0
  CNE,  // reg, reg    | ac = (regA != regB) ? 1 : 0
  CLTS, // reg, reg    | ac = (regA (signed) < regB (signed)) ? 1 : 0
  CLTU, // reg, reg    | ac = (regA (unsigned) < regB (unsigned)) ? 1 : 0
  CGES, // reg, reg    | ac = (regA (signed) >= regB (signed)) ? 1 : 0
  CGEU, // reg, reg    | ac = (regA (unsigned) >= regB (unsigned)) ? 1 : 0
  
  OPCODE_COUNT, // The number of opcode values. Not a valid opcode itself.
};

// Describes how the parameters are laid out for a given opcode.
struct ParameterLayout {
  unsigned char numBytes : 2; // The number of bytes used to store all parameters.
  bool hasRegA : 1; // Whether register A is used.
  bool hasRegB : 1; // Whether register B is used.
  unsigned char numImmBits : 5; // The number of bits used to store the immediate value.
  bool immIsSigned : 1; // Whether the immediate value is treated as signed.
};

#define PARAM_LAYOUT(_numBytes, _hasRegA, _hasRegB, _numImmBits, _immIsSigned) \
  (struct ParameterLayout){ \
    .numBytes=(_numBytes), \
    .hasRegA=(_hasRegA), \
    .hasRegB=(_hasRegB), \
    .numImmBits=(_numImmBits), \
    .immIsSigned=(_immIsSigned) \
  }
// 0 bytes
#define PARAM_LAYOUT_NONE             PARAM_LAYOUT(0, false, false, 0, false)
// 1 byte
#define PARAM_LAYOUT_IMMU8            PARAM_LAYOUT(1, false, false, 8, false)
#define PARAM_LAYOUT_IMMS8            PARAM_LAYOUT(1, false, false, 8, true)
#define PARAM_LAYOUT_REGA_IMMU4       PARAM_LAYOUT(1, true, false, 4, false)
#define PARAM_LAYOUT_REGA_IMMS4       PARAM_LAYOUT(1, true, false, 4, true)
#define PARAM_LAYOUT_REGA_REGB        PARAM_LAYOUT(1, true, true, 0, false)
// 2 bytes
#define PARAM_LAYOUT_IMMU16           PARAM_LAYOUT(2, false, false, 16, false)
#define PARAM_LAYOUT_IMMS16           PARAM_LAYOUT(2, false, false, 16, true)
#define PARAM_LAYOUT_REGA_IMMU12      PARAM_LAYOUT(2, true, false, 12, false)
#define PARAM_LAYOUT_REGA_IMMS12      PARAM_LAYOUT(2, true, false, 12, true)
#define PARAM_LAYOUT_REGA_REGB_IMMU8  PARAM_LAYOUT(2, true, true, 8, false)
#define PARAM_LAYOUT_REGA_REGB_IMMS8  PARAM_LAYOUT(2, true, true, 8, true)
// 3 bytes
#define PARAM_LAYOUT_REGA_REGB_IMMU16 PARAM_LAYOUT(3, true, true, 16, false)
#define PARAM_LAYOUT_REGA_REGB_IMMS16 PARAM_LAYOUT(3, true, true, 16, true)

// Describes the details of a particular opcode.
struct OpcodeInfo {
  const char* name;
  struct ParameterLayout parameterLayout;
  // TODO: Consider adding function pointer for implementation.
};

// The details of each opcode. Can be indexed directly by a valid Opcode value.
extern const struct OpcodeInfo OPCODE_INFO[];

// Converts a byte to an opcode.
// Returns NOP if the byte is not a valid opcode.
// Note: A valid opcode can safely be cast to an unsigned char to obtain its byte value.
enum Opcode byteToOpcode(unsigned char byte);
