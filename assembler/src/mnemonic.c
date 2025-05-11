#include "assembler/mnemonic.h"

const AssemblyMnemonicInfo MNEMONIC_INFO[ASSEMBLY_MNEMONIC_COUNT] = {
  // Control flow

  { .identifier = "nop", .overloadCount = 1, .overloads = {
    { .operandCount = 0, .opcode = OPCODE_NOP }
  } },

  { .identifier = "jmp", .overloadCount = 2, .overloads = {
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_JMP_R },
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_JMP_I },
  } },

  { .identifier = "jmz", .overloadCount = 2, .overloads = {
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_JMZ_R },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_JMZ_I },
  } },

  { .identifier = "slp", .overloadCount = 2, .overloads = {
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_SLP_R },
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_SLP_I },
  } },

  // Memory

  { .identifier = "set", .overloadCount = 2, .overloads = {
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_SET_R },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_SET_I },
  } },

  { .identifier = "ldb", .overloadCount = 2, .overloads = {
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_LDB_R },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_LDB_I },
  } },

  { .identifier = "ldw", .overloadCount = 2, .overloads = {
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_LDW_R },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_LDW_I },
  } },

  { .identifier = "stb", .overloadCount = 4, .overloads = {
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_STB_RR },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_STB_RI },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_STB_IR },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_STB_II },
  } },

  { .identifier = "stw", .overloadCount = 4, .overloads = {
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_STW_RR },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_STW_RI },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_STW_IR },
    { .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_STW_II },
  } },

  { .identifier = "pshb", .overloadCount = 1, .overloads = {
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_PSHB },
  } },

  { .identifier = "pshw", .overloadCount = 1, .overloads = {
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_PSHW },
  } },

  { .identifier = "popb", .overloadCount = 1, .overloads = {
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_POPB },
  } },

  { .identifier = "popw", .overloadCount = 1, .overloads = {
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_POPW },
  } },

  // Math

  { .identifier = "add", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_ADD_R },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_ADD_I },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_ADD_I },
  } },

  { .identifier = "sub", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_SUB_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_SUB_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_SUB_IR },
  } },

  { .identifier = "mul", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_MUL_R },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_MUL_I },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_MUL_I },
  } },

  { .identifier = "divs", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_DIVS_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_DIVS_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_DIVS_IR },
  } },

  { .identifier = "divu", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_DIVU_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_DIVU_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_DIVU_IR },
  } },

  { .identifier = "rems", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_REMS_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_REMS_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_REMS_IR },
  } },

  { .identifier = "remu", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_REMU_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_REMU_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_REMU_IR },
  } },

  // Bitwise logic

  { .identifier = "and", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_AND_R },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_AND_I },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_AND_I },
  } },

  { .identifier = "ior", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_IOR_R },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_IOR_I },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_IOR_I },
  } },

  { .identifier = "xor", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_XOR_R },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_XOR_I },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_XOR_I },
  } },

  { .identifier = "lsh", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_LSH_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_LSH_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_LSH_IR },
  } },

  { .identifier = "rshs", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_RSHS_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_RSHS_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_RSHS_IR },
  } },

  { .identifier = "rshu", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_RSHU_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_RSHU_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_RSHU_IR },
  } },

  // Comparison

  { .identifier = "ceq", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CEQ_R },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CEQ_I },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CEQ_I },
  } },

  { .identifier = "cne", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CNE_R },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CNE_I },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CNE_I },
  } },

  { .identifier = "clts", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CLTS_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CLTS_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CLTS_IR },
  } },

  { .identifier = "cltu", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CLTU_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CLTU_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CLTU_IR },
  } },

  { .identifier = "cges", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CGES_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CGES_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CGES_IR },
  } },

  { .identifier = "cgeu", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CGEU_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CGEU_RI },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CGEU_IR },
  } },

  { .identifier = "cgts", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .swapRegisterBAndRegisterC = true, .opcode = OPCODE_CLTS_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CLTS_IR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CLTS_RI },
  } },

  { .identifier = "cgtu", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .swapRegisterBAndRegisterC = true, .opcode = OPCODE_CLTU_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CLTU_IR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CLTU_RI },
  } },

  { .identifier = "cles", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .swapRegisterBAndRegisterC = true, .opcode = OPCODE_CGES_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CGES_IR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CGES_RI },
  } },

  { .identifier = "cleu", .overloadCount = 3, .overloads = {
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .swapRegisterBAndRegisterC = true, .opcode = OPCODE_CGEU_RR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_CGEU_IR },
    { .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_CGEU_RI },
  } },
};

const AssemblyMnemonicInfo* getAssemblyMnemonicInfo(AssemblyMnemonic mnemonic) {
  if (mnemonic >= 0 && mnemonic < ASSEMBLY_MNEMONIC_COUNT) {
    return &MNEMONIC_INFO[mnemonic];
  } else {
    return NULL;
  }
}
