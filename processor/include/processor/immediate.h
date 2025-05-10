#pragma once
#include <stdint.h>

// An immediate value. May be 4, 8, or 16-bit and may be signed or unsigned.
typedef union ImmediateValue {
  uint16_t u16 : 16; // Unsigned 16-bit value.
  uint16_t u8 : 8; // Unsigned 8-bit value.
  uint16_t u4 : 4; // Unsigned 4-bit value.

  int16_t s16 : 16; // Signed 16-bit value.
  int16_t s8 : 8; // Signed 8-bit value.
  int16_t s4 : 4; // Signed 4-bit value.
} ImmediateValue;
