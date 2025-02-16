#include <stdio.h>

#define MEM_SIZE 65536

#define IMM_SML 0b11111
#define IMM_MED 0b11111111
#define IMM_LRG 0b11111111111

struct Registers {
  unsigned short ip; // Instruction pointer. Can't be referenced directly.

  unsigned short x0;
  unsigned short x1;
  unsigned short x2;
  unsigned short x3;
  unsigned short x4;
  unsigned short x5;
  unsigned short x6;
  unsigned short x7;
};

// Instructions (based on RV32C)

// Layout:
// A: | register A (3 bits) | register B (3 bits) | register C (3 bits) | function (2 bits) | opcode (5 bits) |
// B: | register A (3 bits) | register B (3 bits) | immediate (5 bits)                      | opcode (5 bits) |
// C: | register A (3 bits) | immediate (8 bits)                                            | opcode (5 bits) |
// D: | immediate (11 bits)                                                                 | opcode (5 bits) |

// Instructions:
// Instruction | Layout | Opcode | Function
// Control flow:
//   NOP       | Any    | 000 00 |
//   JMP.reg   | C      | 000 01 |
//      .imm   | D      | 000 10 |
//   JMZ.reg   | B      | 001 00 |
//      .imm   | C      | 001 01 |
//   JMN.reg   | B      | 001 10 |
//      .imm   | C      | 001 11 |
// Memory:
//   LDM       | B      | 010 00 |
//   STM       | B      | 010 01 |
//   LIH       | C      | 010 10 |
//   LIL       | C      | 010 11 |
// Math:
//   ADD       | A      | 011 00 | 00
//   SUB       | A      | 011 00 | 01
//   MUL       | A      | 011 01 | 00
//   DIV       | A      | 011 10 | 00  
//   DVS       | A      | 011 10 | 01
//   REM       | A      | 011 10 | 10
//   RMS       | A      | 011 10 | 11
// Bitwise:
//   AND       | A      | 011 11 | 00
//   IOR       | A      | 011 11 | 01
//   XOR       | A      | 011 11 | 10
//   LSH.reg   | A      | 100 00 | 00
//      .imm   | B      | 100 01 |
//   RSH.reg   | A      | 100 00 | 01
//      .imm   | B      | 100 10 |
//   RSE.reg   | A      | 100 00 | 10
//      .imm   | B      | 100 11 |
// Comparison:
//   CEQ.reg   | A      | 101 00 | 00
//      .imm   | B      | 101 01 |
//   CNE.reg   | A      | 101 00 | 01
//      .imm   | B      | 101 10 |
//   CLT.reg   | A      | 101 11 | 00
//      .imm   | B      | 110 00 |
//   CGE.reg   | A      | 101 11 | 01
//      .imm   | B      | 110 01 |
//   CLS.reg   | A      | 101 11 | 10
//      .imm   | B      | 110 10 |
//   CGS.reg   | A      | 101 11 | 11
//      .imm   | B      | 110 11 |

enum OpCode {
  // Control flow
  NOP     = 0b00000,
  JMP_reg = 0b00001,
  JMP_imm = 0b00010,
  JMZ_reg = 0b00100,
  JMZ_imm = 0b00101,
  JMN_reg = 0b00110,
  JMN_imm = 0b00111,
  // Memory
  LDM     = 0b01000,
  STM     = 0b01001,
  LIH     = 0b01010,
  LIL     = 0b01011,
  // Math
  ADD_SUB = 0b01100,
  MUL     = 0b01101,
  DIV_REM = 0b01110,
  // Bitwise
  AND_OR  = 0b01111,
  SH_reg  = 0b10000,
  LSH_imm = 0b10001,
  RSH_imm = 0b10010,
  RSE_imm = 0b10011,
  // Comparison
  CM0_reg = 0b10100,
  CEQ_imm = 0b10101,
  CNE_imm = 0b10110,
  CM1_reg = 0b10111,
  CLT_imm = 0b11000,
  CGE_imm = 0b11001,
  CLS_imm = 0b11010,
  CGS_imm = 0b11011,
};

void runCpuCycle(char* mem, struct Registers* registers);

int main() {
  printf("Hello, from Embattled!\n");

  char mem[MEM_SIZE] = { 0 };
  struct Registers registers = { 0 };

  while (1) {
    runCpuCycle(mem, &registers);
  }
}

void runCpuCycle(char* mem, struct Registers* registers) {
  unsigned short ip = registers->ip;
  char* next = &(mem[ip]);
  ip += 2;

  char opCode = next[0] & 0b11111;
  unsigned short imm = (next[0] >> 5) | (next[1] << 3);
  char regA = (imm >> 2) & 0b111;
  char regB = (imm >> 5) & 0b111;
  char regC = (imm >> 8) & 0b111;

  switch (opCode) {
    case JMP_reg:
      break;
    case JMP_imm:
      break;
    case JMZ_reg:
      break;
    case JMZ_imm:
      break;
    case JMN_reg:
      break;
    case JMN_imm:
      break;
  // Memory
    case LDM:
      break;
    case STM:
      break;
    case LIH:
      break;
    case LIL:
      break;
  // Math
    case ADD_SUB:
      break;
    case MUL:
      break;
    case DIV_REM:
      break;
  // Bitwise
    case AND_OR:
      break;
    case SH_reg:
      break;
    case LSH_imm:
      break;
    case RSH_imm:
      break;
    case RSE_imm:
      break;
  // Comparison
    case CM0_reg:
      break;
    case CEQ_imm:
      break;
    case CNE_imm:
      break;
    case CM1_reg:
      break;
    case CLT_imm:
      break;
    case CGE_imm:
      break;
    case CLS_imm:
      break;
    case CGS_imm:
      break;
    default:
      break;
  }

  registers->ip = ip;
}
