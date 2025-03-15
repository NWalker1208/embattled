#pragma once
#include <stdbool.h>
#include "parser/assembly.h"

static const char TOO_FEW_PARAMS[] = "Too few parameters";
static const char TOO_MANY_PARAMS[] = "Too many parameters";
static const char EXPECTED_REGISTER[] = "Expected register as parameter";
static const char EXPECTED_IMMEDIATE_VALUE[] = "Expected immediate value as parameter";
static const char EXPECTED_IMMEDIATE_VALUE_OR_LABEL_REF[] = "Expected immediate value or label reference as parameter";
static const char IMMEDIATE_VALUE_OUT_OF_RANGE[] = "Immediate value is outside the allowed range";
static const char INVALID_INSTRUCTION[] = "Invalid instruction";
static const char UNDEFINED_LABEL[] = "Undefined label";

struct AssemblingError {
  // The message describing the error. Should be a string with a static lifetime.
  const char* message;
  // The line on which the error occurred.
  unsigned int lineNumber;
};

// Attempts to assemble the given assembly lines into an executable in the memory space provided.
// If successful, writes the assembled executable to memory and returns true.
// Otherwise, outputs the cause through error and returns false.
bool tryAssemble(struct AssemblyLine* lines, unsigned int linesCount, unsigned char* memory, struct AssemblingError* error);
