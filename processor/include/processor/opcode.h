#pragma once

enum Opcode {
  // Control flow
  NOP,  // layout 0 | no params   | no effect
  JMP,  // layout 4 | imm[16]     | ac = ip; ip = imm
  JMZ,  // layout 4 | imm[16]     | if ac == 0, then ip = imm, else no effect
  SLP,  // layout 2 | reg, imm[4] | sleep for reg + (unsigned) imm cycles
  // Memory
  MOV,  // layout 1 | reg, reg    | regA = regB
  LDIB, // layout 3 | imm[8]      | ac = 0:imm
  LDIW, // layout 4 | imm[16]     | ac = imm
  LDMB, // layout 2 | reg, imm[4] | ac = 0:mem[addr]             where addr = regA + imm (signed)
  LDMW, // layout 2 | reg, imm[4] | ac = mem[addr + 1]:mem[addr] where addr = regA + imm (signed)
  STMB, // layout 2 | reg, imm[4] | mem[addr] = ac (low)         where addr = regA + imm (signed)
  STMW, // layout 2 | reg, imm[4] | mem[addr + 1]:mem[addr] = ac where addr = regA + imm (signed)
  PSHB, // layout 2 | reg         | mem[sp - 1] = regA (low); sp -= 1
  PSHW, // layout 2 | reg         | mem[sp - 1]:mem[sp - 2] = regA; sp -= 2
  POPB, // layout 2 | reg         | sp += 1; regA = 0:mem[sp - 1]
  POPW, // layout 2 | reg         | sp += 2; regA = mem[sp - 1]:mem[sp - 2]; 
  // Math and logic
  INC,  // layout 2 | reg, imm[4] | regA += imm (unsigned)
  DEC,  // layout 2 | reg, imm[4] | regA -= imm (unsigned)
  ADD,  // layout 1 | reg, reg    | ac = regA + regB
  SUB,  // layout 1 | reg, reg    | ac = regA - regB
  MUL,  // layout 1 | reg, reg    | ac = regA * regB
  DIVS, // layout 1 | reg, reg    | ac = regA (signed)   / regB (signed)
  DIVU, // layout 1 | reg, reg    | ac = regA (unsigned) / regB (unsigned)
  REMS, // layout 1 | reg, reg    | ac = regA (signed)   % regB (signed)
  REMU, // layout 1 | reg, reg    | ac = regA (unsigned) % regB (unsigned)
  LSH,  // layout 1 | reg, reg    | ac = regA << regB (signed)
  RSHS, // layout 1 | reg, reg    | ac = regA (signed)   >> regB (signed)
  RSHU, // layout 1 | reg, reg    | ac = regA (unsigned) >> regB (signed)
  LSI,  // layout 2 | reg, imm[4] | ac = regA << imm (unsigned)
  RSIS, // layout 2 | reg, imm[4] | ac = regA (signed)   >> imm (unsigned)
  RSIU, // layout 2 | reg, imm[4] | ac = regA (unsigned) >> imm (unsigned)
  AND,  // layout 1 | reg, reg    | ac = regA & regB
  IOR,  // layout 1 | reg, reg    | ac = regA | regB
  XOR,  // layout 1 | reg, reg    | ac = regA ^ regB
  CEQ,  // layout 1 | reg, reg    | ac = (regA == regB) ? 1 : 0
  CNE,  // layout 1 | reg, reg    | ac = (regA != regB) ? 1 : 0
  CLTS, // layout 1 | reg, reg    | ac = (regA (signed) < regB (signed)) ? 1 : 0
  CLTU, // layout 1 | reg, reg    | ac = (regA (unsigned) < regB (unsigned)) ? 1 : 0
  CGES, // layout 1 | reg, reg    | ac = (regA (signed) >= regB (signed)) ? 1 : 0
  CGEU, // layout 1 | reg, reg    | ac = (regA (unsigned) >= regB (unsigned)) ? 1 : 0
};

extern const unsigned int OPCODE_COUNT;
extern const unsigned char OPCODE_VALUES[];
extern const char* OPCODE_NAMES[];

enum Opcode decodeOpcode(unsigned char opcodeValue);
