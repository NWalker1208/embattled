#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "processor/instruction.h"

uint16_t fetchInstruction(const uint8_t* memory, uint16_t addr, Instruction* instructionOut) {
  // Read the opcode from memory.
  instructionOut->opcode = byteToOpcode(memory[addr]);

  const OpcodeInfo* opcodeInfo = getOpcodeInfo(instructionOut->opcode);
  uint8_t numBytes = opcodeInfo->layout.numBytes;
  
  if (numBytes == 1) {
    instructionOut->operands = (InstructionOperands){0};
    return 1;
  }

  // Read the operand bytes from memory.
  addr++;
  uint8_t numOperandBytes = numBytes - 1;
  unsigned char operandBytes[4] = { 0 };
  for (int i = 0; i < numOperandBytes; i++) {
    operandBytes[i] = memory[addr];
    addr++;
  }

  // Decode the register operands.
  bool hasRegA = opcodeInfo->layout.hasRegA;
  bool hasRegB = opcodeInfo->layout.hasRegB;
  bool hasRegC = opcodeInfo->layout.hasRegC;

  if (hasRegA) {
    instructionOut->operands.registerA = nibbleToRegister(operandBytes[numOperandBytes - 1] >> 4);
  } else {
    instructionOut->operands.registerA = REGISTER_NL;
  }

  if (hasRegB) {
    instructionOut->operands.registerB = nibbleToRegister(operandBytes[numOperandBytes - 1] & 0xF);
  } else {
    instructionOut->operands.registerB = REGISTER_NL;
  }

  if (hasRegC) {
    instructionOut->operands.registerC = nibbleToRegister(operandBytes[numOperandBytes - 2] >> 4);
  } else {
    instructionOut->operands.registerC = REGISTER_NL;
  }

  // Decode the immediate value operands.
  uint8_t numImmABits = opcodeInfo->layout.numImmABits;
  bool hasImmB = opcodeInfo->layout.hasImmB;

  assert(numImmABits == 0 || numImmABits == 4 || numImmABits == 8 || numImmABits == 16);
  uint8_t immBOffset;
  switch (numImmABits) {
    case 0: {
      instructionOut->operands.immediateA.u16 = 0;
      immBOffset = 0;
    } break;
    case 4: {
      instructionOut->operands.immediateA.u16 = (uint16_t)(operandBytes[0] & 0xF);
      immBOffset = 1;
    } break;
    case 8: {
      instructionOut->operands.immediateA.u16 = (uint16_t)operandBytes[0];
      immBOffset = 1;
    } break;
    case 16: {
      instructionOut->operands.immediateA.u16 = (uint16_t)operandBytes[0] | ((uint16_t)operandBytes[1] << 8);
      immBOffset = 2;
    } break;
  }

  if (hasImmB) {
    instructionOut->operands.immediateB.u16 = (uint16_t)operandBytes[immBOffset] | ((uint16_t)operandBytes[immBOffset + 1] << 8);
  } else {
    instructionOut->operands.immediateB.u16 = 0;
  }

  return numBytes;
}

uint16_t writeInstruction(uint8_t* memory, uint16_t addr, Instruction instruction) {
  // Check if the opcode is valid and get the associated info.
  const OpcodeInfo* opcodeInfo = getOpcodeInfo(instruction.opcode);
  if (opcodeInfo == NULL) {
    return 0;
  }

  // Write the opcode byte to memory.
  memory[addr] = (uint8_t)instruction.opcode;
  addr++;
  
  uint8_t numBytes = opcodeInfo->layout.numBytes;
  if (numBytes == 1) {
    return 1;
  }

  // Encode the immediate value operands.
  uint8_t numImmABits = opcodeInfo->layout.numImmABits;
  bool hasImmB = opcodeInfo->layout.hasImmB;

  if (numImmABits > 0) {
    if (numImmABits == 4) {
      memory[addr] = (uint8_t)(instruction.operands.immediateA.u16 & 0x0F);
    } else {
      memory[addr] = (uint8_t)(instruction.operands.immediateA.u16 & 0xFF);
    }
    addr++;
    if (numImmABits == 16) {
      memory[addr] = (uint8_t)(instruction.operands.immediateA.u16 >> 8);
      addr++;
    }
  }
  if (hasImmB) {
    memory[addr] = (uint8_t)(instruction.operands.immediateB.u16 & 0xFF);
    addr++;
    memory[addr] = (uint8_t)(instruction.operands.immediateB.u16 >> 8);
    addr++;
  }
  
  // Encode the register operands.
  bool hasRegA = opcodeInfo->layout.hasRegA;
  bool hasRegB = opcodeInfo->layout.hasRegB;
  bool hasRegC = opcodeInfo->layout.hasRegC;

  // Note: None of the layouts used place a 4-bit immediate value alongside a register.
  //       If such a layout is ever used, this code will need to be modified to combine the immediate value with the register in a single byte.

  if (hasRegC) {
    memory[addr] = (uint8_t)((instruction.operands.registerC & 0x0F) << 4);
    addr++;
  }

  if (hasRegA || hasRegB) {
    uint8_t value = 0;
    if (hasRegA) {
      value = (uint8_t)((instruction.operands.registerA & 0x0F) << 4);
    }
    if (hasRegB) {
      value = (value & 0xF0) | (uint8_t)(instruction.operands.registerB & 0x0F);
    }
    memory[addr] = value;
    addr++;
  }

  return numBytes;
}

void printInstruction(Instruction instruction) {
  const OpcodeInfo* opcodeInfo = getOpcodeInfo(instruction.opcode);
  if (opcodeInfo == NULL) {
    printf("opcode=invalid\n");
    return;
  }

  printf("opcode=%-7s", opcodeInfo->identifier);
  if (opcodeInfo->layout.hasRegA) {
    printf("  regA=%-3s", getRegisterIdentifier(instruction.operands.registerA));
  }
  if (opcodeInfo->layout.hasRegB) {
    printf("  regB=%-3s", getRegisterIdentifier(instruction.operands.registerA));
  }
  if (opcodeInfo->layout.hasRegC) {
    printf("  regC=%-3s", getRegisterIdentifier(instruction.operands.registerC));
  }

  uint8_t numImmABits = opcodeInfo->layout.numImmABits;
  if (numImmABits > 0) {
    if (numImmABits == 4) {
      printf("  immA=%01x", instruction.operands.immediateA.u16);
    } else if (numImmABits == 8) {
      printf("  immA=%02x", instruction.operands.immediateA.u16);
    } else if (numImmABits == 16) {
      printf("  immA=%04x", instruction.operands.immediateA.u16);
    }
    printf("(%hu)", instruction.operands.immediateA.u16);
  }

  if (opcodeInfo->layout.hasImmB) {
    printf("  immB=%04x(%hu)", instruction.operands.immediateB.u16, instruction.operands.immediateB.u16);
  }

  printf("\n");
}
