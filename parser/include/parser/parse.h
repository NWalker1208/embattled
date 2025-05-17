#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "utilities/text.h"
#include "assembler/assembly.h"

// Assembly Syntax:
//   <program> -> (<line> | <comment>)*
//   <line> ->
//     ((<name> | (<name>? '@' <address>)) ':') |
//     (<opcode> (<parameter> (',' <parameter>)*)?) |
//     ('.data' <hexByte>+)
//   <comment> -> ';' <text>
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

extern const char INVALID_LABEL_NAME[];
extern const char INVALID_LABEL_ADDR[];
extern const char UNRECOGNIZED_MNEMONIC[];
extern const char INVALID_OPERAND[];
extern const char TOO_MANY_OPERANDS[];
extern const char INVALID_REGISTER[];
extern const char INVALID_HEX_VALUE[];
extern const char INVALID_INT_VALUE[];
extern const char INVALID_HEX_BYTE[];
extern const char UNEXPECTED_CHARACTER[];
extern const char UNEXPECTED_END_OF_FILE[];

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
// Outputs all successfully parsed lines through program.
// If no parsing errors occur, returns true.
// If any parsing errors occur, updates errors and returns false.
bool TryParseAssemblyProgram(const TextContents* text, AssemblyProgram* program, ParsingErrorList* errors);

// Parses the next line of assembly from the given text and advances the position.
// Skips leading whitespace (including newlines) and comments.
// Appends to the data buffer if necessary.
// If parsing succeeds, outputs the parsed line through line, advances to the end of the parsed text (usually the whole line), and returns true.
// If parsing fails, outputs the cause through error, advances to the end of the text line, and returns false.
bool TryParseAssemblyLine(const TextContents* text, TextOffset* position, uint8_t** dataBuffer, size_t* dataBufferSize, AssemblyLine* line, ParsingError* error);
