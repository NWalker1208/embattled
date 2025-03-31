#include <stdio.h>
#include <stdlib.h>
#include "utilities/file.h"
#include "parser/parse.h"
#include "assembler/assembly.h"
#include "assembler/assemble.h"
#include "processor/process.h"
#include "processor/instruction.h"


int main(int argc, char* argv[]) {
  // Get command line arguments: path of assembly file
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <assembly file>\n", argv[0]);
    return 1;
  }
  char* assemblyFilePath = argv[1];

  // Load the contents of the assembly file
  size_t fileLength;
  char* chars = ReadAllText(assemblyFilePath, &fileLength);
  if (chars == NULL) {
    fprintf(stderr, "Failed to read assembly file\n");
    return 1;
  }

  // Parse the assembly file
  printf("Parsing file\n");
  TextContents text = InitTextContents(&chars, fileLength);
  AssemblyProgram program;
  ParsingErrorList parsingErrors = { 0 };
  if (!TryParseAssemblyProgram(&text, &program, &parsingErrors)) {
    if (parsingErrors.moreErrors) {
      fprintf(stderr, "Failed to parse assembly file due to %zu+ errors.\n", parsingErrors.errorCount);
    } else {
      fprintf(stderr, "Failed to parse assembly file due to %zu errors.\n", parsingErrors.errorCount);
    }

    for (size_t i = 0; i < parsingErrors.errorCount; i++) {
      fprintf(stderr, "Line %zu, column %zu: %s\n",
        parsingErrors.errors[i].sourceSpan.start.line + 1,
        parsingErrors.errors[i].sourceSpan.start.column + 1,
        parsingErrors.errors[i].message);
    }
    return 1;
  }
  printf("Parsed %zu lines of assembly code\n", program.lineCount);

  // Assemble the program from the parsed lines
  printf("Assembling program\n");
  struct ProcessState processState = { 0 };
  AssemblingError assemblingError;
  if (!TryAssembleProgram(&text, &program, processState.memory, &assemblingError)) {
    fprintf(stderr, "Failed to assemble program due to error on line %zu, column %zu: %s\n",
      assemblingError.sourceSpan.start.line + 1,
      assemblingError.sourceSpan.start.column + 1,
      assemblingError.message);
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
