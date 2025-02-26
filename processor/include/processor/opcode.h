#pragma once

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
  
  OPCODE_COUNT,
};

// Describes how the parameters are laid out for a given opcode.
enum ParameterLayout {
  // 0 bytes
  NONE,       // No parameters
  // 1 byte
  REGA,       // Register A
  REGA_REGB,  // Register A, Register B
  REGA_IMM4,  // Register A, 4-bit Immediate
  IMM8,       // 8-bit Immediate
  // 2 bytes
  REGA_IMM12, // Register A, 12-bit Immediate
  IMM16,      // 16-bit Immediate
};

struct OpcodeInfo {
  const char* name;
  enum ParameterLayout parameterLayout;
};

extern const struct OpcodeInfo OPCODE_INFO[];

enum Opcode decodeOpcode(unsigned char opcodeValue);
