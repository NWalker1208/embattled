#include <stdio.h>

struct Registers {
  // System registers
  unsigned short ip; // Instruction pointer
  unsigned short sp; // Stack pointer

  // General registers
  unsigned int a; // eax, ax, ah, al
  unsigned int b; // ebx, bx, bh, bl
  unsigned int c; // ecx, cx, ch, cl
  unsigned int d; // edx, dx, dh, dl
};

int main() {
  printf("Hello, from Embattled!\n");
}
