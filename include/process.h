const unsigned int MEMORY_SIZE = 65536;

struct ProcessState {
  // Special registers
  unsigned short ip;
  unsigned short sp;
  unsigned short ac;

  // General registers
  unsigned short x0;
  unsigned short x1;
  unsigned short x2;
  unsigned short x3;
  unsigned short x4;
  unsigned short x5;
  unsigned short x6;
  unsigned short x7;
  unsigned short x8;
  unsigned short x9;
  unsigned short x10;
  unsigned short x11;
};

void stepProcess(unsigned char* memory, struct ProcessState* processState);

void printProcessState(struct ProcessState* processState);
