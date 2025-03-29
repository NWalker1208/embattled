#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "utilities/text.h"
#include "assembler/assembly.h"

// Assembly Syntax:
//   <program> -> <line>*
//   <line> ->
//     ((<name> | (<name>? '@' <address>)) ':') |
//     (<opcode> (<parameter> (',' <parameter>)*)?) |
//     ('.data' <hexByte>+)
//   <address> -> <hexDigit>{1,4}
//   <name> -> [a-zA-Z_][a-zA-Z0-9_]*
//   <parameter> -> <register> | <immediateValue> | <labelReference>
//   <register> -> '$' <registerName>
//   <immediateValue> -> '0x' <hexDigit>+ | ('-' | '+')? <digit>+
//   <labelReference> -> '@' <name>
//   <hexByte> -> <hexDigit><hexDigit>
//   <hexDigit> -> [0-9a-fA-F]
//   <digit> -> [0-9]
// Spaces and tabs between any two tokens are ignored.
// (Except in the case of a label, which does not allow whitespace between either the name or address and the '@' symbol).
// Any whitespace between two lines is ignored.

extern const char* INVALID_LABEL;
extern const char* INVALID_OPCODE;
extern const char* INVALID_PARAMETER;
extern const char* INVALID_REGISTER;
extern const char* INVALID_HEX_VALUE;
extern const char* INVALID_INT_VALUE;
extern const char* INVALID_HEX_BYTE;
extern const char* UNEXPECTED_CHARACTER;
extern const char* UNEXPECTED_END_OF_FILE;

struct ParsingError {
  // The message describing the error. Should be a string with a static lifetime.
  const char* message;
  // The pointer to the location in the text at which the error occurred. Has the same lifetime as the text string.
  const char* location;
};

// Parses the next line of assembly code from the given text and advances the text pointer.
// Compatible with both Unix and Windows line endings.
// Always advances text past the end of the current line.
// If parsing succeeds, outputs the parsed line through line and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseAssemblyLine(const char** text, struct AssemblyLine* line, struct ParsingError* error);
