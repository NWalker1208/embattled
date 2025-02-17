enum Opcode {
  // Control flow
  NOP,
  JMP,
  JMZ,
  // Memory
  MOV,
  LDI,
  LDM,
  STM,
  // Math and logic
  ADD,
  SUB,
  MUL,
  DIVS,
  DIVU,
  REMS,
  REMU,
  LSH,
  RSHS,
  RSHU,
  AND,
  IOR,
  XOR,
  CEQ,
  CNE,
  CLTS,
  CLTU,
  CGES,
  CGEU,
};

enum Register {
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
