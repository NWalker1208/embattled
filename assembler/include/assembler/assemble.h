#pragma once
#include <stdbool.h>
#include "parser/assembly.h"

struct AssemblingError {
  const char* message;
  unsigned int lineNumber;
};

// Attempts to assemble the given assembly lines into an executable in the memory space provided.
// If successful, writes the assembled executable to memory and returns true.
// Otherwise, outputs the cause through error and returns false.
bool tryAssemble(struct AssemblyLine* lines, unsigned int linesCount, unsigned char* memory, struct AssemblingError* error);
