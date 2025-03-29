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

typedef struct {
  // The message describing the error. Should be a string with a static lifetime.
  const char* message;
  // The span of the source TextContents at which the error occurred.
  TextContentsSpan sourceSpan;
} ParsingError;

// Parses an assembly program from the provided text.
// Moves the text contents into the program struct.
// If no parsing errors occur, sets errors to NULL and returns 0.
// If any parsing errors occur, sets errors to a dynamically allocated array and returns the number of errors.
size_t TryParseAssemblyProgram(TextContents* text, AssemblyProgram* program, ParsingError** errors);

// Parses the next line of assembly from the given text and advances the position.
// Always advances the position to the beginning of some subsequent line.
// If parsing succeeds, outputs the parsed line through line and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool TryParseAssemblyLine(const TextContents* text, TextContentsOffset* position, AssemblyLine* line, ParsingError* error);
