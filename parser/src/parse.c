#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser/parse.h"
#include "parser/utilities.h"

#pragma region Helper function signatures

// Parses an assembly instruction.
// If the current line is a valid instruction, advances text to the end of the line,
// outputs the instruction through result, and returns true.
// If parsing fails, prints one or more errors to the provided err file,
// leaves text and result as they are, and returns false.
bool tryParseInstruction(FILE* err, const char** text, struct AssemblyInstruction* result);

// Parses hexadecimal assembly data bytes.
// Expects text to be at the first hexadecimal byte (past the '.data' sequence).
// If text contains valid hexadecimal bytes from the start to the end of the line,
// advances text to the end of the line, outputs the data through result, and returns true.
// If parsing fails, prints one or more errors to the provided err file,
// leaves text and result as they are, and returns false.
bool tryParseAssemblyData(FILE* err, const char** text, struct AssemblyData* result);

// Parses an assembly instruction parameter.
// If parsing succeeds, advances text past the end of the parameter,
// outputs the parameter through result and returns true.
// If parsing fails, leaves text and result as they are and returns false.
bool tryParseParameter(const char** text, struct AssemblyParameter* result);

// Parses an opcode name.
// Expects text to be at the start of the opcode name.
// If parsing succeeds, advances text past the end of the opcode name,
// outputs the opcode through result and returns true.
// If parsing fails, leaves text and result as they are and returns false.
bool tryParseOpcode(const char** text, enum Opcode* result);

// Parses a register name.
// Expects text to be at the start of the register name (past the "$" character).
// If parsing succeeds, advances text past the end of the register name,
// outputs the register through result and returns true.
// If parsing fails, leaves text and result as they are and returns false.
bool tryParseRegister(const char** text, enum Register* result);

// Parses a hexadecimal immediate value.
// Expects text to be at the first hexadecimal digit (pase the "0x" sequence).
// If parsing succeeds, advances text past the end of the hexadecimal digits,
// outputs the immediate value through result and returns true.
// If parsing fails, leaves text and result as they are and returns false.
bool tryParseImmediateHexValue(const char** text, signed int* result);

// Parses a decimal immediate value.
// Expects text to be at the first digit, or at the sign character if one is present.
// If parsing succeeds, advances text past the end of the digits,
// outputs the immediate value through result and returns true.
// If parsing fails, leaves text and result as they are and returns false.
bool tryParseImmediateDecValue(const char** text, signed int* result);

// Parses a label name.
// If parsing succeeds, advances text past the end of the label and returns a pointer to a new
// null-terminated array containing a copy of the label.
// If parsing fails, leaves text as it is and returns NULL.
char* tryParseLabel(const char** text);

#pragma endregion

bool tryParseAssemblyLine(FILE* err, const char** text, struct AssemblyLine* result) {
  struct AssemblyLine partialResult = { 0 }; // Copied to result on success
  skipAllWhitespace(text);

  // Parse the label if one is present
  if (findCharOnLine(*text, ':') != NULL) {
    partialResult.label = tryParseLabel(text);
    if (partialResult.label == NULL) {
      destroyAssemblyLine(&partialResult);
      skipToNextLine(text);
      return false; // Failed to parse label
    }
    skipInlineWhitespace(text);
    if (**text != ':') {
      destroyAssemblyLine(&partialResult);
      skipToNextLine(text);
      return false; // Expected ':' after label
    }
    skipAllWhitespace(text);
  }

  // If line starts with ".data", parse as assembly data.
  // Otherwise, parse as an assembly instruction.
  if (startsWithCaseInsensitive(*text, ".data")) {
    partialResult.kind = DATA;
    if (!tryParseAssemblyData(err, text, &partialResult.data)) {
      destroyAssemblyLine(&partialResult);
      skipToNextLine(text);
      return false; // Failed to parse assembly data
    }
  } else {
    partialResult.kind = INSTRUCTION;
    if (!tryParseInstruction(err, text, &partialResult.instruction)) {
      destroyAssemblyLine(&partialResult);
      skipToNextLine(text);
      return false; // Failed to parse assembly instruction
    }
  }

  // Check that we are at the end of the line
  if (**text != '\r' && **text != '\n') {
    destroyAssemblyLine(&partialResult);
    skipToNextLine(text);
    return false; // Expected end of line
  }

  // Copy out result, advance to next line, and return success.
  *result = partialResult;
  skipToNextLine(text);
  return true;
}
