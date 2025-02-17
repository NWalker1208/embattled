#include <stdio.h>
#include <windows.h>

int main() {
  printf("Hello, from Embattled!\n");
}

void runCpuCycle(char* mem, struct Registers* registers) {
  unsigned short ip = registers->ip;
  char* next = &(mem[ip]);
  ip += 2;

  enum OpCode opCode = next[0] & 0b11111;
  unsigned short imm = (next[0] >> 5) | (next[1] << 3);
  enum RegisterCode regCodeA = (imm >> 8) & 0b111;
  enum RegisterCode regCodeB = (imm >> 5) & 0b111;
  enum RegisterCode regCodeC = (imm >> 2) & 0b111;
  char func = imm & 0b11;

  // TODO: Add print out of current instruction.
  unsigned short* regA = getRegisterAddr(regCodeA, registers);
  unsigned short* regB = getRegisterAddr(regCodeB, registers);
  unsigned short* regC = getRegisterAddr(regCodeC, registers);

  unsigned short addr, addrLow, addrHigh;

  switch (opCode) {
  // Control flow
    case JMP:
      ip += IMM_LRG_S(imm);
      break;
    case JMZ:
      if (*regA == 0) {
        ip += IMM_MED_S(imm);
      }
      break;
  // Memory
    case LDIL:
      *regA = (*regA & (0xFF << 8)) | IMM_MED_U(imm);
      break;
    case LDIH:
      *regA = (IMM_MED_U(imm) << 8) | (*regA & 0xFF);
      break;
    case LDML:
      addr = (*regB + IMM_SML_S(imm));
      *regA = (*regA & (0xFF << 8)) | (unsigned short)mem[addr];
      break;
    case LDMH:
      addr = (*regB + IMM_SML_S(imm));
      *regA = ((unsigned short)mem[addr] << 8) | (*regA & 0xFF);
      break;
    case LDMW:
      addrLow = (*regB + IMM_SML_S(imm));
      addrHigh = addrLow + 1;
      *regA = ((unsigned short)mem[addrHigh] << 8) | (unsigned short)mem[addrLow];
      break;
    case STML:
      addr = (*regB + IMM_SML_S(imm));
      mem[addr] = (char)(*regA & 0xFF);
      break;
    case STMH:
      addr = (*regB + IMM_SML_S(imm));
      mem[addr] = (char)((*regA & 0xFF00) >> 8);
      break;
    case STMW:
      addrLow = (*regB + IMM_SML_S(imm));
      addrHigh = addrLow + 1;
      mem[addrLow] = (char)(*regA & 0xFF);
      mem[addrHigh] = (char)((*regA & 0xFF00) >> 8);
      break;
  // Math
    case ADD_SUB:
      switch (func) {
        case 0b00: // ADD
          *regA = *regB + *regC;
          break;
        case 0b01: // SUB
          *regA = *regB - *regC;
          break;
        default: // Invalid instruction
          break;
      }
      break;
    case MUL:
      *regA = *regB * *regC;
      break;
    case DIV_REM:
      switch (func) {
        case 0b00: // DIVS
          *regA = ((signed short)*regB) / ((signed short)*regC);
          break;
        case 0b01: // DIVU
          *regA = ((unsigned short)*regB) / ((unsigned short)*regC);
          break;
        case 0b10: // REMS
          *regA = ((signed short)*regB) % ((signed short)*regC);
          break;
        case 0b11: // REMU
          *regA = ((unsigned short)*regB) % ((unsigned short)*regC);
          break;
      }
      break;
  // Bitwise
    case AND_OR:
      switch (func) {
        case 0b00: // AND
          *regA = *regB & *regC;
          break;
        case 0b01: // IOR
          *regA = *regB | *regC;
          break;
        case 0b10: // XOR
          *regA = *regB ^ *regC;
          break;
        default: // Invalid instruction
          break;
      }
      break;
    case LSH:
      *regA = *regB << IMM_SML_U(imm);
      break;
    case RSHS:
      *regA = ((signed short)*regB) >> IMM_SML_U(imm);
      break;
    case RSEU:
      *regA = ((unsigned short)*regB) >> IMM_SML_U(imm);
      break;
  // Comparison
    case CEQ_CNE:
      switch (func) {
        case 0b00: // CEQ
          *regA = (*regB == *regC) ? 1 : 0;
          break;
        case 0b01: // CNE
          *regA = (*regB != *regC) ? 1 : 0;
          break;
        default: // Invalid instruction
          break;
      }
      break;
    case CLT_CGT:
      switch (func) {
        case 0b00: // CLTS
          *regA = (((signed short)*regB) < ((signed short)*regC)) ? 1 : 0;
          break;
        case 0b01: // CLTU
          *regA = (((unsigned short)*regB) < ((unsigned short)*regC)) ? 1 : 0;
          break;
        case 0b10: // CGES
          *regA = (((signed short)*regB) >= ((signed short)*regC)) ? 1 : 0;
          break;
        case 0b11: // CGEU
          *regA = (((unsigned short)*regB) >= ((unsigned short)*regC)) ? 1 : 0;
          break;
      }
      break;
    default: // NOP or invalid instruction
      break;
  }

  registers->ip = ip;
}

unsigned short* getRegisterAddr(enum RegisterCode code, struct Registers* registers) {
  switch (code) {
    case x0: return &registers->x0;
    case x1: return &registers->x1;
    case x2: return &registers->x2;
    case x3: return &registers->x3;
    case x4: return &registers->x4;
    case x5: return &registers->x5;
    case x6: return &registers->x6;
    case x7: return &registers->x7;
    default: return NULL;
  }
}
