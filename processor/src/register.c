#include "processor/register.h"

const unsigned int REGISTER_COUNT = 16;

const unsigned char REGISTER_CODES[] = {
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
  "nl",
  "ip",
  "sp",
  "ac",
  "x0",
  "x1",
  "x2",
  "x3",
  "x4",
  "x5",
  "x6",
  "x7",
  "x8",
  "x9",
  "x10",
  "x11",
};

enum Register decodeRegister(unsigned char registerId) {
  for (unsigned int i = 0; i < REGISTER_COUNT; i++) {
    if (registerId == REGISTER_CODES[i]) {
      return i;
    }
  }
  return NL;
}
