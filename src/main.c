#include <stdio.h>

#define MEM_SIZE 65536

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
// A: | function (2 bits) | register A (3 bits) | register B (3 bits) | register C (3 bits) | opcode (5 bits) |
// B: | immediate (5 bits)                      | register A (3 bits) | register B (3 bits) | opcode (5 bits) |
// C: | immediate (8 bits)                                            | register (3 bits)   | opcode (5 bits) |
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
  
}
