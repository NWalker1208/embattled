#include "processor/opcode.h"

const OpcodeInfo OPCODE_INFO[OPCODE_COUNT] = {
  {
    .identifier = "nop",
    .operandLayout = OPERAND_LAYOUT_NONE,
  }
  // TODO: Define info for other opcodes
};

const OpcodeInfo* getOpcodeInfo(Opcode opcode) {
  if (opcode >= 0 && opcode < OPCODE_COUNT) {
    return &OPCODE_INFO[opcode];
  } else {
    return NULL;
  }
}

Opcode byteToOpcode(uint8_t byte) {
  if (byte < OPCODE_COUNT) {
    return byte;
  } else {
    return OPCODE_NOP;
  }
}
