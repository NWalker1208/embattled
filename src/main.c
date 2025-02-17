#include <stdio.h>
#include <windows.h>
#include "process.h"

int main() {
  printf("Hello, from Embattled!\n");

  unsigned char memory[MEMORY_SIZE] = { 0 };
  struct ProcessState processState = { 0 };

  while (1) {
    stepProcess(memory, &processState);
    printProcessState(&processState);
    Sleep(1000);
  }
}
