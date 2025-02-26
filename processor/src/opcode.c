#include "processor/opcode.h"

const struct OpcodeInfo OPCODE_INFO[OPCODE_COUNT] = {
  {
    .name = "NOP",
    .parameterLayout = NONE,
  },
  {
    .name = "JMP",
    .parameterLayout = IMM16,
  },
  {
    .name = "JMZ",
    .parameterLayout = IMM16,
  },
  {
    .name = "SLP",
    .parameterLayout = REGA_IMM4,
  },
  {
    .name = "MOV",
    .parameterLayout = REGA_REGB,
  },
  {
    .name = "LDIB",
    .parameterLayout = IMM8,
  },
  {
    .name = "LDIW",
    .parameterLayout = IMM16,
  },
  {
    .name = "LDMB",
    .parameterLayout = REGA_IMM4,
  },
  {
    .name = "LDMW",
    .parameterLayout = REGA_IMM4,
  },
  {
    .name = "STMB",
    .parameterLayout = REGA_IMM4,
  },
  {
    .name = "STMW",
    .parameterLayout = REGA_IMM4,
  },
  {
    .name = "PSHB",
    .parameterLayout = REGA,
  },
  {
    .name = "PSHW",
    .parameterLayout = REGA,
  },
  {
    .name = "POPB",
    .parameterLayout = REGA,
  },
  {
    .name = "POPW",
    .parameterLayout = REGA,
  },
  {
    .name = "INC",
    .parameterLayout = REGA_IMM4,
  },
  {
    .name = "DEC",
    .parameterLayout = REGA_IMM4,
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
    .parameterLayout = REGA_IMM4,
  },
  {
    .name = "RSIS",
    .parameterLayout = REGA_IMM4,
  },
  {
    .name = "RSIU",
    .parameterLayout = REGA_IMM4,
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

enum Opcode decodeOpcode(unsigned char opcodeValue) {
  if (opcodeValue >= 0 && opcodeValue < OPCODE_COUNT) {
    return opcodeValue;
  } else {
    return NOP;
  }
}
