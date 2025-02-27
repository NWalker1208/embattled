#include "processor/instruction.h"

struct Instruction fetchInstruction(unsigned char* memory, unsigned short* ip) {
  struct Instruction instruction = { 0 };
  unsigned char opcodeValue = memory[*ip];
  instruction.opcode = decodeOpcode(opcodeValue);
  struct OpcodeInfo opcodeInfo = OPCODE_INFO[instruction.opcode]; // Next: Fix this method to fetch based on parameter layout
  (*ip)++;

  switch (opcodeValue & 0b11) {
    case 0b00:
      break;
    case 0b01:
      unsigned char params = memory[*ip];
      (*ip)++;
      instruction.registerA = decodeRegister(params >> 4);
      instruction.registerB = decodeRegister(params & 0b1111);
      instruction.immediateValue = params; // Instructions that only use 4 bits will mask out upper bits.
      break;
    case 0b10:
      unsigned char immediateLower = memory[*ip];
      (*ip)++;
      unsigned char immediateUpper = memory[*ip];
      (*ip)++;
      instruction.immediateValue = ((unsigned short)immediateUpper << 8) | (unsigned short)immediateLower;
      break;
  }

  return instruction;
}
