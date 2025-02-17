#include "process.h"
#include "instruction.h"

struct Instruction fetchInstruction(unsigned char* memory, unsigned short* ip) {
  struct Instruction instruction = { 0 };
  instruction.opcode = memory[*ip];
  (*ip)++;

  unsigned char addlBytes = instruction.opcode & 0b11;
  switch (addlBytes) {
    case 0:
      break;
    case 1:
      unsigned char params = memory[*ip];
      (*ip)++;
      instruction.registerA = params >> 4;
      instruction.registerB = params & 0b1111;
      instruction.immediateValue = params; // Instructions that only use 4 bits will mask out upper bits.
      break;
    case 2:
      unsigned char immediateLower = memory[*ip];
      (*ip)++;
      unsigned char immediateUpper = memory[*ip];
      (*ip)++;
      instruction.immediateValue = ((unsigned short)immediateUpper << 8) | (unsigned short)immediateLower;
      break;
  }

  return instruction;
}

void stepProcess(unsigned char* memory, struct ProcessState* processState) {
  struct Instruction instr = fetchInstruction(memory, &(processState->ip));
}
