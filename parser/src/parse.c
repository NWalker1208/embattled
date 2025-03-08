#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "parser/parse.h"
#include "parser/utilities.h"

#pragma region Error messages

const char* INVALID_LABEL = "Invalid label";
const char* INVALID_OPCODE = "Invalid opcode";
const char* INVALID_PARAMETER = "Invalid parameter";
const char* INVALID_REGISTER = "Invalid register";
const char* INVALID_HEX_VALUE = "Invalid hexadecimal value";
const char* INVALID_BYTE = "Invalid data byte";
const char* UNEXPECTED_CHARACTER = "Unexpected character";
const char* UNEXPECTED_END_OF_FILE = "Unexpected end of file";

#define PARSING_ERROR(_message, _location) (struct ParsingError){.message = (_message), .location = (_location)}

#pragma endregion

#pragma region Helper function signatures

// Parses an assembly instruction.
// If the current line is a valid instruction, advances text to the end of the line,
// outputs the instruction through result, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseInstruction(const char** text, struct AssemblyInstruction* instruction, struct ParsingError* error);

// Parses hexadecimal assembly data bytes.
// Expects text to be at the first hexadecimal byte (past the '.data' sequence).
// If text contains valid hexadecimal bytes from the start to the end of the line,
// advances text to the end of the line, outputs the data through result, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseAssemblyData(const char** text, struct AssemblyData* data, struct ParsingError* error);

// Parses an assembly instruction parameter.
// If parsing succeeds, advances text past the end of the parameter,
// outputs the parameter through result, and returns true.
// If parsing fails, returns false.
bool tryParseParameter(const char** text, struct AssemblyParameter* parameter);

// Parses an opcode name.
// Expects text to be at the start of the opcode name.
// If parsing succeeds, advances text past the end of the opcode name,
// outputs the opcode through result and returns true.
// If parsing fails, returns false.
bool tryParseOpcode(const char** text, enum Opcode* opcode);

// Parses a register name.
// Expects text to be at the start of the register name (past the "$" character).
// If parsing succeeds, advances text past the end of the register name,
// outputs the register through result and returns true.
// If parsing fails, returns false.
bool tryParseRegister(const char** text, enum Register* reg);

// Parses a hexadecimal immediate value.
// Expects text to be at the first hexadecimal digit (pase the "0x" sequence).
// If parsing succeeds, advances text past the end of the hexadecimal digits,
// outputs the immediate value through result and returns true.
// If parsing fails, returns false.
bool tryParseImmediateHexValue(const char** text, signed int* value);

// Parses a decimal immediate value.
// Expects text to be at the first digit, or at the sign character if one is present.
// If parsing succeeds, advances text past the end of the digits,
// outputs the immediate value through result and returns true.
// If parsing fails, returns false.
bool tryParseImmediateDecValue(const char** text, signed int* value);

// Parses and copies a label name.
// If parsing succeeds, advances text past the end of the label and returns a pointer to a new
// null-terminated array containing a copy of the label.
// If parsing fails, leaves text as it is and returns NULL.
char* tryCopyLabel(const char** text);

#pragma endregion

bool tryParseAssemblyLine(const char** text, struct AssemblyLine* line, struct ParsingError* error) {
  struct AssemblyLine result = { 0 }; // Copied to result on success
  skipAllWhitespace(text);

  // Parse the label if one is present
  if (findCharOnLine(*text, ':') != NULL) {
    result.label = tryCopyLabel(text);
    if (result.label == NULL) {
      *error = PARSING_ERROR(INVALID_LABEL, *text);
      destroyAssemblyLine(&result);
      skipToNextLine(text);
      return false; // Failed to parse label
    }
    skipInlineWhitespace(text);
    if (**text != ':') {
      *error = PARSING_ERROR(UNEXPECTED_CHARACTER, *text);
      destroyAssemblyLine(&result);
      skipToNextLine(text);
      return false; // Expected ':' after label
    }
    skipAllWhitespace(text);
  }

  // If line starts with ".data", parse as assembly data.
  // Otherwise, parse as an assembly instruction.
  if (startsWithCaseInsensitive(*text, ".data")) {
    result.kind = DATA;
    if (!tryParseAssemblyData(text, &result.data, error)) {
      destroyAssemblyLine(&result);
      skipToNextLine(text);
      return false; // Failed to parse assembly data
    }
  } else {
    result.kind = INSTRUCTION;
    if (!tryParseInstruction(text, &result.instruction, error)) {
      destroyAssemblyLine(&result);
      skipToNextLine(text);
      return false; // Failed to parse assembly instruction
    }
  }
  assert(**text == '\r' || **text == '\n');

  // Copy out result, advance to next line, and return success.
  *line = result;
  skipToNextLine(text);
  return true;
}

bool tryParseInstruction(const char** text, struct AssemblyInstruction* instruction, struct ParsingError* error) {
  *error = PARSING_ERROR("Not yet implemented.", *text);
  return false;
}

bool tryParseAssemblyData(const char** text, struct AssemblyData* data, struct ParsingError* error) {
  *error = (struct ParsingError) {
    .message = "Not yet implemented.",
    .location = *text,
  };
  return false;
}

char* tryCopyLabel(const char** text) {
  return NULL;
}
