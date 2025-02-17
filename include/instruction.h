// Layout (indicated by last 3 bits of opcode):
// 0: | opcode (8 bits) |
// 1: | opcode (8 bits) | reg A (4 bits) | reg B (4 bits) |
// 2: | opcode (8 bits) | reg A (4 bits) | imm (4 bits)   |
// 3: | opcode (8 bits) | imm (8 bits)                    |
// 4: | opcode (8 bits) | imm (16 bits)                                                     |

enum Opcode {
  // Control flow
  NOP,  // layout 0 | no params
  JMP,  // layout 4 | imm[16]
  JMZ,  // layout 4 | imm[16]
  JML,  // layout 4 | imm[16]
  // Memory
  MOV,  // layout 1 | reg, reg
  LDIB, // layout 3 | imm[8]
  LDIW, // layout 4 | imm[16]
  LDMB, // layout 1 | reg, reg
  LDMW, // layout 1 | reg, reg
  STMB, // layout 1 | reg, reg
  STMW, // layout 1 | reg, reg
  PSHB, // layout 0 | no params
  PSHW, // layout 0 | no params
  POPB, // layout 0 | no params
  POPW, // layout 0 | no params
  // Math and logic
  ADD,  // layout 1 | reg, reg
  SUB,  // layout 1 | reg, reg
  MUL,  // layout 1 | reg, reg
  DIVS, // layout 1 | reg, reg
  DIVU, // layout 1 | reg, reg
  REMS, // layout 1 | reg, reg
  REMU, // layout 1 | reg, reg
  LSH,  // layout 1 | reg, reg
  RSHS, // layout 1 | reg, reg
  RSHU, // layout 1 | reg, reg
  LSI,  // layout 2 | reg, imm[4]
  RSIS, // layout 2 | reg, imm[4]
  RSIU, // layout 2 | reg, imm[4]
  AND,  // layout 1 | reg, reg
  IOR,  // layout 1 | reg, reg
  XOR,  // layout 1 | reg, reg
  CEQ,  // layout 1 | reg, reg
  CNE,  // layout 1 | reg, reg
  CLTS, // layout 1 | reg, reg
  CLTU, // layout 1 | reg, reg
  CGES, // layout 1 | reg, reg
  CGEU, // layout 1 | reg, reg
};

enum Register {
  IP,
  SP,
  AC,
  X0 = 0,
  X1 = 1,
  X2 = 2,
  X3 = 3,
  X4 = 4,
  X5 = 5,
  X6 = 6,
  X7 = 7,
};

struct Instruction {
  enum Opcode opcode;
  enum Register registerA;
  enum Register registerB;
  enum Register registerC;
  unsigned short immediateValue;
};
