#include "processor/opcode.h"
#include "processor/register.h"

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
  };
};
