#pragma once
#include <stdbool.h>

// A code indicating which action the processor should perform for a given instruction.
// Representable by one byte.
typedef enum ProcessorOpcode {
  PROCESSOR_OPCODE_NOP,

  PROCESSOR_OPCODE_JMP_R,
  PROCESSOR_OPCODE_JMP_I,
  
  PROCESSOR_OPCODE_JMZ_R,
  PROCESSOR_OPCODE_JMZ_I,
  
  PROCESSOR_OPCODE_SLP_R,
  PROCESSOR_OPCODE_SLP_I,
  
  PROCESSOR_OPCODE_SET_R,
  PROCESSOR_OPCODE_SET_I,
  
  PROCESSOR_OPCODE_LDB_R,
  PROCESSOR_OPCODE_LDB_I,
  
  PROCESSOR_OPCODE_LDW_R,
  PROCESSOR_OPCODE_LDW_I,
  
  PROCESSOR_OPCODE_STB_RR,
  PROCESSOR_OPCODE_STB_RI,
  PROCESSOR_OPCODE_STB_IR,
  PROCESSOR_OPCODE_STB_II,
  
  PROCESSOR_OPCODE_STW_RR,
  PROCESSOR_OPCODE_STW_RI,
  PROCESSOR_OPCODE_STW_IR,
  PROCESSOR_OPCODE_STW_II,
  
  PROCESSOR_OPCODE_PSHB,
  
  PROCESSOR_OPCODE_PSHW,
  
  PROCESSOR_OPCODE_POPB,
  
  PROCESSOR_OPCODE_POPW,
  
  PROCESSOR_OPCODE_ADD_R,
  PROCESSOR_OPCODE_ADD_I,
  
  PROCESSOR_OPCODE_SUB_RR,
  PROCESSOR_OPCODE_SUB_RI,
  PROCESSOR_OPCODE_SUB_IR,
  
  PROCESSOR_OPCODE_MUL_R,
  PROCESSOR_OPCODE_MUL_I,
  
  PROCESSOR_OPCODE_DIVS_RR,
  PROCESSOR_OPCODE_DIVS_RI,
  PROCESSOR_OPCODE_DIVS_IR,
  
  PROCESSOR_OPCODE_DIVU_RR,
  PROCESSOR_OPCODE_DIVU_RI,
  PROCESSOR_OPCODE_DIVU_IR,
  
  PROCESSOR_OPCODE_REMS_RR,
  PROCESSOR_OPCODE_REMS_RI,
  PROCESSOR_OPCODE_REMS_IR,
  
  PROCESSOR_OPCODE_REMU_RR,
  PROCESSOR_OPCODE_REMU_RI,
  PROCESSOR_OPCODE_REMU_IR,
  
  PROCESSOR_OPCODE_AND_R,
  PROCESSOR_OPCODE_AND_I,
  
  PROCESSOR_OPCODE_IOR_R,
  PROCESSOR_OPCODE_IOR_I,
  
  PROCESSOR_OPCODE_XOR_R,
  PROCESSOR_OPCODE_XOR_I,
  
  PROCESSOR_OPCODE_LSH_RR,
  PROCESSOR_OPCODE_LSH_RI,
  PROCESSOR_OPCODE_LSH_IR,
  
  PROCESSOR_OPCODE_RSHS_RR,
  PROCESSOR_OPCODE_RSHS_RI,
  PROCESSOR_OPCODE_RSHS_IR,
  
  PROCESSOR_OPCODE_RSHU_RR,
  PROCESSOR_OPCODE_RSHU_RI,
  PROCESSOR_OPCODE_RSHU_IR,
  
  PROCESSOR_OPCODE_CEQ_R,
  PROCESSOR_OPCODE_CEQ_I,
  
  PROCESSOR_OPCODE_CNE_R,
  PROCESSOR_OPCODE_CNE_I,
  
  PROCESSOR_OPCODE_CLTS_RR,
  PROCESSOR_OPCODE_CLTS_RI,
  PROCESSOR_OPCODE_CLTS_IR,
  
  PROCESSOR_OPCODE_CLTU_RR,
  PROCESSOR_OPCODE_CLTU_RI,
  PROCESSOR_OPCODE_CLTU_IR,
  
  PROCESSOR_OPCODE_CGES_RR,
  PROCESSOR_OPCODE_CGES_RI,
  PROCESSOR_OPCODE_CGES_IR,
  
  PROCESSOR_OPCODE_CGEU_RR,
  PROCESSOR_OPCODE_CGEU_RI,
  PROCESSOR_OPCODE_CGEU_IR,

  PROCESSOR_OPCODE_COUNT
} ProcessorOpcode;

