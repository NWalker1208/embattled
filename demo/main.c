#include <stdio.h>
#include <stdlib.h>
#include "parser/assembly.h"
#include "parser/parse.h"
#include "assembler/assemble.h"
#include "processor/process.h"
#include "processor/instruction.h"

#if defined(_MSC_VER)
#include <windows.h>
#define SLEEP(seconds) Sleep((seconds) * 1000)
#else
#include <unistd.h>
#define SLEEP(seconds) sleep(seconds)
#endif

// Reads an entire file as text into a dynamically allocated null-terminated string.
char* readAllText(const char* filePath);

int main(int argc, char* argv[]) {
  // Get command line arguments: path of assembly file
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <assembly file>\n", argv[0]);
    return 1;
  }
  char* assemblyFilePath = argv[1];

  // Load the contents of the assembly file
  const char* text = readAllText(assemblyFilePath);
  if (text == NULL) {
    return 1;
  }

  // Parse the assembly file line by line
  printf("Parsing file\n");
  struct AssemblyLine* lines = NULL;
  unsigned int lineCount = 0;
  bool anyErrors = false;
  while (*text != '\0') {
    lineCount++;
    lines = realloc(lines, sizeof(struct AssemblyLine) * lineCount);
    struct AssemblyLine* nextLine = &lines[lineCount - 1];
    memset(nextLine, 0, sizeof(struct AssemblyLine)); // Ensure all fields are initialized to 0

    struct ParsingError parseError;
    if (!tryParseAssemblyLine(&text, nextLine, &parseError)) {
      fprintf(stderr, "Failed to parse line %u: %s\n", lineCount, parseError.message);
      anyErrors = true;
    }
  }

  if (anyErrors) {
    return 1;
  }
  printf("Parsed %d lines of assembly code\n", lineCount);

  // Assemble the program from the parsed lines
  printf("Assembling program\n");
  struct ProcessState processState = { 0 };
  struct AssemblingError asmError;
  if (!tryAssemble(lines, lineCount, processState.memory, &asmError)) {
    fprintf(stderr, "Failed to assemble program due to error on line %u: %s\n", asmError.lineNumber, asmError.message);
    return 1;
  }

  // Execute the program
  printf("Executing program\n");
  while (true) {
    // Display info about next instruction and current state
    struct Instruction nextInstruction = { 0 };
    fetchInstruction(processState.memory, processState.registers.ip, &nextInstruction);
    printInstruction(nextInstruction);
    printRegistersState(&processState.registers);

    // Allow user to override values in memory each cycle
    char input[6];
    unsigned short addressToWrite;
    unsigned char valueToWrite;
    while (true) {
      printf("Enter address to write, or leave blank to continue: ");
      if (fgets(input, 6, stdin) != NULL && sscanf(input, "%hx", &addressToWrite) == 1) {
        printf("Enter value to write, or leave blank to cancel: ");
        if (fgets(input, 4, stdin) != NULL && sscanf(input, "%hhx", &valueToWrite) == 1) {
          processState.memory[addressToWrite] = valueToWrite;
        } else {
          break;
        }
      } else {
        break;
      }
    }

    // Run next cycle
    stepProcess(&processState);
  }

  return 0;
}

char* readAllText(const char* filePath) {
  // Open the file
  FILE* file = fopen(filePath, "rb"); // Get raw bytes of text file
  if (filePath == NULL) {
    perror("Failed to open file");
    return NULL;
  }

  // Measure the length of the file
  fseek(file, 0, SEEK_END);
  size_t fileLength = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Read the contents of the file
  char* contents = malloc(fileLength + 1);
  size_t bytesRead = fread(contents, 1, fileLength, file);
  if (bytesRead != fileLength) {
    fprintf(stderr, "Failed to read entire file. Got %llu bytes but expected %llu.\n", bytesRead, fileLength);
    return NULL;
  }
  fclose(file);
  contents[bytesRead] = '\0';

  return contents;
}
