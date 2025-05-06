#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "processor/register.h"

// Describes how the operands are laid out in memory for a given opcode.
typedef struct OperandLayout {
  uint8_t numBytes : 3; // The number of bytes used to store all parameters (0-4).
  bool hasRegA : 1; // Whether register A is present.
  bool hasRegB : 1; // Whether register B is present.
  bool hasRegC : 1; // Whether register C is present.
  uint8_t numImmABits : 5; // The number of bits used to store immediate value A (0, 4, 8, or 16).
  bool hasImmB : 1; // Whether immediate value B (16-bit) is present.
} OperandLayout;

#define _OPERAND_LAYOUT(_numBytes, _hasRegA, _hasRegB, _hasRegC, _numImmABits, _hasImmB) \
  (OperandLayout){ \
    .numBytes=(_numBytes), \
    .hasRegA=(_hasRegA), \
    .hasRegB=(_hasRegB), \
    .hasRegC=(_hasRegC), \
    .numImmABits=(_numImmABits), \
    .hasImmB=(_hasImmB) \
  }

// 0 bytes
#define OPERAND_LAYOUT_NONE             _OPERAND_LAYOUT(0, false, false, false, 0, false)  // [ opcode (8 bits) ]
// 1 byte
#define OPERAND_LAYOUT_REGA             _OPERAND_LAYOUT(1, true, false, false, 0, false)   // [ opcode (8 bits) ][ reg A (4 bits)  | unused (4 bits)    ]
#define OPERAND_LAYOUT_REGA_REGB        _OPERAND_LAYOUT(1, true, true, false, 0, false)    // [ opcode (8 bits) ][ reg A (4 bits)  | reg B (4 bits)     ]
// 2 bytes
#define OPERAND_LAYOUT_IMMA16           _OPERAND_LAYOUT(2, false, false, false, 16, false) // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immA[8:15] (8 bits)              ]
#define OPERAND_LAYOUT_REGA_IMMA8       _OPERAND_LAYOUT(2, true, false, false, 8, false)   // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ reg A (4 bits) | unused (4 bits) ]
#define OPERAND_LAYOUT_REGA_REGB_IMMA4  _OPERAND_LAYOUT(2, true, true, false, 4, false)    // [ opcode (8 bits) ][ unused (4 bits) | immA[0:3] (4 bits) ][ reg A (4 bits) | reg B (4 bits)  ]
#define OPERAND_LAYOUT_REGA_REGB_REGC   _OPERAND_LAYOUT(2, true, true, true, 0, false)     // [ opcode (8 bits) ][ reg C (4 bits)  | unused (4 bits)    ][ reg A (4 bits) | reg B (4 bits)  ]
// 3 bytes
#define OPERAND_LAYOUT_IMMA8_IMMB16     _OPERAND_LAYOUT(3, false, false, false, 8, true)   // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immB[0:7] (8 bits)               ][ immB[8:15] (8 bits)              ]
#define OPERAND_LAYOUT_REGA_IMMA16      _OPERAND_LAYOUT(3, true, false, false, 16, false)  // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immA[8:15] (8 bits)              ][ reg A (4 bits) | unused (4 bits) ]
#define OPERAND_LAYOUT_REGA_REGB_IMMA16 _OPERAND_LAYOUT(3, true, true, false, 16, false)   // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immA[8:15] (8 bits)              ][ reg A (4 bits) | reg B (4 bits)  ]
// 4 bytes
#define OPERAND_LAYOUT_IMMA16_IMMB16    _OPERAND_LAYOUT(4, false, false, false, 16, true)  // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immA[8:15] (8 bits)              ][ immB[0:7] (8 bits)               ][ immB[8:15] (8 bits) ]

// The operands for an instruction to execute.
typedef struct InstructionOperands {
  Register regA;
  Register regB;
  Register regC;
  uint16_t immA;
  uint16_t immB;
} InstructionOperands;
