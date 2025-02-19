#include "parse.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Assembly Syntax:
// <file> -> <line>*
// <line> ->
//   [<label>:] (
//     (<opcode> [(<register> [<register> | <immediateValue>]) | <immediateValue> | <labelReference>]) |
//     ('.data' <hexByte>+)
//   )
// <register> -> '$' <registerName>
// <immediateValue> -> '0x' <hexDigit>{1,4}
// <labelReference> -> '@' <label>
// <hexByte> -> <hexDigit><hexDigit>
// <hexDigit> -> [0-9a-fA-F]

// Whitespace between any two tokens is ignored.
// Additionally, newlines between a label and the subsequent token are ignored.

bool startsWithCaseInsensitive(const char* text, const char* prefix) {
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

// Finds the first occurence of the given character in the first line of the string.
// If the character is not found, the function returns NULL.
char* findCharOnLine(char* str, char c) {
  while (*str != '\0' && *str != '\n' && *str != '\r') {
    if (*str == c) {
      return str;
    }
    str++;
  }
  return NULL;
}

// Converts a hexadecimal digit to its associated nibble value (0 - 15).
// If the character is a hexadecimal digit, outputs through result and returns true.
// If the character is not a hexadecimal digit, returns false.
bool tryHexToNibble(char c, unsigned char* result) {

}

// Parses an immediate value of between 1 and 4 hexadecimal digits, stopping at the first whitespace.
// If parsing succeeds, outputs the value through result and returns true.
// If parsing fails, advances to the next whitespace character and returns false.
// Expects the text to have already advanced past the "0x" characters.
bool tryParseImmediateValue(char** text, unsigned short* result) {

}

// Parses an instruction, including its opcode and any parameters.
// If parsing succeeds, outputs the instruction through result and returns true.
// If parsing fails, prints an error to err, advances to the end of the line,
// and returns false.
// Expects the text to start at the beginning of the opcode.
bool tryParseInstruction(FILE* err, char** text, struct Instruction* result) {

}

// Parses a data line containing 1 or more hexadecimal bytes.
// If parsing succeeds, outputs the instruction through result and returns true.
// If parsing fails, prints an error to err, advances to the end of the line,
// and returns false.
// Expects the text to have already advanced past the ".data" characters.
bool tryParseData(FILE* err, char** text, struct BinaryData* result) {

}

bool tryParseNextLine(FILE* err, char** text, struct AssemblyLine* result) {
  struct AssemblyLine parsedLine = { 0 }; // Copied to result on success, cleaned up on failure.

  // Skip leading whitespace and blank lines
  skipAllWhitespace(text);
  
  // Check for a label
  char* labelStart = *text;
  char* labelEnd = findCharOnLine(*text, ':');
  if (labelEnd != NULL) {
    // Allocate a new string for the label
    size_t labelLength = labelEnd - labelStart;
    parsedLine.label = (char*)malloc(labelLength + 1);
    memcpy(parsedLine.label, labelStart, labelLength);
    parsedLine.label[labelLength] = '\0';

    // Skip past the label, the colon, and any whitespace
    *text = labelEnd + 1;
    skipAllWhitespace(text);
  }

  // Check whether this is a data line or an instruction
  if (startsWithCaseInsensitive(*text, ".data")) {
    parsedLine.kind = DATA;
    *text += 5; // Skip past ".data"
    skipAllWhitespace(text);

    if (!tryParseData(err, text, &parsedLine.contents.data)) {
      free(parsedLine.label);
      return false;
    }
  } else {
    parsedLine.kind = INSTRUCTION;
    
    if (!tryParseInstruction(err, text, &parsedLine.contents.instruction)) {
      free(parsedLine.label);
      return false;
    }
  }

  *result = parsedLine;
  return true;
}
