#include "instruction.h"

enum AssemblyLineKind {
  INVALID = 0,
  INSTRUCTION,
  DATA,
};

struct AssemblyLine {
  char* label;
  enum AssemblyLineKind kind;
  union {
    struct Instruction instruction; // kind == INSTRUCTION
    unsigned char* data; // kind == DATA
  } contents;
};

// Parses the next instruction from the given text and advances the text pointer.
// Compatible with both Unix and Windows line endings.
// If an invalid line is encountered, advances past the next newline sequence.
struct AssemblyLine parseNextLine(char** text);
