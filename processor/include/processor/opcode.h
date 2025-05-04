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

// Describes how the parameters are laid out in memory for a given opcode.
typedef struct ProcessorParameterLayout {
  unsigned char numBytes : 3; // The number of bytes used to store all parameters (0-4).
  bool hasRegA : 1; // Whether register A is present.
  bool hasRegB : 1; // Whether register B is present.
  bool hasRegC : 1; // Whether register C is present.
  unsigned char numImmABits : 5; // The number of bits used to store immediate value A (0, 4, 8, or 16).
  bool hasImmB : 1; // Whether immediate value B (16-bit) is present.
} ProcessorParameterLayout;

#define _PROCESSOR_PARAMETER_LAYOUT(_numBytes, _hasRegA, _hasRegB, _hasRegC, _numImmABits, _hasImmB) \
  (struct ProcessorParameterLayout){ \
    .numBytes=(_numBytes), \
    .hasRegA=(_hasRegA), \
    .hasRegB=(_hasRegB), \
    .hasRegC=(_hasRegC), \
    .numImmABits=(_numImmABits), \
    .hasImmB=(_hasImmB) \
  }
// 0 bytes
#define PROCESSOR_PARAMETER_LAYOUT_NONE             _PROCESSOR_PARAMETER_LAYOUT(0, false, false, false, 0, false)
// 1 byte
#define PROCESSOR_PARAMETER_LAYOUT_REGA             _PROCESSOR_PARAMETER_LAYOUT(1, true, false, false, 0, false)
#define PROCESSOR_PARAMETER_LAYOUT_REGA_REGB        _PROCESSOR_PARAMETER_LAYOUT(1, true, true, false, 0, false)
// 2 bytes
#define PROCESSOR_PARAMETER_LAYOUT_IMMA16           _PROCESSOR_PARAMETER_LAYOUT(2, false, false, false, 16, false)
#define PROCESSOR_PARAMETER_LAYOUT_REGA_IMMA8       _PROCESSOR_PARAMETER_LAYOUT(2, true, false, false, 8, false)
#define PROCESSOR_PARAMETER_LAYOUT_REGA_REGB_IMMA4  _PROCESSOR_PARAMETER_LAYOUT(2, true, true, false, 4, false)
#define PROCESSOR_PARAMETER_LAYOUT_REGA_REGB_REGC   _PROCESSOR_PARAMETER_LAYOUT(2, true, true, true, 0, false)
// 3 bytes
#define PROCESSOR_PARAMETER_LAYOUT_IMMA8_IMMB16     _PROCESSOR_PARAMETER_LAYOUT(3, false, false, false, 8, true)
#define PROCESSOR_PARAMETER_LAYOUT_REGA_IMMA16      _PROCESSOR_PARAMETER_LAYOUT(3, true, false, false, 16, false)
#define PROCESSOR_PARAMETER_LAYOUT_REGA_REGB_IMMA16 _PROCESSOR_PARAMETER_LAYOUT(3, true, true, false, 16, false)
// 4 bytes
#define PROCESSOR_PARAMETER_LAYOUT_IMMA16_IMMB16    _PROCESSOR_PARAMETER_LAYOUT(4, false, false, false, 16, true)

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
