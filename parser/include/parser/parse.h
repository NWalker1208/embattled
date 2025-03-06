#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "processor/opcode.h"
#include "processor/register.h"

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

enum AssemblyParameterKind {
  REGISTER,
  IMMEDIATE_VALUE,
  LABEL_REFERENCE,
};

struct AssemblyParameter {
  enum AssemblyParameterKind kind;
  union {
    enum Register registerName; // kind == REGISTER
    signed int immediateValue; // kind == IMMEDIATE_VALUE
    char* referencedLabel; // kind == LABEL_REFERENCE
  };
};

struct AssemblyInstruction {
  enum Opcode opcode;
  unsigned int parameterCount;
  struct AssemblyParameter* parameters;
};

struct AssemblyData {
  unsigned int length;
  unsigned char* bytes;
};

enum AssemblyLineKind {
  INSTRUCTION,
  DATA,
};

struct AssemblyLine {
  char* label;
  enum AssemblyLineKind kind;
  union {
    struct AssemblyInstruction instruction; // kind == INSTRUCTION
    struct AssemblyData data; // kind == DATA
  } contents;
};

// Parses the next line of assembly code from the given text and advances the text pointer.
// Compatible with both Unix and Windows line endings.
// Always advances text past the end of the current line.
// If parsing succeeds, outputs the parsed line through result and returns true.
// If parsing fails, prints one or more errors to the provided err file and returns false.
bool tryParseAssemblyLine(FILE* err, const char** text, struct AssemblyLine* result);
