#include <stdio.h>
#include <stdlib.h>
#include "parser/assembly.h"
#include "parser/parse.h"
#include "assembler/assemble.h"
#include "processor/process.h"

#if defined(_MSC_VER)
#include <windows.h>
#define SLEEP(seconds) Sleep((seconds) * 1000)
#else
#include <unistd.h>
#define SLEEP(seconds) sleep(seconds)
#endif

int main(int argc, char* argv[]) {
  // Get command line arguments: path of assembly file
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <assembly file>\n", argv[0]);
    return 1;
  }
  char* assemblyFilePath = argv[1];

  // Load the contents of the assembly file
  FILE* assemblyFile = fopen(assemblyFilePath, "r");
  if (assemblyFile == NULL) {
    perror("Failed to open assembly file");
    return 1;
  }
  fseek(assemblyFile, 0, SEEK_END);
  size_t assemblyFileSize = ftell(assemblyFile);
  fseek(assemblyFile, 0, SEEK_SET);
  char* assemblyFileContents = malloc(assemblyFileSize + 1);
  if (fread(assemblyFileContents, assemblyFileSize, 1, assemblyFile) != 1) {
    fprintf(stderr, "Failed to read assembly file");
    return 1;
  }
  fclose(assemblyFile);
  assemblyFileContents[assemblyFileSize] = '\0';

  // Parse the assembly file line by line
  const char* text = assemblyFileContents;
  struct AssemblyLine* lines = NULL;
  unsigned int lineCount = 0;
  bool anyErrors = false;
  while (*text != '\0') {
    lineCount++;
    lines = realloc(lines, sizeof(struct AssemblyLine) * lineCount);

    struct ParsingError parseError;
    if (!tryParseAssemblyLine(&text, &lines[lineCount - 1], &parseError)) {
      fprintf(stderr, "Failed to parse line %u: %s\n", lineCount, parseError.message);
      anyErrors = true;
    }
  }

  if (anyErrors) {
    return 1;
  }

  // Assemble the program from the parsed lines
  struct ProcessState processState = { 0 };
  struct AssemblingError asmError;
  if (!tryAssemble(lines, lineCount, processState.memory, &asmError)) {
    fprintf(stderr, "Failed to assemble program due to error on line %u: %s\n", asmError.lineNumber, asmError.message);
    return 1;
  }

  // Execute the program
  while (true) {
    SLEEP(1);
    stepProcess(&processState);
  }

  return 0;
}
