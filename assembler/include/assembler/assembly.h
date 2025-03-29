#pragma once
#include <stdint.h>
#include "utilities/text.h"
#include "processor/opcode.h"
#include "processor/register.h"

// TODO: Add support for storing references in data sections
// TODO: Add support for relative references?

// The label represented by an assembly line.
typedef struct {
  // The span of the name portion of the label. Empty if name is not present.
  TextSpan nameSpan;
  // The span of the address portion of the label. Empty if address is not present.
  TextSpan addressSpan;
  // The value of the address portion of the label, if present.
  unsigned short address;
} AssemblyLabel;

// The kind of an assembly parameter.
typedef enum {
  ASSEMBLY_PARAM_REGISTER,
  ASSEMBLY_PARAM_IMMEDIATE,
  ASSEMBLY_PARAM_LABEL,
} AssemblyParameterKind;

// The parameter of an assembly instruction.
typedef struct {
  // The location of the parameter in the source TextContents.
  TextSpan sourceSpan;
  // The kind of parameter that this is.
  AssemblyParameterKind kind;
  union {
    enum Register registerName; // kind == ASSEMBLY_PARAM_REGISTER
    signed int immediateValue;  // kind == ASSEMBLY_PARAM_IMMEDIATE
    TextSpan labelSpan;         // kind == ASSEMBLY_PARAM_LABEL
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
  ASSEMBLY_LINE_INSTRUCTION,
  ASSEMBLY_LINE_DATA,
} AssemblyLineKind;

// A line of an assembly program.
typedef struct {
  // The location of the line in the source TextContents.
  TextSpan sourceSpan;
  // The kind of line that this is.
  AssemblyLineKind kind;
  union {
    AssemblyLabel label;             // kind == LABEL
    AssemblyInstruction instruction; // kind == INSTRUCTION
    AssemblyData data;               // kind == DATA
  };
} AssemblyLine;

// An assembly program consisting of zero or more lines.
typedef struct {
  // The text from which this program was parsed.
  TextContents sourceText;
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
