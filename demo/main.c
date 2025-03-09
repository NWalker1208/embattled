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
    fprintf(stderr, "Failed to read entire file. Got %d bytes but expected %d.\n", bytesRead, fileLength);
    return NULL;
  }
  fclose(file);
  contents[bytesRead] = '\0';

  return contents;
}
