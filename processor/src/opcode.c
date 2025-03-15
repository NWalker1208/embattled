#include "processor/opcode.h"

const struct OpcodeInfo OPCODE_INFO[OPCODE_COUNT] = {
  {
    .name = "NOP",
    .parameterLayout = PARAM_LAYOUT_NONE,
  },
  {
    .name = "JMP",
    .parameterLayout = PARAM_LAYOUT_IMMU16,
  },
  {
    .name = "JMZ",
    .parameterLayout = PARAM_LAYOUT_IMMU16,
  },
  {
    .name = "SLP",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMU4,
  },
  {
    .name = "MOV",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "LDIB",
    .parameterLayout = PARAM_LAYOUT_IMMU8,
  },
  {
    .name = "LDIW",
    .parameterLayout = PARAM_LAYOUT_IMMU16,
  },
  {
    .name = "LDMB",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMS4,
  },
  {
    .name = "LDMW",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMS4,
  },
  {
    .name = "STMB",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMS4,
  },
  {
    .name = "STMW",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMS4,
  },
  {
    .name = "PSHB",
    .parameterLayout = PARAM_LAYOUT_REGA,
  },
  {
    .name = "PSHW",
    .parameterLayout = PARAM_LAYOUT_REGA,
  },
  {
    .name = "POPB",
    .parameterLayout = PARAM_LAYOUT_REGA,
  },
  {
    .name = "POPW",
    .parameterLayout = PARAM_LAYOUT_REGA,
  },
  {
    .name = "INC",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMU4,
  },
  {
    .name = "DEC",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMU4,
  },
  {
    .name = "ADD",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "SUB",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "MUL",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "DIVS",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "DIVU",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "REMS",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "REMU",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "LSH",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "RSHS",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "RSHU",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "LSI",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMU4,
  },
  {
    .name = "RSIS",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMU4,
  },
  {
    .name = "RSIU",
    .parameterLayout = PARAM_LAYOUT_REGA_IMMU4,
  },
  {
    .name = "AND",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "IOR",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "XOR",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "CEQ",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "CNE",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "CLTS",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "CLTU",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "CGES",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  },
  {
    .name = "CGEU",
    .parameterLayout = PARAM_LAYOUT_REGA_REGB,
  }
};

enum Opcode byteToOpcode(unsigned char byte) {
  if (byte < OPCODE_COUNT) {
    return byte;
  } else {
    return NOP;
  }
}
