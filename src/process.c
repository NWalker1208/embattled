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

unsigned short* getRegisterAddr(struct ProcessState* processState, enum Register reg) {
  switch (reg) {
    case NL:
      return 0;
    case IP:
      return &processState->ip;
    case SP:
      return &processState->sp;
    case AC:
      return &processState->ac;
    case X0:
      return &processState->x0;
    case X1:
      return &processState->x1;
    case X2:
      return &processState->x2;
    case X3:
      return &processState->x3;
    case X4:
      return &processState->x4;
    case X5:
      return &processState->x5;
    case X6:
      return &processState->x6;
    case X7:
      return &processState->x7;
    case X8:
      return &processState->x8;
    case X9:
      return &processState->x9;
    case X10:
      return &processState->x10;
    case X11:
      return &processState->x11;
    default:
      return 0;
  }
}

void stepProcess(unsigned char* memory, struct ProcessState* processState) {
  struct Instruction instr = fetchInstruction(memory, &(processState->ip));
}
