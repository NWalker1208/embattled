#include <stdio.h>

#define MEM_SIZE 65536

struct Registers {
  unsigned short ip; // Instruction pointer. Can't be referenced directly.
  unsigned short sp; // Stack pointer. Can't be referenced directly.

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
// A: | function                      | register A (3 bits) | register B (3 bits) | opcode |
// B: | function | immediate (5 bits) | register A (3 bits) | register B (3 bits) | opcode |
// C: | function | immediate (8 bits)                       | register (3 bits)   | opcode |

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
