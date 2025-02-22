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

enum Register decodeRegister(unsigned char registerId);
