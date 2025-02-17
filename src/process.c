#include "process.h"
#include "instruction.h"

#define IMM_SML_U(imm) (imm & 0b1'1111)
#define IMM_SML_S(imm) (imm & 0b1'0000 ? (~0b1'1111 | IMM_SML_U(imm)) : IMM_SML_U(imm))

#define IMM_MED_U(imm) (imm & 0b1111'1111)
#define IMM_MED_S(imm) (imm & 0b1000'0000 ? (~0b1111'1111 | IMM_MED_U(imm)) : IMM_MED_U(imm))

#define IMM_LRG_U(imm) (imm & 0b111'1111'1111)
#define IMM_LRG_S(imm) (imm & 0b100'0000'0000 ? (~0b111'1111'1111 | IMM_LRG_U(imm)) : IMM_LRG_U(imm))

struct Instruction fetchInstruction(unsigned char* memory, unsigned short* ip) {
  struct Instruction instruction = { 0 };
  instruction.opcode = decodeOpcode(memory[*ip]);
  (*ip)++;

  unsigned char addlBytes = instruction.opcode & 0b11;
  switch (addlBytes) {
    case 0:
      break;
    case 1:
      unsigned char params = memory[*ip];
      (*ip)++;
      instruction.registerA = decodeRegister(params >> 4);
      instruction.registerB = decodeRegister(params & 0b1111);
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

void printProcessState(struct ProcessState* state) {
  printf("ip = 0x%04x    sp = 0x%04x    ac = 0x%04x\n", state->ip, state->sp, state->ac);
  printf("x0 = 0x%04x    x1 = 0x%04x    x2 = 0x%04x    x3 = 0x%04x\n", state->x0, state->x1, state->x2, state->x3);
  printf("x4 = 0x%04x    x5 = 0x%04x    x6 = 0x%04x    x7 = 0x%04x\n", state->x4, state->x5, state->x6, state->x7);
  printf("x8 = 0x%04x    x9 = 0x%04x    x10 = 0x%04x   x11 = 0x%04x\n", state->x8, state->x9, state->x10, state->x11);
}
