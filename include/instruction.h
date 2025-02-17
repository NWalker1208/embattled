// Layout:
// 0: | opcode (8 bits) |
// 1: | opcode (8 bits) | reg A (4 bits) | reg B (4 bits) |
// 2: | opcode (8 bits) | reg A (4 bits) | imm (4 bits)   |
// 3: | opcode (8 bits) | imm (8 bits)                    |
// 4: | opcode (8 bits) | imm (16 bits)                                                     |
// (last 2 bits of opcode represent number of bytes in addition to opcode)

enum Opcode {
  // Control flow
  NOP  = 0b000000'00, // layout 0 | no params
  JMP  = 0b000001'10, // layout 4 | imm[16]
  JMZ  = 0b000010'10, // layout 4 | imm[16]
  JML  = 0b000011'10, // layout 4 | imm[16]
  // Memory
  MOV  = 0b000100'01, // layout 1 | reg, reg
  LDIB = 0b000110'01, // layout 3 | imm[8]
  LDIW = 0b000111'10, // layout 4 | imm[16]
  LDMB = 0b001000'01, // layout 1 | reg, reg
  LDMW = 0b001001'01, // layout 1 | reg, reg
  STMB = 0b001010'01, // layout 1 | reg, reg
  STMW = 0b001011'01, // layout 1 | reg, reg
  PSHB = 0b001100'00, // layout 0 | no params
  PSHW = 0b001101'00, // layout 0 | no params
  POPB = 0b001110'00, // layout 0 | no params
  POPW = 0b001111'00, // layout 0 | no params
  // Math and logic
  ADD  = 0b010000'01, // layout 1 | reg, reg
  SUB  = 0b010001'01, // layout 1 | reg, reg
  MUL  = 0b010010'01, // layout 1 | reg, reg
  DIVS = 0b010110'01, // layout 1 | reg, reg
  DIVU = 0b010111'01, // layout 1 | reg, reg
  REMS = 0b011000'01, // layout 1 | reg, reg
  REMU = 0b011001'01, // layout 1 | reg, reg
  LSH  = 0b011010'01, // layout 1 | reg, reg
  RSHS = 0b011011'01, // layout 1 | reg, reg
  RSHU = 0b011100'01, // layout 1 | reg, reg
  LSI  = 0b011101'01, // layout 2 | reg, imm[4]
  RSIS = 0b011110'01, // layout 2 | reg, imm[4]
  RSIU = 0b011111'01, // layout 2 | reg, imm[4]
  AND  = 0b100000'01, // layout 1 | reg, reg
  IOR  = 0b100001'01, // layout 1 | reg, reg
  XOR  = 0b100010'01, // layout 1 | reg, reg
  CEQ  = 0b100011'01, // layout 1 | reg, reg
  CNE  = 0b100100'01, // layout 1 | reg, reg
  CLTS = 0b100101'01, // layout 1 | reg, reg
  CLTU = 0b100110'01, // layout 1 | reg, reg
  CGES = 0b100111'01, // layout 1 | reg, reg
  CGEU = 0b101000'01, // layout 1 | reg, reg
};

enum Register {
  // Special registers
  NL = 0, // Null register. Always 0.
  IP = 1, // Instruction pointer. Address of the next instruction to execute.
  SP = 2, // Stack pointer. Address of the top of the stack.
  AC = 3, // Accumulator. Holds the result of most instructions.
  // General registers
  X0 = 4,
  X1 = 5,
  X2 = 6,
  X3 = 7,
  X4 = 8,
  X5 = 9,
  X6 = 10,
  X7 = 11,
  X8 = 12,
  X9 = 13,
  X10 = 14,
  X11 = 15,
};

struct Instruction {
  enum Opcode opcode;
  enum Register registerA;
  enum Register registerB;
  unsigned short immediateValue;
};
