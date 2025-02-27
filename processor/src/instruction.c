#include "processor/instruction.h"

struct Instruction fetchInstruction(unsigned char* memory, unsigned short* ip) {
  struct Instruction instruction = { 0 };
  instruction.opcode = byteToOpcode(memory[*ip]);

  const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[instruction.opcode];
  (*ip)++;

  // Read parameter bytes and advance instruction pointer
  unsigned char paramsLow;
  unsigned char paramsHigh;
  switch (opcodeInfo->parameterLayout) {
    case REGA:
    case REGA_REGB:
    case REGA_IMM4:
    case IMM8:
      paramsLow = memory[*ip];
      paramsHigh = 0;
      (*ip)++;
      break;
      
    case REGA_IMM12:
    case IMM16:
      paramsLow = memory[*ip];
      paramsHigh = memory[*ip + 1];
      (*ip) += 2;
      break;
  }

  // Decode register parameters
  switch (opcodeInfo->parameterLayout) {
    case REGA_REGB:
      instruction.parameters.registerB = nibbleToRegister(paramsLow & 0xF);
    case REGA:
    case REGA_IMM4:
    case REGA_IMM12:
      instruction.parameters.registerA = nibbleToRegister(paramsLow >> 4);
      break;
  }

  // Decode immediate value parameter
  switch (opcodeInfo->parameterLayout) {
    case IMM8:
      instruction.parameters.immediateValue = paramsLow;
    case REGA_IMM4:
      instruction.parameters.immediateValue &= 0xF;
      break;
    
    case IMM16:
      instruction.parameters.immediateValue = ((unsigned short)paramsHigh << 8) | (unsigned short)paramsLow;
    case REGA_IMM12:
      instruction.parameters.immediateValue &= 0xFFF;
      break;
  }

  return instruction;
}
