// Layout:
// 0: | opcode (8 bits) |
// 1: | opcode (8 bits) | reg A (4 bits) | reg B (4 bits) |
// 2: | opcode (8 bits) | reg A (4 bits) | imm (4 bits)   |
// 3: | opcode (8 bits) | imm (8 bits)                    |
// 4: | opcode (8 bits) | imm (16 bits)                                                     |
// (last 2 bits of opcode represent number of bytes in addition to opcode)

enum Opcode {
  // Control flow
  NOP,  // layout 0 | no params
  JMP,  // layout 4 | imm[16]
  JMZ,  // layout 4 | imm[16]
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

const unsigned int OPCODE_COUNT = 36;

const unsigned char OPCODE_VALUES[] = {
  0b000000'00, // NOP
  0b000001'10, // JMP
  0b000010'10, // JMZ
  0b000100'01, // MOV
  0b000110'01, // LDIB
  0b000111'10, // LDIW
  0b001000'01, // LDMB
  0b001001'01, // LDMW
  0b001010'01, // STMB
  0b001011'01, // STMW
  0b001100'00, // PSHB
  0b001101'00, // PSHW
  0b001110'00, // POPB
  0b001111'00, // POPW
  0b010000'01, // ADD
  0b010001'01, // SUB
  0b010010'01, // MUL
  0b010110'01, // DIVS
  0b010111'01, // DIVU
  0b011000'01, // REMS
  0b011001'01, // REMU
  0b011010'01, // LSH
  0b011011'01, // RSHS
  0b011100'01, // RSHU
  0b011101'01, // LSI
  0b011110'01, // RSIS
  0b011111'01, // RSIU
  0b100000'01, // AND
  0b100001'01, // IOR
  0b100010'01, // XOR
  0b100011'01, // CEQ
  0b100100'01, // CNE
  0b100101'01, // CLTS
  0b100110'01, // CLTU
  0b100111'01, // CGES
  0b101000'01, // CGEU
};

const char* OPCODE_NAMES[] = {
  "NOP",
  "JMP",
  "JMZ",
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

enum Register {
  // Special registers
  NL, // Null register. Always 0.
  IP, // Instruction pointer. Address of the next instruction to execute.
  SP, // Stack pointer. Address of the top of the stack.
  AC, // Accumulator. Holds the result of most instructions.
  // General registers
  X0,
  X1,
  X2,
  X3,
  X4,
  X5,
  X6,
  X7,
  X8,
  X9,
  X10,
  X11,
};

const unsigned int REGISTER_COUNT = 16;

const unsigned char REGISTER_IDS[] = {
  0b0000, // NL
  0b0001, // IP
  0b0010, // SP
  0b0011, // AC
  0b0100, // X0
  0b0101, // X1
  0b0110, // X2
  0b0111, // X3
  0b1000, // X4
  0b1001, // X5
  0b1010, // X6
  0b1011, // X7
  0b1100, // X8
  0b1101, // X9
  0b1110, // X10
  0b1111, // X11
};

const char* REGISTER_NAMES[] = {
  "NL",
  "IP",
  "SP",
  "AC",
  "X0",
  "X1",
  "X2",
  "X3",
  "X4",
  "X5",
  "X6",
  "X7",
  "X8",
  "X9",
  "X10",
  "X11",
};

struct Instruction {
  enum Opcode opcode;
  enum Register registerA;
  enum Register registerB;
  unsigned short immediateValue;
};
