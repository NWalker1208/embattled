#include <stdio.h>
#include "processor/process.h"
#include "processor/instruction.h"

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

void stepProcess(struct ProcessState* state) {
  unsigned short ip = state->registers.ip;
  struct Instruction instr = fetchInstruction(state->memory, &ip);

  unsigned short nullRegister = 0;
  enum Opcode opcode = instr.opcode;
  unsigned short* regA = getRegisterPtr(&state->registers, instr.registerA);
  if (regA == NULL) { regA = &nullRegister; }
  unsigned short* regB = getRegisterPtr(&state->registers, instr.registerB);
  if (regB == NULL) { regB = &nullRegister; }
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
      if (state->registers.ac == 0) {
        ip = imm;
      }
      break;
    // Memory
    case MOV:
      *regA = *regB;
      break;
    case LDIB:
      state->registers.ac = imm & MEDIUM_IMM_MASK;
      break;
    case LDIW:
    state->registers.ac = imm;
      break;
    case LDMB:
      *regA = (unsigned short)state->memory[*regB];
      break;
    case LDMW:
      addrLow = *regB;
      addrHigh = addrLow + 1;
      *regA = ((unsigned short)state->memory[addrHigh] << 8) | (unsigned short)state->memory[addrLow];
      break;
    case STMB:
      state->memory[*regB] = (char)(*regA & 0xFF);
      break;
    case STMW:
      addrLow = *regB;
      addrHigh = addrLow + 1;
      state->memory[addrLow] = (char)(*regA & 0xFF);
      state->memory[addrHigh] = (char)((*regA & 0xFF00) >> 8);
      break;
    case PSHB:
      state->registers.sp--;
      state->memory[state->registers.sp] = (char)(*regA & 0xFF);
      break;
    case PSHW:
      state->registers.sp -= 2;
      addrLow = state->registers.sp;
      addrHigh = addrLow + 1;
      state->memory[addrLow] = (char)(*regA & 0xFF);
      state->memory[addrHigh] = (char)((*regA & 0xFF00) >> 8);
      break;
    case POPB:
      *regA = (unsigned short)state->memory[state->registers.sp];
      state->registers.sp++;
      break;
    case POPW:
      addrLow = state->registers.sp;
      addrHigh = addrLow + 1;
      *regA = ((unsigned short)state->memory[addrHigh] << 8) | (unsigned short)state->memory[addrLow];
      state->registers.sp += 2;
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

  state->registers.ip = ip;
}
