#pragma once
#include <stdint.h>
#include "processor/operand.h"
#include "processor/process.h"

// A code indicating which action the processor should perform for a given instruction.
// Representable by one byte.
typedef enum Opcode {
  OPCODE_NOP,

  OPCODE_JMP_R,
  OPCODE_JMP_I,
  
  OPCODE_JMZ_R,
  OPCODE_JMZ_I,
  
  OPCODE_SLP_R,
  OPCODE_SLP_I,
  
  OPCODE_SET_R,
  OPCODE_SET_I,
  
  OPCODE_LDB_R,
  OPCODE_LDB_I,
  
  OPCODE_LDW_R,
  OPCODE_LDW_I,
  
  OPCODE_STB_RR,
  OPCODE_STB_RI,
  OPCODE_STB_IR,
  OPCODE_STB_II,
  
  OPCODE_STW_RR,
  OPCODE_STW_RI,
  OPCODE_STW_IR,
  OPCODE_STW_II,
  
  OPCODE_PSHB,
  
  OPCODE_PSHW,
  
  OPCODE_POPB,
  
  OPCODE_POPW,
  
  OPCODE_ADD_R,
  OPCODE_ADD_I,
  
  OPCODE_SUB_RR,
  OPCODE_SUB_RI,
  OPCODE_SUB_IR,
  
  OPCODE_MUL_R,
  OPCODE_MUL_I,
  
  OPCODE_DIVS_RR,
  OPCODE_DIVS_RI,
  OPCODE_DIVS_IR,
  
  OPCODE_DIVU_RR,
  OPCODE_DIVU_RI,
  OPCODE_DIVU_IR,
  
  OPCODE_REMS_RR,
  OPCODE_REMS_RI,
  OPCODE_REMS_IR,
  
  OPCODE_REMU_RR,
  OPCODE_REMU_RI,
  OPCODE_REMU_IR,
  
  OPCODE_AND_R,
  OPCODE_AND_I,
  
  OPCODE_IOR_R,
  OPCODE_IOR_I,
  
  OPCODE_XOR_R,
  OPCODE_XOR_I,
  
  OPCODE_LSH_RR,
  OPCODE_LSH_RI,
  OPCODE_LSH_IR,
  
  OPCODE_RSHS_RR,
  OPCODE_RSHS_RI,
  OPCODE_RSHS_IR,
  
  OPCODE_RSHU_RR,
  OPCODE_RSHU_RI,
  OPCODE_RSHU_IR,
  
  OPCODE_CEQ_R,
  OPCODE_CEQ_I,
  
  OPCODE_CNE_R,
  OPCODE_CNE_I,
  
  OPCODE_CLTS_RR,
  OPCODE_CLTS_RI,
  OPCODE_CLTS_IR,
  
  OPCODE_CLTU_RR,
  OPCODE_CLTU_RI,
  OPCODE_CLTU_IR,
  
  OPCODE_CGES_RR,
  OPCODE_CGES_RI,
  OPCODE_CGES_IR,
  
  OPCODE_CGEU_RR,
  OPCODE_CGEU_RI,
  OPCODE_CGEU_IR,

  OPCODE_COUNT
} Opcode;

// Describes the details of a particular opcode.
typedef struct OpcodeInfo {
  const char* identifier; // The identifier of the opcode as a string.
  OperandLayout operandLayout; // The layout of the opcode's operands.
  void (*execute)(ProcessState* state, InstructionOperands operands); // A function which executes the opcode against the provided process state and with the provided operands.
} OpcodeInfo;

// Gets the info for the specified opcode.
// If the opcode is invalid, returns NULL.
const OpcodeInfo* getOpcodeInfo(Opcode opcode);

// Converts a byte to an opcode.
// Returns NOP if the byte is not a valid opcode.
// Note: A valid opcode can safely be cast to a uint8_t to obtain its byte value.
Opcode byteToOpcode(uint8_t byte);
