#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "processor/instruction.h"

uint16_t fetchInstruction(const uint8_t* memory, uint16_t addr, Instruction* instructionOut) {
  instruction->opcode = byteToOpcode(memory[addr]);
  addr++;

  const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[instruction->opcode];

  // Read the parameter bytes from memory.
  unsigned char numBytes = opcodeInfo->parameterLayout.numBytes;
  if (numBytes == 0) {
    return 1;
  }

  unsigned char parameterBytes[3];
  for (int i = 0; i < numBytes; i++) {
    parameterBytes[i] = memory[addr];
    addr++;
  }

  // Extract the register values from the parameter bytes.
  bool hasRegA = opcodeInfo->parameterLayout.hasRegA;
  bool hasRegB = opcodeInfo->parameterLayout.hasRegB;

  if (hasRegA) {
    instruction->parameters.registerA = nibbleToRegister(parameterBytes[numBytes - 1] >> 4);
  }

  if (hasRegB) {
    instruction->parameters.registerB = nibbleToRegister(parameterBytes[numBytes - 1] & 0xF);
  }

  // Extract the immediate value from the parameter bytes.
  unsigned short value;
  switch (numBytes) {
    case 1:
      if (!hasRegB) {
        value = parameterBytes[0];
        if (!hasRegA) {
          instruction->parameters.immediate.u8 = value;
        } else {
          instruction->parameters.immediate.u4 = value;
        }
      }
      break;
    case 2:
      value = ((unsigned short)parameterBytes[1] << 8) | (unsigned short)parameterBytes[0];
      if (!hasRegB) {
        if (!hasRegA) {
          instruction->parameters.immediate.u16 = value;
        } else {
          instruction->parameters.immediate.u12 = value;
        }
      } else {
        instruction->parameters.immediate.u8 = value;
      }
      break;
    case 3:
      value = ((unsigned short)parameterBytes[1] << 8) | (unsigned short)parameterBytes[0];
      instruction->parameters.immediate.u16 = value;
      break;
  }

  return 1 + numBytes;
}

uint16_t writeInstruction(uint8_t* memory, uint16_t addr, Instruction instruction) {
  // Write the opcode byte to memory, if it is valid
  enum Opcode opcode = instruction.opcode;
  if (opcode < 0 || opcode >= OPCODE_COUNT) {
    return 0;
  }

  memory[addr] = (unsigned char)opcode;
  addr++;
  
  const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[instruction.opcode];
  unsigned char numBytes = opcodeInfo->parameterLayout.numBytes;
  if (numBytes == 0) {
    return 1;
  }

  // Write the immediate value to memory
  switch (numBytes) {
    case 1:
      memory[addr] = (unsigned char)instruction.parameters.immediate.u8;
      break;
    case 2: 
    case 3:
      memory[addr] = (unsigned char)(instruction.parameters.immediate.u16 & 0xFF);
      memory[addr + 1] = (unsigned char)(instruction.parameters.immediate.u16 >> 8);
  }
  addr += numBytes - 1;

  // Write the registers to memory
  bool hasRegA = opcodeInfo->parameterLayout.hasRegA;
  bool hasRegB = opcodeInfo->parameterLayout.hasRegB;

  if (hasRegA) {
    memory[addr] = (((unsigned char)instruction.parameters.registerA << 4) & 0xF0) | (memory[addr] & 0x0F);
  }

  if (hasRegB) {
    memory[addr] = (memory[addr] & 0xF0) | ((unsigned char)instruction.parameters.registerB & 0x0F);
  }

  return 1 + numBytes;
}

void printInstruction(Instruction instruction) {
  if (instruction.opcode < 0 || instruction.opcode >= OPCODE_COUNT) {
    printf("opcode=INVALID\n");
    return;
  }

  const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[instruction.opcode];
  printf("opcode=%-4s", opcodeInfo->name);
  if (opcodeInfo->parameterLayout.hasRegA) {
    printf("  regA=%-3s", REGISTER_NAMES[instruction.parameters.registerA]);
  }
  if (opcodeInfo->parameterLayout.hasRegB) {
    printf("  regB=%-3s", REGISTER_NAMES[instruction.parameters.registerB]);
  }

  unsigned char numImmBits = opcodeInfo->parameterLayout.numImmBits;
  if (numImmBits > 0) {
    printf("  imm(raw)=%04x", instruction.parameters.immediate.u16);

    bool immIsSigned = opcodeInfo->parameterLayout.immIsSigned;
    if (!immIsSigned) {
      unsigned short value;
      if (numImmBits == 4) {
        value = instruction.parameters.immediate.u4;
      } else if (numImmBits == 8) {
        value = instruction.parameters.immediate.u8;
      } else if (numImmBits == 12) {
        value = instruction.parameters.immediate.u12;
      } else {
        value = instruction.parameters.immediate.u16;
      }
      printf("  imm=%hu", value);
    } else {
      signed short value;
      if (numImmBits == 4) {
        value = instruction.parameters.immediate.s4;
      } else if (numImmBits == 8) {
        value = instruction.parameters.immediate.s8;
      } else if (numImmBits == 12) {
        value = instruction.parameters.immediate.s12;
      } else {
        value = instruction.parameters.immediate.s16;
      }
      printf("  imm=%hd", value);
    }
  }

  printf("\n");
}
