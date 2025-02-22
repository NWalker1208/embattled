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
  PSHB, // layout 2 | reg
  PSHW, // layout 2 | reg
  POPB, // layout 2 | reg
  POPW, // layout 2 | reg
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

extern const unsigned int OPCODE_COUNT;
extern const unsigned char OPCODE_VALUES[];
extern const char* OPCODE_NAMES[];

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

extern const unsigned int REGISTER_COUNT;
extern const unsigned char REGISTER_IDS[];
extern const char* REGISTER_NAMES[];

struct Instruction {
  enum Opcode opcode;
  enum Register registerA;
  enum Register registerB;
  unsigned short immediateValue;
};

enum Opcode decodeOpcode(unsigned char opcodeValue);
enum Register decodeRegister(unsigned char registerId);
