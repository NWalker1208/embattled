#include "assembler/mnemonic.h"

const AssemblyMnemonicInfo MNEMONIC_INFO[ASSEMBLY_MNEMONIC_COUNT] = {
  0
};

const AssemblyMnemonicInfo* getAssemblyMnemonicInfo(AssemblyMnemonic mnemonic) {
  if (mnemonic >= 0 && mnemonic < ASSEMBLY_MNEMONIC_COUNT) {
    return &MNEMONIC_INFO[mnemonic];
  } else {
    return NULL;
  }
}
