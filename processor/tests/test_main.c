#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "processor/process.h"
#include "processor/instruction.h"

bool tryParseInt(const char* str, int* result);

int main(int argc, char* argv[]) {
  // Parse arguments (file name of binary, number of steps to run)
  if (argc != 3) {
    printf("Usage: %s <binary file> <number of steps>\n", argv[0]);
    return 1;
  }

  char* binaryFileName = argv[1];
  long numberOfSteps;
  if (!tryParseInt(argv[2], &numberOfSteps)) {
    printf("Invalid number of steps: %s\n", argv[2]);
    return 1;
  }

  // Open binary file
  FILE* binaryFile = fopen(binaryFileName, "rb");
  if (binaryFile == NULL) {
    printf("Failed to open binary file: %s\n", binaryFileName);
    return 1;
  }

  // Read first MEMORY_SIZE bytes from binary file
  unsigned char memory[MEMORY_SIZE] = { 0 };
  size_t bytesRead = fread(memory, sizeof(unsigned char), MEMORY_SIZE, binaryFile);
  if (bytesRead == 0) {
    printf("Failed to read binary file: %s\n", binaryFileName);
    fclose(binaryFile);
    return 1;
  }
  fclose(binaryFile);

  // Run process for numberOfSteps steps
  struct ProcessState processState = { 0 };

  for (int i = 0; i < numberOfSteps; i++) {
    stepProcess(memory, &processState);
    printProcessState(&processState);
    Sleep(1000);
  }
}

bool tryParseInt(const char* str, int* result) {
  char* end;
  errno = 0;
  long value = strtol(str, &end, 10);
  if (errno == ERANGE || *end != '\0' || end == str) {
    return false;
  }

  if (value < INT_MIN || value > INT_MAX) {
    return false;
  }

  *result = (int)value;
  return true;
}

/*
Example program:
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
*/
