#include <stdio.h>

#define MEM_SIZE 65536

#define IMM_SML 0b11111
#define IMM_MED 0b11111111
#define IMM_LRG 0b11111111111

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

void runCpuCycle(char* mem, struct Registers* registers);
unsigned short* getRegisterAddr(enum RegisterCode code, struct Registers* registers);

int main() {
  printf("Hello, from Embattled!\n");

  char mem[MEM_SIZE] = { 0 };
  struct Registers registers = { 0 };

  while (1) {
    runCpuCycle(mem, &registers);
  }
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

  switch (opCode) {
  // Control flow
    case JMP:
      break;
    case JMZ:
      break;
  // Memory
    case LDIL:
      break;
    case LDIH:
      break;
    case LDML:
      break;
    case LDMH:
      break;
    case LDMW:
      break;
    case STML:
      break;
    case STMH:
      break;
    case STMW:
      break;
  // Math
    case ADD_SUB:
      switch (func) {
        case 0b00: // ADD
          break;
        case 0b01: // SUB
          break;
        default: // Invalid instruction
          break;
      }
      break;
    case MUL:
      break;
    case DIV_REM:
      switch (func) {
        case 0b00: // DIVS
          break;
        case 0b01: // DIVU
          break;
        case 0b10: // REMS
          break;
        case 0b11: // REMU
          break;
      }
      break;
  // Bitwise
    case AND_OR:
      switch (func) {
        case 0b00: // AND
          break;
        case 0b01: // IOR
          break;
        case 0b10: // XOR
          break;
        default: // Invalid instruction
          break;
      }
      break;
    case LSH:
      break;
    case RSHS:
      break;
    case RSEU:
      break;
  // Comparison
    case CEQ_CNE:
      switch (func) {
        case 0b00: // CEQ
          break;
        case 0b01: // CNE
          break;
        default: // Invalid instruction
          break;
      }
      break;
    case CLT_CGT:
      switch (func) {
        case 0b00: // CLTS
          break;
        case 0b01: // CLTU
          break;
        case 0b10: // CGES
          break;
        case 0b11: // CGEU
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
