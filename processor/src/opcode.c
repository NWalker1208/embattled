#include "processor/opcode.h"

const unsigned int OPCODE_COUNT = 36;

const unsigned char OPCODE_VALUES[] = {
  // Control flow
  0b000000'00, // NOP
  0b000001'10, // JMP
  0b000010'10, // JMZ
  0b000011'01, // SLP
  // Memory
  0b000100'01, // MOV
  0b000110'01, // LDIB
  0b000111'10, // LDIW
  0b001000'01, // LDMB
  0b001001'01, // LDMW
  0b001010'01, // STMB
  0b001011'01, // STMW
  0b001100'01, // PSHB
  0b001101'01, // PSHW
  0b001110'01, // POPB
  0b001111'01, // POPW
  // Math and logic
  0b010000'01, // INC
  0b010001'01, // DEC
  0b010010'01, // ADD
  0b010011'01, // SUB
  0b010100'01, // MUL
  0b011000'01, // DIVS
  0b011001'01, // DIVU
  0b011010'01, // REMS
  0b011011'01, // REMU
  0b011100'01, // LSH
  0b011101'01, // RSHS
  0b011110'01, // RSHU
  0b011111'01, // LSI
  0b100000'01, // RSIS
  0b100001'01, // RSIU
  0b100010'01, // AND
  0b100011'01, // IOR
  0b100100'01, // XOR
  0b100101'01, // CEQ
  0b100110'01, // CNE
  0b100111'01, // CLTS
  0b101000'01, // CLTU
  0b101001'01, // CGES
  0b101010'01, // CGEU
};

const char* OPCODE_NAMES[] = {
  // Control flow
  "NOP",
  "JMP",
  "JMZ",
  "SLP",
  // Memory
  "MOV",
  "LDIB",
  "LDIW",
  "LDMB",
  "LDMW",
  "STMB",
  "STMW",
  "PSHB",
  "PSHW",
  "POPB",
  "POPW",
  // Math and logic
  "INC",
  "DEC",
  "ADD",
  "SUB",
  "MUL",
  "DIVS",
  "DIVU",
  "REMS",
  "REMU",
  "LSH",
  "RSHS",
  "RSHU",
  "LSI",
  "RSIS",
  "RSIU",
  "AND",
  "IOR",
  "XOR",
  "CEQ",
  "CNE",
  "CLTS",
  "CLTU",
  "CGES",
  "CGEU",
};

enum Opcode decodeOpcode(unsigned char opcodeValue) {
  for (unsigned int i = 0; i < OPCODE_COUNT; i++) {
    if (opcodeValue == OPCODE_VALUES[i]) {
      return i;
    }
  }
  return NOP;
}
