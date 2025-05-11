#pragma once
#include <stdint.h>
#include "assembler/mnemonic.h"
#include "utilities/text.h"
#include "processor/opcode.h"
#include "processor/register.h"

#define MAX_ASSEMBLY_OPERANDS 3

// TODO: Add support for storing references in data sections
// TODO: Add support for relative references?

// A label represented by a line of assembly code.
typedef struct {
  // The span of the name portion of the label. Empty if name is not present.
  TextSpan nameSpan;
  // The span of the address portion of the label. Empty if address is not present.
  TextSpan addressSpan;
  // The value of the address portion of the label, if present.
  uint16_t address;
} AssemblyLabel;

// The kind of an assembly operand.
typedef enum AssemblyOperandKind {
  ASSEMBLY_OPERAND_REGISTER,
  ASSEMBLY_OPERAND_IMMEDIATE,
  ASSEMBLY_OPERAND_LABEL,
} AssemblyOperandKind;

// An operand of an assembly instruction.
typedef struct AssemblyOperand {
  // The location of the parameter in the source TextContents.
  TextSpan sourceSpan;
  // The kind of parameter that this is.
  AssemblyOperandKind kind;
  union {
    Register registerName;  // kind == ASSEMBLY_OPERAND_REGISTER
    int32_t immediateValue; // kind == ASSEMBLY_OPERAND_IMMEDIATE
    TextSpan labelSpan;     // kind == ASSEMBLY_OPERAND_LABEL
  };
} AssemblyOperand;

// An instruction represented by a line of assembly code.
typedef struct AssemblyInstruction {
  // The mnemonic for this instruction's opcode.
  AssemblyMnemonic mnemonic;
  // The number of items in the operands array (up to MAX_ASSEMBLY_OPERANDS).
  size_t operandCount;
  // The operands of this instruction.
  AssemblyOperand operands[MAX_ASSEMBLY_OPERANDS];
} AssemblyInstruction;

// Some data represented by a line of assembly code.
typedef struct AssemblyData {
  // The index of the start of this data in the program data buffer.
  size_t startIndex;
  // The number of bytes in this data.
  size_t byteCount;
} AssemblyData;

// The kind of a line of assembly code.
typedef enum AssemblyLineKind {
  ASSEMBLY_LINE_LABEL,
  ASSEMBLY_LINE_INSTRUCTION,
  ASSEMBLY_LINE_DATA,
} AssemblyLineKind;

// A line of assembly code.
typedef struct AssemblyLine {
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
typedef struct AssemblyProgram {
  // The number of lines in the assembly program.
  size_t lineCount;
  // The lines of the assembly program. Should be a dynamically allocated array.
  AssemblyLine* lines;
  // The buffer for any assembly data in the program. Should be a dynamically allocated array.
  uint8_t* dataBuffer;
} AssemblyProgram;

// Frees all memory referenced by the assembly program and replaces freed pointers with NULL.
void DestroyAssemblyProgram(AssemblyProgram* program);
