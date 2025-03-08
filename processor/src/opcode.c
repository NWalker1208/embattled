#include "processor/opcode.h"

const struct OpcodeInfo OPCODE_INFO[OPCODE_COUNT] = {
  {
    .name = "NOP",
    .parameterLayout = NONE,
  },
  {
    .name = "JMP",
    .parameterLayout = IMMU16,
  },
  {
    .name = "JMZ",
    .parameterLayout = IMMU16,
  },
  {
    .name = "SLP",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "MOV",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "LDIB",
    .parameterLayout = IMMU8,
  },
  {
    .name = "LDIW",
    .parameterLayout = IMMU16,
  },
  {
    .name = "LDMB",
    .parameterLayout = REGA_IMMS4,
  },
  {
    .name = "LDMW",
    .parameterLayout = REGA_IMMS4,
  },
  {
    .name = "STMB",
    .parameterLayout = REGA_IMMS4,
  },
  {
    .name = "STMW",
    .parameterLayout = REGA_IMMS4,
  },
  {
    .name = "PSHB",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "PSHW",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "POPB",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "POPW",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "INC",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "DEC",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "ADD",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "SUB",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "MUL",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "DIVS",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "DIVU",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "REMS",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "REMU",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "LSH",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "RSHS",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "RSHU",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "LSI",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "RSIS",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "RSIU",
    .parameterLayout = REGA_IMMU4,
  },
  {
    .name = "AND",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "IOR",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "XOR",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "CEQ",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "CNE",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "CLTS",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "CLTU",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "CGES",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "CGEU",
    .parameterLayout = REGA_REGB,
  }
};

enum Opcode byteToOpcode(unsigned char byte) {
  if (byte >= 0 && byte < OPCODE_COUNT) {
    return byte;
  } else {
    return NOP;
  }
}
