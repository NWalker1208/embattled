#include <stdio.h>
#include <stdbool.h>
#include "processor/instruction.h"

enum AssemblyLineKind {
  INVALID = 0,
  INSTRUCTION,
  DATA,
};

struct BinaryData {
  unsigned int length;
  unsigned char* bytes;
};

struct AssemblyLine {
  char* label;
  enum AssemblyLineKind kind;
  union {
    struct Instruction instruction; // kind == INSTRUCTION
    struct BinaryData data; // kind == DATA
  } contents;
};

// Parses the next instruction from the given text and advances the text pointer.
// Compatible with both Unix and Windows line endings.
// If parsing succeeds, outputs the parsed line through result and returns true.
// If parsing fails, prints one or more errors to the provided
// err file, advances past the end of the line, and returns false.
bool tryParseNextLine(FILE* err, char** text, struct AssemblyLine* result);
