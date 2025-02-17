#include "parse.h"
#include <stdbool.h>
#include <ctype.h>
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
// Whitespace between any two tokens is ignored.
// Additionally, newlines between a label and the subsequent token are ignored.

bool startsWithCaseInsensitive(char* text, char* prefix) {
  while (*text != '\0' && *prefix != '\0') {
    if (tolower(*text) != tolower(*prefix)) {
      return false;
    }
    text++;
    prefix++;
  }
  return *prefix == '\0';
}

bool isInlineWhitespace(char c) {
  return c == ' ' || c == '\t';
}

void skipInlineWhitespace(char** text) {
  while (isInlineWhitespace(**text)) {
    (*text)++;
  }
}

void skipAllWhitespace(char** text) {
  while (isspace(**text)) {
    (*text)++;
  }
}

struct AssemblyLine parseNextLine(char** text) {
  struct AssemblyLine parsedLine = { 0 };

  return parsedLine;
}
