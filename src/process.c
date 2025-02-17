#include "process.h"

#include <stdio.h>
#include "instruction.h"

const unsigned short SMALL_IMM_MASK = 0xF;
const unsigned short MEDIUM_IMM_MASK = 0xFF;

struct Instruction fetchInstruction(unsigned char* memory, unsigned short* ip) {
  struct Instruction instruction = { 0 };
  unsigned char opcodeValue = memory[*ip];
  instruction.opcode = decodeOpcode(opcodeValue);
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

unsigned short* getRegisterAddr(struct ProcessState* processState, unsigned short* nullRegister, enum Register reg) {
  switch (reg) {
    case NL:
      return nullRegister;
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
      return nullRegister;
  }
}

void stepProcess(unsigned char* memory, struct ProcessState* processState) {
  unsigned short ip = processState->ip;
  struct Instruction instr = fetchInstruction(memory, &ip);

  unsigned short nullRegister = 0;
  enum Opcode opcode = instr.opcode;
  unsigned short* regA = getRegisterAddr(processState, &nullRegister, instr.registerA);
  unsigned short* regB = getRegisterAddr(processState, &nullRegister, instr.registerB);
  unsigned short imm = instr.immediateValue;

  printf("opcode=%-4s  regA=%-3s  regB=%-3s  imm=0x%04x\n", OPCODE_NAMES[opcode], REGISTER_NAMES[instr.registerA], REGISTER_NAMES[instr.registerB], imm);

  unsigned short addrLow, addrHigh;

  switch (opcode) {
    // Control flow
    case NOP:
      break;
    case JMP:
      ip = imm;
      break;
    case JMZ:
      if (processState->ac == 0) {
        ip = imm;
      }
      break;
    // Memory
    case MOV:
      *regA = *regB;
      break;
    case LDIB:
      processState->ac = imm & MEDIUM_IMM_MASK;
      break;
    case LDIW:
      processState->ac = imm;
      break;
    case LDMB:
      *regA = (unsigned short)memory[*regB];
      break;
    case LDMW:
      addrLow = *regB;
      addrHigh = addrLow + 1;
      *regA = ((unsigned short)memory[addrHigh] << 8) | (unsigned short)memory[addrLow];
      break;
    case STMB:
      memory[*regB] = (char)(*regA & 0xFF);
      break;
    case STMW:
      addrLow = *regB;
      addrHigh = addrLow + 1;
      memory[addrLow] = (char)(*regA & 0xFF);
      memory[addrHigh] = (char)((*regA & 0xFF00) >> 8);
      break;
    case PSHB:
      processState->sp--;
      memory[processState->sp] = (char)(*regA & 0xFF);
      break;
    case PSHW:
      processState->sp -= 2;
      addrLow = processState->sp;
      addrHigh = addrLow + 1;
      memory[addrLow] = (char)(*regA & 0xFF);
      memory[addrHigh] = (char)((*regA & 0xFF00) >> 8);
      break;
    case POPB:
      *regA = (unsigned short)memory[processState->sp];
      processState->sp++;
      break;
    case POPW:
      addrLow = processState->sp;
      addrHigh = addrLow + 1;
      *regA = ((unsigned short)memory[addrHigh] << 8) | (unsigned short)memory[addrLow];
      processState->sp += 2;
      break;
    // Math and logic
    case ADD:
      *regA = *regA + *regB;
      break;
    case SUB:
      *regA = *regA - *regB;
      break;
    case MUL:
      *regA = *regA * *regB;
      break;
    case DIVS:
      *regA = ((signed short)*regA) / ((signed short)*regB);
      break;
    case DIVU:
      *regA = ((unsigned short)*regA) / ((unsigned short)*regB);
      break;
    case REMS:
      *regA = ((signed short)*regA) % ((signed short)*regB);
      break;
    case REMU:
      *regA = ((unsigned short)*regA) % ((unsigned short)*regB);
      break;
    case LSH:
      *regA = *regA << *regB;
      break;
    case RSHS:
      *regA = ((signed short)*regA) >> *regB;
      break;
    case RSHU:
      *regA = ((unsigned short)*regA) >> *regB;
      break;
    case LSI:
      *regA = *regA << (imm & SMALL_IMM_MASK);
      break;
    case RSIS:
      *regA = ((signed short)*regA) >> (imm & SMALL_IMM_MASK);
      break;
    case RSIU:
      *regA = ((unsigned short)*regA) >> (imm & SMALL_IMM_MASK);
      break;
    case AND:
      *regA = *regA & *regB;
      break;
    case IOR:
      *regA = *regA | *regB;
      break;
    case XOR:
      *regA = *regA ^ *regB;
      break;
    case CEQ:
      *regA = (*regA == *regB) ? 1 : 0;
      break;
    case CNE:
      *regA = (*regA != *regB) ? 1 : 0;
      break;
    case CLTS:
      *regA = (((signed short)*regA) < ((signed short)*regB)) ? 1 : 0;
      break;
    case CLTU:
      *regA = (((unsigned short)*regA) < ((unsigned short)*regB)) ? 1 : 0;
      break;
    case CGES:
      *regA = (((signed short)*regA) >= ((signed short)*regB)) ? 1 : 0;
      break;
    case CGEU:
      *regA = (((unsigned short)*regA) >= ((unsigned short)*regB)) ? 1 : 0;
      break;
    default: // Invalid instruction
      break;
  }

  processState->ip = ip;
}

void printProcessState(struct ProcessState* state) {
  printf("ip = 0x%04x    sp = 0x%04x    ac = 0x%04x\n", state->ip, state->sp, state->ac);
  printf("x0 = 0x%04x    x1 = 0x%04x    x2 = 0x%04x    x3 = 0x%04x\n", state->x0, state->x1, state->x2, state->x3);
  printf("x4 = 0x%04x    x5 = 0x%04x    x6 = 0x%04x    x7 = 0x%04x\n", state->x4, state->x5, state->x6, state->x7);
  printf("x8 = 0x%04x    x9 = 0x%04x    x10 = 0x%04x   x11 = 0x%04x\n", state->x8, state->x9, state->x10, state->x11);
}
