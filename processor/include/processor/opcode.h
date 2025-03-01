#pragma once

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
  LSH,  // reg, reg    | ac = regA << regB (signed)
  RSHS, // reg, reg    | ac = regA (signed)   >> regB (signed)
  RSHU, // reg, reg    | ac = regA (unsigned) >> regB (signed)
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
enum ParameterLayout {
  // Flags
  SIZE_MASK       = 0b0011, // The mask for the number of parameter bytes.
  REGA_FLAG       = 0b0100, // The flag for the presence of register A.
  REGB_FLAG       = 0b1000, // The flag for the presence of register B.
  // 0 bytes
  NONE            = 0b0000, // No parameters
  // 1 byte
  IMM8            = 0b0001, // 8-bit immediate
  REGA_IMM4       = 0b0101, // Register A, 4-bit immediate
  REGA_REGB       = 0b1101, // Register A, register B
  // 2 bytes
  IMM16           = 0b0010, // 16-bit immediate
  REGA_IMM12      = 0b0110, // Register A, 12-bit immediate
  REGA_REGB_IMM8  = 0b1110, // Register A, register B, 8-bit immediate
  // 3 bytes
  REGA_REGB_IMM16 = 0b1111, // Register A, register B, 16-bit immediate
};

// Describes the details of a particular opcode.
struct OpcodeInfo {
  const char* name;
  enum ParameterLayout parameterLayout;
  // TODO: Consider adding function pointer for implementation.
};

// The details of each opcode. Can be indexed directly by a valid Opcode value.
extern const struct OpcodeInfo OPCODE_INFO[];

// Converts a byte to an opcode.
// Returns NOP if the byte is not a valid opcode.
// Note: A valid opcode can safely be cast to an unsigned char to obtain its byte value.
enum Opcode byteToOpcode(unsigned char byte);
