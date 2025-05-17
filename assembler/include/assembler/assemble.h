#pragma once
#include <stdbool.h>
#include "utilities/text.h"
#include "assembler/assembly.h"

extern const char INVALID_LINE[];
extern const char MULTIPLE_LABELS[];
extern const char DUPLICATE_LABEL_NAME[];
extern const char LABEL_ADDRESS_TOO_LOW[];
extern const char INVALID_LABEL[];
extern const char NO_MATCHING_OVERLOAD[];
extern const char EXPECTED_IMMEDIATE_VALUE_NOT_LABEL[];
extern const char IMMEDIATE_VALUE_OUT_OF_RANGE_16_BIT[];
extern const char IMMEDIATE_VALUE_OUT_OF_RANGE_16_BIT_UNSIGNED[];
extern const char IMMEDIATE_VALUE_OUT_OF_RANGE_16_BIT_SIGNED[];
extern const char IMMEDIATE_VALUE_OUT_OF_RANGE_8_BIT[];
extern const char IMMEDIATE_VALUE_OUT_OF_RANGE_8_BIT_UNSIGNED[];
extern const char IMMEDIATE_VALUE_OUT_OF_RANGE_8_BIT_SIGNED[];
extern const char IMMEDIATE_VALUE_OUT_OF_RANGE_4_BIT[];
extern const char IMMEDIATE_VALUE_OUT_OF_RANGE_4_BIT_UNSIGNED[];
extern const char IMMEDIATE_VALUE_OUT_OF_RANGE_4_BIT_SIGNED[];
extern const char INVALID_INSTRUCTION[];
extern const char EXPECTED_INSTRUCTION_OR_DATA[];
extern const char UNDEFINED_LABEL_NAME[];
extern const char OUT_OF_MEMORY[];

typedef struct AssemblingError {
  // The message describing the error. Should be a string with a static lifetime.
  const char* message;
  // The span of the source TextContents at which the error occurred.
  TextSpan sourceSpan;
} AssemblingError;

// Attempts to assemble the given assembly program into bytes in the memory space provided.
// If successful, writes the assembled bytes to memory and returns true.
// Otherwise, outputs the cause through error and returns false.
bool TryAssembleProgram(const TextContents* sourceText, const AssemblyProgram* assemblyProgram, uint8_t* memory, AssemblingError* error);