// Describes how the parameters are laid out for a given opcode.
struct ParameterLayout {
  unsigned char numBytes : 2; // The number of bytes used to store all parameters.
  bool hasRegA : 1; // Whether register A is used.
  bool hasRegB : 1; // Whether register B is used.
  unsigned char numImmBits : 5; // The number of bits used to store the immediate value.
  bool immIsSigned : 1; // Whether the immediate value is treated as signed.
};

#define PARAM_LAYOUT(_numBytes, _hasRegA, _hasRegB, _numImmBits, _immIsSigned) \
  (struct ParameterLayout){ \
    .numBytes=(_numBytes), \
    .hasRegA=(_hasRegA), \
    .hasRegB=(_hasRegB), \
    .numImmBits=(_numImmBits), \
    .immIsSigned=(_immIsSigned) \
  }
// 0 bytes
#define PARAM_LAYOUT_NONE             PARAM_LAYOUT(0, false, false, 0, false)
// 1 byte
#define PARAM_LAYOUT_IMMU8            PARAM_LAYOUT(1, false, false, 8, false)
#define PARAM_LAYOUT_IMMS8            PARAM_LAYOUT(1, false, false, 8, true)
#define PARAM_LAYOUT_REGA_IMMU4       PARAM_LAYOUT(1, true, false, 4, false)
#define PARAM_LAYOUT_REGA_IMMS4       PARAM_LAYOUT(1, true, false, 4, true)
#define PARAM_LAYOUT_REGA             PARAM_LAYOUT(1, true, false, 0, false)
#define PARAM_LAYOUT_REGA_REGB        PARAM_LAYOUT(1, true, true, 0, false)
// 2 bytes
#define PARAM_LAYOUT_IMMU16           PARAM_LAYOUT(2, false, false, 16, false)
#define PARAM_LAYOUT_IMMS16           PARAM_LAYOUT(2, false, false, 16, true)
#define PARAM_LAYOUT_REGA_IMMU12      PARAM_LAYOUT(2, true, false, 12, false)
#define PARAM_LAYOUT_REGA_IMMS12      PARAM_LAYOUT(2, true, false, 12, true)
#define PARAM_LAYOUT_REGA_REGB_IMMU8  PARAM_LAYOUT(2, true, true, 8, false)
#define PARAM_LAYOUT_REGA_REGB_IMMS8  PARAM_LAYOUT(2, true, true, 8, true)
// 3 bytes
#define PARAM_LAYOUT_REGA_REGB_IMMU16 PARAM_LAYOUT(3, true, true, 16, false)
#define PARAM_LAYOUT_REGA_REGB_IMMS16 PARAM_LAYOUT(3, true, true, 16, true)

// Describes the details of a particular opcode.
struct OpcodeInfo {
  const char* name;
  struct ParameterLayout parameterLayout;
  // TODO: Consider adding function pointer for implementation.
};

// The details of each opcode. Can be indexed directly by a valid Opcode value.
extern const struct OpcodeInfo OPCODE_INFO[];

// Converts a byte to an opcode.
// Returns NOP if the byte is not a valid opcode.
// Note: A valid opcode can safely be cast to an unsigned char to obtain its byte value.
enum Opcode byteToOpcode(unsigned char byte);
