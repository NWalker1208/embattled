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
};

const AssemblyMnemonicInfo* getAssemblyMnemonicInfo(AssemblyMnemonic mnemonic) {
  if (mnemonic >= 0 && mnemonic < ASSEMBLY_MNEMONIC_COUNT) {
    return &MNEMONIC_INFO[mnemonic];
  } else {
    return NULL;
  }
}
