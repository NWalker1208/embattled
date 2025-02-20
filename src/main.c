#include <stdio.h>
#include <windows.h>
#include "process.h"
#include "instruction.h"

int main() {
  printf("Hello, from Embattled!\n");

  unsigned char memory[MEMORY_SIZE] = { 0 };
  struct ProcessState processState = { 0 };

  memory[0] = OPCODE_VALUES[LDIW];
  memory[1] = 0x01;
  memory[2] = 0x00;
  
  memory[3] = OPCODE_VALUES[MOV];
  memory[4] = REGISTER_IDS[X0] << 4 | REGISTER_IDS[AC];
  
  memory[5] = OPCODE_VALUES[ADD];
  memory[6] = REGISTER_IDS[AC] << 4 | REGISTER_IDS[X0];

  memory[7] = OPCODE_VALUES[PSHW];
  memory[8] = REGISTER_IDS[AC] << 4;

  memory[9] = OPCODE_VALUES[JMP];
  memory[10] = 0x05;
  memory[11] = 0x00;

  while (1) {
    stepProcess(memory, &processState);
    printProcessState(&processState);
    Sleep(1000);
  }
}
