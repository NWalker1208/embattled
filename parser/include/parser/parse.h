#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "parser/assembly.h"

// Assembly Syntax:
//   <file> -> <line>*
//   <line> ->
//     (<label> ':')? (
//       (<opcode> (<parameter> (',' <parameter>)*)?) |
//       ('.data' <hexByte>+)
//     )
//   <parameter> -> <register> | <immediateValue> | <labelReference>
//   <register> -> '$' <registerName>
//   <immediateValue> -> '0x' <hexDigit>+ | ('-' | '+')? <digit>+
//   <labelReference> -> '@' <label>
//   <label> -> [a-zA-Z_][a-zA-Z0-9_]*
//   <hexByte> -> <hexDigit><hexDigit>
//   <hexDigit> -> [0-9a-fA-F]
//   <digit> -> [0-9]
// Spaces and tabs between any two tokens are ignored.
// Any whitespace between a label and the subsequent token is ignored.

extern const char* INVALID_PARAMETER;
extern const char* INVALID_OPCODE;
extern const char* INVALID_REGISTER;
extern const char* INVALID_HEX_VALUE;
extern const char* INVALID_LABEL;
extern const char* INVALID_BYTE;
extern const char* UNEXPECTED_CHARACTER;
extern const char* UNEXPECTED_END_OF_FILE;

struct ParsingError {
  const char* message;
  const char* location;
};

// Parses the next line of assembly code from the given text and advances the text pointer.
// Compatible with both Unix and Windows line endings.
// Always advances text past the end of the current line.
// If parsing succeeds, outputs the parsed line through line and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseAssemblyLine(const char** text, struct AssemblyLine* line, struct ParsingError* error);
