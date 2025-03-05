#include <stdio.h>
#include "processor/process.h"
#include "processor/instruction.h"

void stepProcess(struct ProcessState* state) {
  struct Instruction instr = fetchInstruction(state->memory, &state->registers.ip);

  unsigned short nullRegister = 0;
  enum Opcode opcode = instr.opcode;
  unsigned short* regA = getRegisterPtr(&state->registers, instr.parameters.registerA);
  if (regA == NULL) { regA = &nullRegister; }
  unsigned short* regB = getRegisterPtr(&state->registers, instr.parameters.registerB);
  if (regB == NULL) { regB = &nullRegister; }
  union ImmediateValue imm = instr.parameters.immediate;

  printf("opcode=%-4s  regA=%-3s  regB=%-3s  imm=0x%04x\n",
    OPCODE_INFO[opcode].name,
    REGISTER_NAMES[instr.parameters.registerA],
    REGISTER_NAMES[instr.parameters.registerB],
    imm.u16);

  unsigned short addrLow, addrHigh;

  switch (opcode) {
    // Control flow
    case NOP:
      break;
    case JMP:
      state->registers.ac = state->registers.ip;
      state->registers.ip = imm.u16;
      break;
    case JMZ:
      if (state->registers.ac == 0) {
        state->registers.ip = imm.u16;
      }
      break;
    // Memory
    case MOV:
      *regA = *regB;
      break;
    case LDIB:
      state->registers.ac = imm.u8;
      break;
    case LDIW:
      state->registers.ac = imm.u16;
      break;
    case LDMB:
      addrLow = *regA + imm.s4;
      state->registers.ac = (unsigned short)state->memory[addrLow];
      break;
    case LDMW:
      addrLow = *regA + imm.s4;
      addrHigh = addrLow + 1;
      state->registers.ac = ((unsigned short)state->memory[addrHigh] << 8) | (unsigned short)state->memory[addrLow];
      break;
    case STMB:
      addrLow = *regA + imm.s4;
      state->memory[addrLow] = (unsigned char)(state->registers.ac & 0x00FF);
      break;
    case STMW:
      addrLow = *regA + imm.s4;
      addrHigh = addrLow + 1;
      state->memory[addrLow] = (unsigned char)(state->registers.ac & 0x00FF);
      state->memory[addrHigh] = (unsigned char)((state->registers.ac & 0xFF00) >> 8);
      break;
    case PSHB:
      addrLow = state->registers.sp - 1;
      state->memory[addrLow] = (unsigned char)(*regA & 0x00FF);
      state->registers.sp = addrLow;
      break;
    case PSHW:
      addrLow = state->registers.sp - 2;
      addrHigh = addrLow + 1;
      state->memory[addrLow] = (unsigned char)(*regA & 0x00FF);
      state->memory[addrHigh] = (unsigned char)((*regA & 0xFF00) >> 8);
      state->registers.sp = addrLow;
      break;
    case POPB:
      addrLow = state->registers.sp;
      state->registers.sp++;
      *regA = (unsigned short)state->memory[addrLow];
      break;
    case POPW:
      addrLow = state->registers.sp;
      addrHigh = addrLow + 1;
      state->registers.sp += 2;
      *regA = ((unsigned short)state->memory[addrHigh] << 8) | (unsigned short)state->memory[addrLow];
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
    case DIVS: // TODO: Handle division by zero
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
      *regA = *regA << imm.u4;
      break;
    case RSIS:
      *regA = ((signed short)*regA) >> imm.u4;
      break;
    case RSIU:
      *regA = ((unsigned short)*regA) >> imm.u4;
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
}
