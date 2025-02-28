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
  switch (numBytes) {
    case 1:
      if (!hasRegB) {
        unsigned short value = parameterBytes[0];
        if (!hasRegA) {
          instruction.parameters.immediate.u8 = value;
        } else {
          instruction.parameters.immediate.u4 = value;
        }
      }
      break;
    case 2:
      unsigned short value = ((unsigned short)parameterBytes[1] << 8) | (unsigned short)parameterBytes[0];
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
      unsigned short value = ((unsigned short)parameterBytes[1] << 8) | (unsigned short)parameterBytes[0];
      instruction.parameters.immediate.u16 = value;
      break;
  }

  return instruction;
}
