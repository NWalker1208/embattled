#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "processor/register.h"

enum AssemblyParameterKind {
  INVALID = 0,
  REGISTER,
  IMMEDIATE,
  LABEL_REF,
};

struct AssemblyParameter {
  enum AssemblyParameterKind kind;
  union {
    enum Register registerName; // kind == REGISTER
    signed int immediateValue; // kind == IMMEDIATE
    char* referencedLabel; // kind == LABEL_REF
  };
};

struct AssemblyInstruction {
  char* opcode;
  unsigned int parameterCount;
  struct AssemblyParameter* parameters;
};

struct AssemblyData {
  unsigned int length;
  unsigned char* bytes;
};

enum AssemblyLineKind {
  INVALID = 0,
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

// Parses the next instruction from the given text and advances the text pointer.
// Compatible with both Unix and Windows line endings.
// If parsing succeeds, outputs the parsed line through result and returns true.
// If parsing fails, prints one or more errors to the provided
// err file, advances past the end of the line, and returns false.
bool tryParseNextLine(FILE* err, char** text, struct AssemblyLine* result);
