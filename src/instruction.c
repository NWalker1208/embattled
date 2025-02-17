#include "instruction.h"

enum Opcode decodeOpcode(unsigned char opcodeValue) {
  for (int i = 0; i < OPCODE_COUNT; i++) {
    if (opcodeValue == OPCODE_VALUES[i]) {
      return i;
    }
  }
  return NOP;
}

enum Register decodeRegister(unsigned char registerId) {
  for (int i = 0; i < REGISTER_COUNT; i++) {
    if (registerId == REGISTER_IDS[i]) {
      return i;
    }
  }
  return NL;
}
