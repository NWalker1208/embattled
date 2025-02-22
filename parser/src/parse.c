#include "parser/parse.h"
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

bool skipInlineWhitespace(char** text) {
  bool skippedAny = false;
  while (isInlineWhitespace(**text)) {
    (*text)++;
    skippedAny = true;
  }
  return skippedAny;
}

void skipAllWhitespace(char** text) {
  while (isspace(**text)) {
    (*text)++;
  }
}

void skipToEndOfLine(char** text) {
  while (**text != '\0' && **text != '\n' && **text != '\r') {
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
  if ('0' <= c && c <= '9') {
    *result = c - '0';
    return true;
  } else if ('a' <= c && c <= 'f') {
    *result = c - 'a' + 10;
    return true;
  } else if ('A' <= c && c <= 'F') {
    *result = c - 'A' + 10;
    return true;
  } else {
    return false;
  }
}

// Parses a register name.
// If parsing succeeds, outputs the register through result and returns true.
// If parsing fails, returns false.
// Expects the text to have already advanced past the "$" character.
bool tryParseRegister(char** text, enum Register* result) {
  if (startsWithCaseInsensitive(*text, "ac")) {
    *result = AC;
    *text += 2; // Skip past "ac"
  } else if (startsWithCaseInsensitive(*text, "x0")) {
    *result = X0;
    *text += 2; // Skip past "x0"
  } else if (startsWithCaseInsensitive(*text, "x1")) {
    *result = X1;
    *text += 2; // Skip past "x1"
  } else if (startsWithCaseInsensitive(*text, "x2")) {
    *result = X2;
    *text += 2; // Skip past "x2"
  } else if (startsWithCaseInsensitive(*text, "x3")) {
    *result = X3;
    *text += 2; // Skip past "x3"
    // TODO: Add other registers
  } else {
    return false;
  }
  return true;
}

// Parses an immediate value of between 1 and 4 hexadecimal digits, stopping at the first whitespace.
// If parsing succeeds, outputs the value through result and returns true.
// If parsing fails, advances to the next whitespace character and returns false.
// Expects the text to have already advanced past the "0x" characters.
bool tryParseImmediateValue(char** text, unsigned short* result) {
  unsigned short value = 0;
  unsigned char nibble;
  int i;
  for (i = 0; i <= 4 && **text != '\0' && !isspace(**text); i++) {
    if (!tryHexToNibble(**text, &nibble) || i == 4) {
      while (**text != '\0' && !isspace(**text)) { (*text)++; }
      return false;
    }
    value = (value << 4) | nibble;
    (*text)++;
  }
  if (i == 0) {
    return false;
  }
  *result = value;
  return true;
}

// Parses an instruction, including its opcode and any parameters.
// If parsing succeeds, outputs the instruction through result and returns true.
// If parsing fails, prints an error to err, advances to the end of the line,
// and returns false.
// Expects the text to start at the beginning of the opcode.
bool tryParseInstruction(FILE* err, char** text, struct Instruction* result) {
  struct Instruction parsedInstruction = { 0 }; // Copied to result on success, cleaned up on failure

  // Parse the opcode
  if (startsWithCaseInsensitive(*text, "nop")) {
    parsedInstruction.opcode = NOP;
    *text += 3; // Skip past "nop"
  } else if (startsWithCaseInsensitive(*text, "jmp")) {
    parsedInstruction.opcode = JMP;
    *text += 3; // Skip past "jmp"
  } else if (startsWithCaseInsensitive(*text, "jmz")) {
    parsedInstruction.opcode = JMZ;
    *text += 3; // Skip past "jmz"
  } else if (startsWithCaseInsensitive(*text, "mov")) {
    parsedInstruction.opcode = MOV;
    *text += 3; // Skip past "mov"
  } else if (startsWithCaseInsensitive(*text, "ldib")) {
    parsedInstruction.opcode = LDIB;
    *text += 4; // Skip past "ldib"
  } else if (startsWithCaseInsensitive(*text, "ldiw")) {
    parsedInstruction.opcode = LDIW;
    *text += 4; // Skip past "ldiw"
  } else if (startsWithCaseInsensitive(*text, "ldmb")) {
    parsedInstruction.opcode = LDMB;
    *text += 4; // Skip past "ldmb"
  } else if (startsWithCaseInsensitive(*text, "ldmw")) {
    parsedInstruction.opcode = LDMW;
    *text += 4; // Skip past "ldmw"
  } else if (startsWithCaseInsensitive(*text, "stmb")) {
    parsedInstruction.opcode = STMB;
    *text += 4; // Skip past "stmb"
  } else if (startsWithCaseInsensitive(*text, "stmw")) {
    parsedInstruction.opcode = STMW;
    *text += 4; // Skip past "stmw"
  } else if (startsWithCaseInsensitive(*text, "pshb")) {
    parsedInstruction.opcode = PSHB;
    *text += 4; // Skip past "pshb"
  } else if (startsWithCaseInsensitive(*text, "pshw")) {
    parsedInstruction.opcode = PSHW;
    *text += 4; // Skip past "pshw"
    // TODO: Add other instructions
  } else {
    fprintf(err, "Error: Unrecognized opcode.\n");
    skipToEndOfLine(text);
    return false;
  }

  // Skip past any in-line whitespace
  if (!skipInlineWhitespace(text)) {
    fprintf(err, "Error: Expected whitespace after the opcode.\n");
    skipToEndOfLine(text);
    return false;
  }

  // Parse the first parameter
  bool canHaveSecondParameter;
  if (**text == '$') {
    (*text)++;
    if (!tryParseRegister(text, &parsedInstruction.registerA)) {
      fprintf(err, "Error: Expected a register name.\n");
      skipToEndOfLine(text);
      return false;
    }
    canHaveSecondParameter = true;
  } else if (**text == '0' && tolower((*text)[1]) == 'x') {
    (*text) += 2; // Skip past "0x"
    if (!tryParseImmediateValue(text, &parsedInstruction.immediateValue)) {
      fprintf(err, "Error: Expected a 1-4 digit hexadecimal immediate value.\n");
      skipToEndOfLine(text);
      return false;
    }
    canHaveSecondParameter = false; // Immediate value is always the last parameter.
  } else if (**text == '@') {
    // TODO: Pull out label name after modifying struct to hold label name.
    canHaveSecondParameter = false; // Label reference is always the last parameter.
  } else {
    fprintf(err, "Error: Expected a register, immediate value, or label reference.\n");
    skipToEndOfLine(text);
    return false;
  }

  if (canHaveSecondParameter) {
    // Skip past any in-line whitespace
    if (!skipInlineWhitespace(text)) {
      fprintf(err, "Error: Expected whitespace after the first parameter.\n");
      skipToEndOfLine(text);
      return false;
    }
  
    // Parse the second parameter
    if (**text == '$') {
      (*text)++;
      if (!tryParseRegister(text, &parsedInstruction.registerB)) {
        fprintf(err, "Error: Expected a register name.\n");
        skipToEndOfLine(text);
        return false;
      }
    } else if (**text == '0' && tolower((*text)[1]) == 'x') {
      (*text) += 2; // Skip past "0x"
      if (!tryParseImmediateValue(text, &parsedInstruction.immediateValue)) {
        fprintf(err, "Error: Expected a 1-4 digit hexadecimal immediate value.\n");
        skipToEndOfLine(text);
        return false;
      }
    } else { // Label references can only exist as the first and only parameter.
      fprintf(err, "Error: Expected a register or immediate value.\n");
      skipToEndOfLine(text);
      return false;
    }
  }

  *result = parsedInstruction;
  return true;
}

// Parses a data line containing 1 or more hexadecimal bytes.
// If parsing succeeds, outputs the instruction through result and returns true.
// If parsing fails, prints an error to err, advances to the end of the line,
// and returns false.
// Expects the text to have already advanced past the ".data" characters.
bool tryParseData(FILE* err, char** text, struct BinaryData* result) {
  struct BinaryData parsedData = { 0 }; // Copied to result on success, cleaned up on failure

  // Skip past any in-line whitespace
  if (!skipInlineWhitespace(text)) {
    fprintf(err, "Error: Expected whitespace after \".data\".\n");
    skipToEndOfLine(text);
    return false;
  }

  // Parse the data bytes
  while (**text != '\0' && **text != '\n' && **text != '\r') {
    unsigned char upperNibble;
    if (!tryHexToNibble(**text, &upperNibble)) {
      fprintf(err, "Error: Expected a 2-digit hexadecimal byte.\n");
      skipToEndOfLine(text);
      free(parsedData.bytes);
      return false;
    }
    (*text)++;
    unsigned char lowerNibble;
    if (!tryHexToNibble(**text, &lowerNibble)) {
      fprintf(err, "Error: Expected a 2-digit hexadecimal byte.\n");
      skipToEndOfLine(text);
      free(parsedData.bytes);
      return false;
    }
    (*text)++;
    parsedData.length++;
    parsedData.bytes = (unsigned char*)realloc(parsedData.bytes, parsedData.length);
    parsedData.bytes[parsedData.length - 1] = (upperNibble << 4) | lowerNibble;

    skipInlineWhitespace(text); // Allow optional whitespace between bytes
  }

  *result = parsedData;
  return true;
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

// TODO: Write unit tests
