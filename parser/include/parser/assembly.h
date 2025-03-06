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

// Frees all dynamically allocated memory referenced by the assembly line.
// Replaces freed pointers with NULL.
void destroyAssemblyLine(struct AssemblyLine* line);

// Frees all dynamically allocated memory referenced by the assembly instruction.
// Replaces freed pointers with NULL.
void destroyAssemblyInstruction(struct AssemblyInstruction* instruction);

// Frees all dynamically allocated memory referenced by the assembly parameter.
// Replaces freed pointers with NULL.
void destroyAssemblyParameter(struct AssemblyParameter* parameter);

// Frees all dynamically allocated memory referenced by the assembly data.
// Replaces freed pointers with NULL.
void destroyAssemblyData(struct AssemblyData* data);
