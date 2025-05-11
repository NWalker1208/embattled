#include "assembler/mnemonic.h"

const AssemblyMnemonicInfo MNEMONIC_INFO[ASSEMBLY_MNEMONIC_COUNT] = {
  { .identifier = "nop", .overloadCount = 1, .overloads = {
    { .operandCount = 0, .opcode = OPCODE_NOP }
  } },

  { .identifier = "jmp", .overloadCount = 2, .overloads = {
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_REGISTER }, .opcode = OPCODE_JMP_R },
    { .operandCount = 1, .operandKinds = { ASSEMBLY_OPERAND_IMMEDIATE }, .opcode = OPCODE_JMP_I },
  } },
};

const AssemblyMnemonicInfo* getAssemblyMnemonicInfo(AssemblyMnemonic mnemonic) {
  if (mnemonic >= 0 && mnemonic < ASSEMBLY_MNEMONIC_COUNT) {
    return &MNEMONIC_INFO[mnemonic];
  } else {
    return NULL;
  }
}
