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

// Parses the next line of assembly code from the given text and advances the text pointer.
// Compatible with both Unix and Windows line endings.
// Always advances text past the end of the current line.
// If parsing succeeds, outputs the parsed line through result and returns true.
// If parsing fails, prints one or more errors to the provided err file and returns false.
bool tryParseAssemblyLine(FILE* err, const char** text, struct AssemblyLine* result);
