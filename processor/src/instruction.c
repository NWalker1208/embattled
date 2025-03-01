#include <string.h>
#include <stdbool.h>
#include "processor/instruction.h"

struct Instruction fetchInstruction(unsigned char* memory, unsigned short* ip) {
  struct Instruction instruction = { 0 };
  instruction.opcode = byteToOpcode(memory[*ip]);

  const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[instruction.opcode];
  (*ip)++;

  // Read the parameter bytes from memory.
  unsigned char numBytes = opcodeInfo->parameterLayout & SIZE_MASK;
  if (numBytes == 0) {
    return instruction;
  }

  unsigned char parameterBytes[3];
  memcpy(parameterBytes, &memory[*ip], numBytes);
  *ip += numBytes;

  // Extract the register values from the parameter bytes.
  bool hasRegA = opcodeInfo->parameterLayout & REGA_FLAG;
  bool hasRegB = opcodeInfo->parameterLayout & REGB_FLAG;

  if (hasRegA) {
    instruction.parameters.registerA = nibbleToRegister(parameterBytes[numBytes - 1] >> 4);
  }

  if (hasRegB) {
    instruction.parameters.registerB = nibbleToRegister(parameterBytes[numBytes - 1] & 0xF);
  }

  // Extract the immediate value from the parameter bytes.
  unsigned short value;
  switch (numBytes) {
    case 1:
      if (!hasRegB) {
        value = parameterBytes[0];
        if (!hasRegA) {
          instruction.parameters.immediate.u8 = value;
        } else {
          instruction.parameters.immediate.u4 = value;
        }
      }
      break;
    case 2:
      value = ((unsigned short)parameterBytes[1] << 8) | (unsigned short)parameterBytes[0];
      if (!hasRegB) {
        if (!hasRegA) {
          instruction.parameters.immediate.u16 = value;
        } else {
          instruction.parameters.immediate.u12 = value;
        }
      } else {
        instruction.parameters.immediate.u8 = value;
      }
      break;
    case 3:
      value = ((unsigned short)parameterBytes[1] << 8) | (unsigned short)parameterBytes[0];
      instruction.parameters.immediate.u16 = value;
      break;
  }

  return instruction;
}

int storeInstruction(unsigned char* memory, unsigned short addr, struct Instruction instruction) {
  // Write the opcode byte to memory, if it is valid
  enum Opcode opcode = instruction.opcode;
  if (opcode < 0 || opcode >= OPCODE_COUNT) {
    return 0;
  }

  memory[addr] = (unsigned char)opcode;
  addr++;
  
  const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[instruction.opcode];
  unsigned char numBytes = opcodeInfo->parameterLayout & SIZE_MASK;
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
  bool hasRegA = opcodeInfo->parameterLayout & REGA_FLAG;
  bool hasRegB = opcodeInfo->parameterLayout & REGB_FLAG;

  if (hasRegA) {
    memory[addr] = (((unsigned char)instruction.parameters.registerA << 4) & 0xF0) | (memory[addr] & 0x0F);
  }

  if (hasRegB) {
    memory[addr] = (memory[addr] & 0xF0) | ((unsigned char)instruction.parameters.registerB & 0x0F);
  }

  return 1 + numBytes;
}
