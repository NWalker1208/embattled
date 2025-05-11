#pragma once
#include <stdint.h>
#include "utilities/text.h"
#include "processor/register.h"

#define MAX_ASSEMBLY_OPERANDS 3

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
