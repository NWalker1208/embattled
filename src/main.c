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
// Instr | Layout | Opcode | Function
// Control flow:
//   NOP | Any
//   JMP | C or D
//   JMZ | B or C
//   JMN | B or C
// Memory:
//   LDM | B
//   STM | B
//   LIH | C
//   LIL | C
// Math:
//   ADD | A
//   SUB | A
//   MUL | A
//   DIV | A
//   DVS | A
//   REM | A
//   RMS | A
// Bitwise:
//   AND | A
//   IOR | A
//   XOR | A
//   LSH | A or B
//   RSH | A or B
//   RSE | A or B
// Comparison:
//   CEQ | A or B
//   CLT | A or B
//   CGE | A or B
//   CLS | A or B
//   CGS | A or B

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
