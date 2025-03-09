#pragma once
#include "processor/opcode.h"
#include "processor/register.h"

// TODO: Add support for specifying starting address of sections
// TODO: Add support for storing references in data sections
// TODO: Add support for relative references?

enum AssemblyParameterKind {
  REGISTER,
  IMMEDIATE_VALUE,
  LABEL_REFERENCE,
};

struct AssemblyParameter {
  // The kind of parameter that this is.
  enum AssemblyParameterKind kind;
  union {
    enum Register registerName; // kind == REGISTER
    signed int immediateValue; // kind == IMMEDIATE_VALUE
    char* referencedLabel; // kind == LABEL_REFERENCE. Should be a dynamically allocated string.
  };
};

struct AssemblyInstruction {
  // The opcode for this instruction.
  enum Opcode opcode;
  // The number of parameters in the parameters array.
  unsigned int parameterCount;
  // The parameters given to this instruction. Should be a dynamically allocated array.
  struct AssemblyParameter* parameters;
};

struct AssemblyData {
  // The number of bytes in the bytes array.
  unsigned int length;
  // The bytes of this data. Should be a dynamically allocated array.
  unsigned char* bytes;
};

enum AssemblyLineKind {
  INSTRUCTION,
  DATA,
};

struct AssemblyLine {
  // The label annotating this line. Should be a dynamically allocated string.
  char* label;
  // The kind of line that this is.
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
