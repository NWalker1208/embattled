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

#define MAX_ERRORS 99

extern const char* INVALID_LABEL_NAME;
extern const char* INVALID_LABEL_ADDR;
extern const char* INVALID_OPCODE;
extern const char* INVALID_PARAMETER;
extern const char* INVALID_REGISTER;
extern const char* INVALID_HEX_VALUE;
extern const char* INVALID_INT_VALUE;
extern const char* INVALID_HEX_BYTE;
extern const char* UNEXPECTED_CHARACTER;
extern const char* UNEXPECTED_END_OF_FILE;

typedef struct {
  // The message describing the error. Should be a string with a static lifetime.
  const char* message;
  // The span of the source TextContents at which the error occurred.
  TextSpan sourceSpan;
} ParsingError;

typedef struct {
  // The number of errors in the list.
  size_t errorCount;
  // The array of errors.
  ParsingError errors[MAX_ERRORS];
  // Whether there were more than MAX_ERRORS errors.
  bool moreErrors;
} ParsingErrorList;

// Parses an assembly program from the provided text.
// Moves the text contents into the program struct.
// If no parsing errors occur, returns true.
// If any parsing errors occur, updates errors and returns false.
bool TryParseAssemblyProgram(TextContents* text, AssemblyProgram* program, ParsingErrorList* errors);

// Parses the next line of assembly from the given text and advances the position.
// Skips leading whitespace and newlines.
// Always advances the position to the end of the line, even when parsing fails.
// If parsing succeeds, outputs the parsed line through line and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool TryParseAssemblyLine(const TextContents* text, TextOffset* position, AssemblyLine* line, ParsingError* error);
