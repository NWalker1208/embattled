#include <stdio.h>
#include <windows.h>

#define MEM_SIZE 65536

#define IMM_SML_U(imm) (imm & 0b11111)
#define IMM_SML_S(imm) (imm & 0b10000 ? (~0b11111 | IMM_SML_U(imm)) : IMM_SML_U(imm))

#define IMM_MED_U(imm) (imm & 0b11111111)
#define IMM_MED_S(imm) (imm & 0b10000000 ? (~0b11111111 | IMM_MED_U(imm)) : IMM_MED_U(imm))

#define IMM_LRG_U(imm) (imm & 0b11111111111)
#define IMM_LRG_S(imm) (imm & 0b10000000000 ? (~0b11111111111 | IMM_LRG_U(imm)) : IMM_LRG_U(imm))

struct Registers {
  unsigned short ip; // Instruction pointer. Can't be referenced directly.

  unsigned short x0;
  unsigned short x1;
  unsigned short x2;
  unsigned short x3;
  unsigned short x4;
  unsigned short x5;
  unsigned short x6;
  unsigned short x7;
};

// Instructions (based on RV32C)

// Layout:
// A: | immediate (11 bits)                                                                 | opcode (5 bits) |
// B: | register A (3 bits) | immediate (8 bits)                                            | opcode (5 bits) |
// C: | register A (3 bits) | register B (3 bits) | immediate (5 bits)                      | opcode (5 bits) |
// D: | register A (3 bits) | register B (3 bits) | register C (3 bits) | function (2 bits) | opcode (5 bits) |

// Instructions:
// Instruction | Layout | Opcode | Function
// Control flow:
//   NOP       | Any    | 000 00 |
//   JMP       | A      | 000 01 |
//   JMZ       | B      | 000 10 |
// Memory:
//   LDIL      | B      | 001 00 |
//   LDIH      | B      | 001 01 |
//   LDML      | C      | 001 10 |
//   LDMH      | C      | 001 11 |
//   LDMW      | C      | 010 00 |
//   STML      | C      | 010 01 |
//   STMH      | C      | 010 10 |
//   STMW      | C      | 010 11 |
// Math:
//   ADD       | D      | 011 00 | 00
//   SUB       | D      | 011 00 | 01
//   MUL       | D      | 011 01 | 00
//   DIVS      | D      | 011 10 | 00  
//   DIVU      | D      | 011 10 | 01
//   REMS      | D      | 011 10 | 10
//   REMU      | D      | 011 10 | 11
// Bitwise:
//   AND       | D      | 100 00 | 00
//   IOR       | D      | 100 00 | 01
//   XOR       | D      | 100 00 | 10
//   LSH       | C      | 100 01 |
//   RSHS      | C      | 100 10 |
//   RSHU      | C      | 100 11 |
// Comparison:
//   CEQ       | D      | 101 00 | 00
//   CNE       | D      | 101 00 | 01
//   CLTS      | D      | 101 01 | 00
//   CLTU      | D      | 101 01 | 01
//   CGES      | D      | 101 01 | 10
//   CGEU      | D      | 101 01 | 11

enum RegisterCode {
  x0 = 0b000,
  x1 = 0b001,
  x2 = 0b010,
  x3 = 0b011,
  x4 = 0b100,
  x5 = 0b101,
  x6 = 0b110,
  x7 = 0x111
};

enum OpCode {
  // Control flow
  NOP     = 0b00000,
  JMP     = 0b00001,
  JMZ     = 0b00010,
  // Memory
  LDIL    = 0b00100,
  LDIH    = 0b00101,
  LDML    = 0b00110,
  LDMH    = 0b00111,
  LDMW    = 0b01000,
  STML    = 0b01001,
  STMH    = 0b01010,
  STMW    = 0b01011,
  // Math
  ADD_SUB = 0b01100,
  MUL     = 0b01101,
  DIV_REM = 0b01110,
  // Bitwise
  AND_OR  = 0b10000,
  LSH     = 0b10001,
  RSHS    = 0b10010,
  RSEU    = 0b10011,
  // Comparison
  CEQ_CNE = 0b10100,
  CLT_CGT = 0b10101,
};

void printRegisters(struct Registers* registers);
void runCpuCycle(char* mem, struct Registers* registers);
unsigned short* getRegisterAddr(enum RegisterCode code, struct Registers* registers);

int main() {
  printf("Hello, from Embattled!\n");

  char mem[MEM_SIZE] = { 0 };
  struct Registers registers = { 0 };

  printRegisters(&registers);
  while (1) {
    Sleep(1000); // TODO: Replace with portable code
    printf("\n");
    runCpuCycle(mem, &registers);
    printRegisters(&registers);
  }
}

void printRegisters(struct Registers* registers) {
  printf("ip = 0x%04x\n", registers->ip);
  printf("x0 = 0x%04x\n", registers->x0);
  printf("x1 = 0x%04x\n", registers->x1);
  printf("x2 = 0x%04x\n", registers->x2);
  printf("x3 = 0x%04x\n", registers->x3);
  printf("x4 = 0x%04x\n", registers->x4);
  printf("x5 = 0x%04x\n", registers->x5);
  printf("x6 = 0x%04x\n", registers->x6);
  printf("x7 = 0x%04x\n", registers->x7);
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

  unsigned short* regA = getRegisterAddr(regCodeA, registers);
  unsigned short* regB = getRegisterAddr(regCodeB, registers);
  unsigned short* regC = getRegisterAddr(regCodeC, registers);

  unsigned short addrLow, addrHigh;

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
      addrLow = (*regB + IMM_SML_S(imm));
      *regA = (*regA & (0xFF << 8)) | (unsigned short)mem[addrLow];
      break;
    case LDMH:
      addrLow = (*regB + IMM_SML_S(imm));
      *regA = ((unsigned short)mem[addrLow] << 8) | (*regA & 0xFF);
      break;
    case LDMW:
      addrLow = (*regB + IMM_SML_S(imm));
      addrHigh = addrLow + 1;
      *regA = ((unsigned short)mem[addrHigh] << 8) | (unsigned short)mem[addrLow];
      break;
    case STML:
      addrLow = (*regB + IMM_SML_S(imm));
      mem[addrLow] = (char)(*regA & 0xFF);
      break;
    case STMH:
      addrLow = (*regB + IMM_SML_S(imm));
      mem[addrLow] = (char)((*regA & 0xFF00) >> 8);
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
