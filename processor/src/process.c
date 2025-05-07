#include <stdlib.h>
#include "processor/process.h"
#include "processor/instruction.h"

void stepProcess(ProcessState* state) {
  Instruction instruction = { 0 };
  state->registers.ip += fetchInstruction(state->memory, state->registers.ip, &instruction);

  const OpcodeInfo* opcodeInfo = getOpcodeInfo(instruction.opcode);
  if (opcodeInfo == NULL) {
    return;
  }

  opcodeInfo->execute(state, instruction.operands);

  // uint16_t nullRegister = 0;
  // Opcode opcode = instr.opcode;
  // uint16_t* regA = getRegisterPtr(&state->registers, instr.operands.registerA);
  // if (regA == NULL) { regA = &nullRegister; }
  // uint16_t* regB = getRegisterPtr(&state->registers, instr.parameters.registerB);
  // if (regB == NULL) { regB = &nullRegister; }
  // union ImmediateValue imm = instr.parameters.immediate;

  // unsigned short addrLow, addrHigh;

  // switch (opcode) {
  //   // Control flow
  //   case NOP:
  //     break;
  //   case JMP:
  //     state->registers.ac = state->registers.ip;
  //     state->registers.ip = imm.u16;
  //     break;
  //   case JMZ:
  //     if (state->registers.ac == 0) {
  //       state->registers.ip = imm.u16;
  //     }
  //     break;
  //   // Memory
  //   case MOV:
  //     *regA = *regB;
  //     break;
  //   case LDIB:
  //     state->registers.ac = imm.u8;
  //     break;
  //   case LDIW:
  //     state->registers.ac = imm.u16;
  //     break;
  //   case LDMB:
  //     addrLow = *regA + imm.s4;
  //     state->registers.ac = (unsigned short)state->memory[addrLow];
  //     break;
  //   case LDMW:
  //     addrLow = *regA + imm.s4;
  //     addrHigh = addrLow + 1;
  //     state->registers.ac = ((unsigned short)state->memory[addrHigh] << 8) | (unsigned short)state->memory[addrLow];
  //     break;
  //   case STMB:
  //     addrLow = *regA + imm.s4;
  //     state->memory[addrLow] = (unsigned char)(state->registers.ac & 0x00FF);
  //     break;
  //   case STMW:
  //     addrLow = *regA + imm.s4;
  //     addrHigh = addrLow + 1;
  //     state->memory[addrLow] = (unsigned char)(state->registers.ac & 0x00FF);
  //     state->memory[addrHigh] = (unsigned char)((state->registers.ac & 0xFF00) >> 8);
  //     break;
  //   case PSHB:
  //     addrLow = state->registers.sp - 1;
  //     state->memory[addrLow] = (unsigned char)(*regA & 0x00FF);
  //     state->registers.sp = addrLow;
  //     break;
  //   case PSHW:
  //     addrLow = state->registers.sp - 2;
  //     addrHigh = addrLow + 1;
  //     state->memory[addrLow] = (unsigned char)(*regA & 0x00FF);
  //     state->memory[addrHigh] = (unsigned char)((*regA & 0xFF00) >> 8);
  //     state->registers.sp = addrLow;
  //     break;
  //   case POPB:
  //     addrLow = state->registers.sp;
  //     state->registers.sp++;
  //     *regA = (unsigned short)state->memory[addrLow];
  //     break;
  //   case POPW:
  //     addrLow = state->registers.sp;
  //     addrHigh = addrLow + 1;
  //     state->registers.sp += 2;
  //     *regA = ((unsigned short)state->memory[addrHigh] << 8) | (unsigned short)state->memory[addrLow];
  //     break;
  //   // Math and logic
  //   case INC:
  //     *regA += imm.u4;
  //     break;
  //   case DEC:
  //     *regA -= imm.u4;
  //     break;
  //   case ADD:
  //     state->registers.ac = *regA + *regB;
  //     break;
  //   case SUB:
  //     state->registers.ac = *regA - *regB;
  //     break;
  //   case MUL:
  //     state->registers.ac = *regA * *regB;
  //     break;
  //   case DIVS: // TODO: Handle division by zero
  //     state->registers.ac = ((signed short)*regA) / ((signed short)*regB);
  //     break;
  //   case DIVU:
  //     state->registers.ac = ((unsigned short)*regA) / ((unsigned short)*regB);
  //     break;
  //   case REMS:
  //     state->registers.ac = ((signed short)*regA) % ((signed short)*regB);
  //     break;
  //   case REMU:
  //     state->registers.ac = ((unsigned short)*regA) % ((unsigned short)*regB);
  //     break;
  //   case LSH:
  //     state->registers.ac = *regA << *regB;
  //     break;
  //   case RSHS:
  //     state->registers.ac = ((signed short)*regA) >> *regB;
  //     break;
  //   case RSHU:
  //     state->registers.ac = ((unsigned short)*regA) >> *regB;
  //     break;
  //   case LSI:
  //     state->registers.ac = *regA << imm.u4;
  //     break;
  //   case RSIS:
  //     state->registers.ac = ((signed short)*regA) >> imm.u4;
  //     break;
  //   case RSIU:
  //     state->registers.ac = ((unsigned short)*regA) >> imm.u4;
  //     break;
  //   case AND:
  //     state->registers.ac = *regA & *regB;
  //     break;
  //   case IOR:
  //     state->registers.ac = *regA | *regB;
  //     break;
  //   case XOR:
  //     state->registers.ac = *regA ^ *regB;
  //     break;
  //   case CEQ:
  //     state->registers.ac = (*regA == *regB) ? 1 : 0;
  //     break;
  //   case CNE:
  //     state->registers.ac = (*regA != *regB) ? 1 : 0;
  //     break;
  //   case CLTS:
  //     state->registers.ac = (((signed short)*regA) < ((signed short)*regB)) ? 1 : 0;
  //     break;
  //   case CLTU:
  //     state->registers.ac = (((unsigned short)*regA) < ((unsigned short)*regB)) ? 1 : 0;
  //     break;
  //   case CGES:
  //     state->registers.ac = (((signed short)*regA) >= ((signed short)*regB)) ? 1 : 0;
  //     break;
  //   case CGEU:
  //     state->registers.ac = (((unsigned short)*regA) >= ((unsigned short)*regB)) ? 1 : 0;
  //     break;
  //   default: // Invalid instruction
  //     break;
  // }
}
