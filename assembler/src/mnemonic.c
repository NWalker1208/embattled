#include "assembler/mnemonic.h"

const AssemblyMnemonicInfo MNEMONIC_INFO[ASSEMBLY_MNEMONIC_COUNT] = {
  // Control flow

  { .identifier = "nop", .overloadCount = 1, .overloads = {
    { .opcode = OPCODE_NOP, .operandCount = 0 }
  } },

  { .identifier = "jmp", .overloadCount = 2, .overloads = {
    { .opcode = OPCODE_JMP_R, .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_JMP_I, .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE } },
  } },

  { .identifier = "jmz", .overloadCount = 2, .overloads = {
    { .opcode = OPCODE_JMZ_R, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_JMZ_I, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
  } },

  { .identifier = "slp", .overloadCount = 2, .overloads = {
    { .opcode = OPCODE_SLP_R, .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_SLP_I, .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE } },
  } },

  // Memory

  { .identifier = "set", .overloadCount = 2, .overloads = {
    { .opcode = OPCODE_SET_R, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_SET_I, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
  } },

  { .identifier = "ldb", .overloadCount = 2, .overloads = {
    { .opcode = OPCODE_LDB_R, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_LDB_I, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
  } },

  { .identifier = "ldw", .overloadCount = 2, .overloads = {
    { .opcode = OPCODE_LDW_R, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_LDW_I, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
  } },

  { .identifier = "stb", .overloadCount = 4, .overloads = {
    { .opcode = OPCODE_STB_RR, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_STB_RI, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_STB_IR, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_STB_II, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_IMMEDIATE } },
  } },

  { .identifier = "stw", .overloadCount = 4, .overloads = {
    { .opcode = OPCODE_STW_RR, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_STW_RI, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_STW_IR, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_STW_II, .operandCount = 2, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_IMMEDIATE } },
  } },

  { .identifier = "pshb", .overloadCount = 1, .overloads = {
    { .opcode = OPCODE_PSHB, .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "pshw", .overloadCount = 1, .overloads = {
    { .opcode = OPCODE_PSHW, .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "popb", .overloadCount = 1, .overloads = {
    { .opcode = OPCODE_POPB, .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "popw", .overloadCount = 1, .overloads = {
    { .opcode = OPCODE_POPW, .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER } },
  } },

  // Math

  { .identifier = "add", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_ADD_R, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_ADD_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_ADD_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "sub", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_SUB_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_SUB_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_SUB_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "mul", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_MUL_R, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_MUL_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_MUL_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "divs", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_DIVS_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_DIVS_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_DIVS_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "divu", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_DIVU_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_DIVU_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_DIVU_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "rems", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_REMS_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_REMS_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_REMS_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "remu", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_REMU_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_REMU_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_REMU_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  // Bitwise logic

  { .identifier = "and", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_AND_R, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_AND_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_AND_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "ior", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_IOR_R, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_IOR_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_IOR_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "xor", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_XOR_R, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_XOR_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_XOR_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "lsh", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_LSH_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_LSH_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_LSH_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "rshs", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_RSHS_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_RSHS_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_RSHS_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "rshu", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_RSHU_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_RSHU_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_RSHU_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  // Comparison

  { .identifier = "ceq", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CEQ_R, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_CEQ_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CEQ_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "cne", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CNE_R, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_CNE_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CNE_I, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "clts", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CLTS_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_CLTS_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CLTS_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "cltu", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CLTU_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_CLTU_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CLTU_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "cges", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CGES_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_CGES_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CGES_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "cgeu", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CGEU_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER } },
    { .opcode = OPCODE_CGEU_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CGEU_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "cgts", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CLTS_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .swapRegisterBAndRegisterC = true },
    { .opcode = OPCODE_CLTS_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CLTS_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "cgtu", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CLTU_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .swapRegisterBAndRegisterC = true },
    { .opcode = OPCODE_CLTU_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CLTU_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "cles", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CGES_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .swapRegisterBAndRegisterC = true },
    { .opcode = OPCODE_CGES_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CGES_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },

  { .identifier = "cleu", .overloadCount = 3, .overloads = {
    { .opcode = OPCODE_CGEU_RR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER }, .swapRegisterBAndRegisterC = true },
    { .opcode = OPCODE_CGEU_IR, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE } },
    { .opcode = OPCODE_CGEU_RI, .operandCount = 3, .operandKinds = { ASSEMBLY_OPERAND_REGISTER, ASSEMBLY_OPERAND_IMMEDIATE, ASSEMBLY_OPERAND_REGISTER } },
  } },
};

const AssemblyMnemonicInfo* getAssemblyMnemonicInfo(AssemblyMnemonic mnemonic) {
  if (mnemonic >= 0 && mnemonic < ASSEMBLY_MNEMONIC_COUNT) {
    return &MNEMONIC_INFO[mnemonic];
  } else {
    return NULL;
  }
}
