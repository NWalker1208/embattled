#pragma once
#include <stdint.h>
#include "utilities/text.h"
#include "processor/opcode.h"
#include "processor/register.h"

// TODO: Add support for storing references in data sections
// TODO: Add support for relative references?

// The kind of an assembly parameter.
typedef enum {
  ASSEMBLY_PARAM_REGISTER,
  ASSEMBLY_PARAM_IMMEDIATE,
  ASSEMBLY_PARAM_LABEL,
} AssemblyParameterKind;

// The parameter of an assembly instruction.
typedef struct {
  // The location of the parameter in the source TextContents.
  TextContentsSpan sourceSpan;
  // The kind of parameter that this is.
  AssemblyParameterKind kind;
  union {
    enum Register registerName;       // kind == ASSEMBLY_PARAM_REGISTER
    signed int immediateValue;        // kind == ASSEMBLY_PARAM_IMMEDIATE
    TextContentsSpan referencedLabel; // kind == ASSEMBLY_PARAM_LABEL
  };
} AssemblyParameter;

// The instruction represented by an assembly line.
typedef struct {
  // The opcode for this instruction.
  enum Opcode opcode;
  // The number of parameters in the parameters array.
  size_t parameterCount;
  // The parameters given to this instruction. Should be a dynamically allocated array.
  AssemblyParameter* parameters;
} AssemblyInstruction;

// The data represented by an assembly line.
typedef struct {
  // The number of bytes in the bytes array.
  size_t length;
  // The bytes of this data. Should be a dynamically allocated array.
  unsigned char* bytes;
} AssemblyData;

// The kind of an assembly line.
typedef enum {
  ASSEMBLY_LINE_LABEL,
  ASSEMBLY_LINE_ADDRESS,
  ASSEMBLY_LINE_INSTRUCTION,
  ASSEMBLY_LINE_DATA,
} AssemblyLineKind;

// A line of an assembly program.
typedef struct {
  // The location of the line in the source TextContents.
  TextContentsSpan sourceSpan;
  // The kind of line that this is.
  AssemblyLineKind kind;
  union {
    TextContentsSpan labelSpan;      // kind == LABEL. Should be a dynamically allocated string.
    unsigned short address;          // kind == ADDRESS
    AssemblyInstruction instruction; // kind == INSTRUCTION
    AssemblyData data;               // kind == DATA
  };
} AssemblyLine;

// An assembly program consisting of zero or more lines.
typedef struct {
  // The number of lines in the assembly program.
  size_t lineCount;
  // The lines of the assembly program. Should be a dynamically allocated array.
  AssemblyLine* lines;
} AssemblyProgram;

// Frees all memory referenced by the assembly program and replaces freed pointers with NULL.
void DestroyAssemblyProgram(AssemblyProgram* program);

// Frees all memory referenced by the assembly line and replaces freed pointers with NULL.
void DestroyAssemblyLine(AssemblyLine* line);

// Frees all memory referenced by the assembly instruction and replaces freed pointers with NULL.
void DestroyAssemblyInstruction(AssemblyInstruction* instruction);

// Frees all memory referenced by the assembly data and replaces freed pointers with NULL.
void DestroyAssemblyData(AssemblyData* data);
