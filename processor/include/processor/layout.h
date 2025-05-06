#pragma once
#include <stdbool.h>
#include <stdint.h>

// Describes how instructions of a particular opcode are laid out in memory.
typedef struct InstructionLayout {
  uint8_t numBytes : 3; // The number of bytes used to encode the instruction (1-5).
  bool hasRegA : 1; // Whether register A is present.
  bool hasRegB : 1; // Whether register B is present.
  bool hasRegC : 1; // Whether register C is present.
  uint8_t numImmABits : 5; // The number of bits used to store immediate value A (0, 4, 8, or 16).
  bool hasImmB : 1; // Whether immediate value B (16-bit) is present.
} InstructionLayout;

#define _INSTRUCTION_LAYOUT(_numBytes, _hasRegA, _hasRegB, _hasRegC, _numImmABits, _hasImmB) \
  (InstructionLayout){ \
    .numBytes=(_numBytes), \
    .hasRegA=(_hasRegA), \
    .hasRegB=(_hasRegB), \
    .hasRegC=(_hasRegC), \
    .numImmABits=(_numImmABits), \
    .hasImmB=(_hasImmB) \
  }

// 1 bytes
#define INSTRUCTION_LAYOUT_NONE             _INSTRUCTION_LAYOUT(1, false, false, false, 0, false)  // [ opcode (8 bits) ]
// 2 byte
#define INSTRUCTION_LAYOUT_REGA             _INSTRUCTION_LAYOUT(2, true, false, false, 0, false)   // [ opcode (8 bits) ][ reg A (4 bits)  | unused (4 bits)    ]
#define INSTRUCTION_LAYOUT_REGA_REGB        _INSTRUCTION_LAYOUT(2, true, true, false, 0, false)    // [ opcode (8 bits) ][ reg A (4 bits)  | reg B (4 bits)     ]
// 3 bytes
#define INSTRUCTION_LAYOUT_IMMA16           _INSTRUCTION_LAYOUT(3, false, false, false, 16, false) // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immA[8:15] (8 bits)              ]
#define INSTRUCTION_LAYOUT_REGA_IMMA8       _INSTRUCTION_LAYOUT(3, true, false, false, 8, false)   // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ reg A (4 bits) | unused (4 bits) ]
#define INSTRUCTION_LAYOUT_REGA_REGB_IMMA4  _INSTRUCTION_LAYOUT(3, true, true, false, 4, false)    // [ opcode (8 bits) ][ unused (4 bits) | immA[0:3] (4 bits) ][ reg A (4 bits) | reg B (4 bits)  ]
#define INSTRUCTION_LAYOUT_REGA_REGB_REGC   _INSTRUCTION_LAYOUT(3, true, true, true, 0, false)     // [ opcode (8 bits) ][ reg C (4 bits)  | unused (4 bits)    ][ reg A (4 bits) | reg B (4 bits)  ]
// 4 bytes
#define INSTRUCTION_LAYOUT_IMMA8_IMMB16     _INSTRUCTION_LAYOUT(4, false, false, false, 8, true)   // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immB[0:7] (8 bits)               ][ immB[8:15] (8 bits)              ]
#define INSTRUCTION_LAYOUT_REGA_IMMA16      _INSTRUCTION_LAYOUT(4, true, false, false, 16, false)  // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immA[8:15] (8 bits)              ][ reg A (4 bits) | unused (4 bits) ]
#define INSTRUCTION_LAYOUT_REGA_REGB_IMMA16 _INSTRUCTION_LAYOUT(4, true, true, false, 16, false)   // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immA[8:15] (8 bits)              ][ reg A (4 bits) | reg B (4 bits)  ]
// 5 bytes
#define INSTRUCTION_LAYOUT_IMMA16_IMMB16    _INSTRUCTION_LAYOUT(5, false, false, false, 16, true)  // [ opcode (8 bits) ][ immA[0:7] (8 bits)                   ][ immA[8:15] (8 bits)              ][ immB[0:7] (8 bits)               ][ immB[8:15] (8 bits) ]
