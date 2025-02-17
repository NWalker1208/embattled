#include "parse.h"
#include <stdbool.h>
#include <string.h>

#define NULL 0

// Assembly Syntax:
// <file> -> <line>*
// <line> ->
//   [<label>:] (
//     (<opcode> [(<register> [<register> | <immediateNibble>]) | <immediateByte> | <immediateWord> | <labelReference>]) |
//     ('.data' <hexValue>+)
//   )
// <register> -> '$' <registerName>
// <immediateNibble> -> '0x' <hexValue> // Must be between 0x0 and 0xF
// <immediateByte> -> '0x' <hexValue> // Must be between 0x0 and 0xFF
// <immediateWord> -> '0x' <hexValue> // Must be between 0x0 and 0xFFFF
// <labelReference> -> '@' <label>
// <hexValue> -> [0-9a-fA-F]+


struct AssemblyLine parseNextLine(char** text) {
  struct AssemblyLine parsedLine = { 0 };

  return parsedLine;
}
