#pragma once
#include <stdbool.h>
#include "utilities/text.h"
#include "assembler/assembly.h"

static const char INVALID_LINE[] = "Invalid line";
static const char DUPLICATE_LABEL[] = "Label is defined more than once";
static const char MULTIPLE_LABELS[] = "Multiple labels applied to the same line";
static const char EXPECTED_INSTRUCTION_OR_DATA[] = "Expected instruction or data after label";
static const char ADDRESS_TOO_LOW[] = "Address is lower than current address being written";
static const char TOO_FEW_PARAMS[] = "Too few parameters";
static const char TOO_MANY_PARAMS[] = "Too many parameters";
static const char EXPECTED_REGISTER[] = "Expected register as parameter";
static const char EXPECTED_IMMEDIATE_VALUE[] = "Expected immediate value as parameter";
static const char EXPECTED_IMMEDIATE_VALUE_OR_LABEL_REF[] = "Expected immediate value or label reference as parameter";
static const char IMMEDIATE_VALUE_OUT_OF_RANGE[] = "Immediate value is outside the allowed range";
static const char INVALID_INSTRUCTION[] = "Invalid instruction";
static const char UNDEFINED_LABEL[] = "Undefined label";

typedef struct {
  // The message describing the error. Should be a string with a static lifetime.
  const char* message;
  // The span of the source TextContents at which the error occurred.
  TextContentsSpan sourceSpan;
} AssemblingError;

// Attempts to assemble the given assembly program into bytes in the memory space provided.
// If successful, writes the assembled bytes to memory and returns true.
// Otherwise, outputs the cause through error and returns false.
bool TryAssembleProgram(const AssemblyProgram* program, unsigned char* memory, AssemblingError* error);
