#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "parser/parse.h"
#include "parser/utilities.h"

#pragma region Error messages

const char* INVALID_LABEL = "Invalid label";
const char* INVALID_OPCODE = "Invalid opcode";
const char* INVALID_PARAMETER = "Invalid parameter";
const char* INVALID_REGISTER = "Invalid register";
const char* INVALID_HEX_VALUE = "Invalid or out-of-range hexadecimal value";
const char* INVALID_INT_VALUE = "Invalid or out-of-range integer value";
const char* INVALID_BYTE = "Invalid data byte";
const char* UNEXPECTED_CHARACTER = "Unexpected character";
const char* UNEXPECTED_END_OF_FILE = "Unexpected end of file";

#define PARSING_ERROR(_message, _location) (struct ParsingError){.message = (_message), .location = (_location)}

#pragma endregion

#pragma region Helper function signatures

// Parses an assembly instruction.
// If the current line is a valid instruction, advances text to the end of the line,
// outputs the instruction through result, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseInstruction(const char** text, struct AssemblyInstruction* instruction, struct ParsingError* error);

// Parses hexadecimal assembly data bytes.
// Expects text to be at the first hexadecimal byte (past the '.data' sequence).
// If text contains valid hexadecimal bytes from the start to the end of the line,
// advances text to the end of the line, outputs the data through result, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseAssemblyData(const char** text, struct AssemblyData* data, struct ParsingError* error);

// Parses an assembly instruction parameter.
// If parsing succeeds, advances text past the end of the parameter,
// outputs the parameter through result, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseParameter(const char** text, struct AssemblyParameter* parameter, struct ParsingError* error);

// Parses an opcode name.
// Expects text to be at the start of the opcode name.
// If parsing succeeds, advances text past the end of the opcode name,
// outputs the opcode through result and returns true.
// If parsing fails, returns false.
bool tryParseOpcode(const char** text, enum Opcode* opcode);

// Parses a register name.
// Expects text to be at the start of the register name (past the "$" character).
// If parsing succeeds, advances text past the end of the register name,
// outputs the register through result and returns true.
// If parsing fails, returns false.
bool tryParseRegister(const char** text, enum Register* reg);

// Parses a hexadecimal immediate value.
// Expects text to be at the first hexadecimal digit (pase the "0x" sequence).
// If parsing succeeds, advances text past the end of the hexadecimal digits,
// outputs the immediate value through result and returns true.
// If parsing fails, returns false.
bool tryParseImmediateHexValue(const char** text, signed int* value);

// Parses a decimal immediate value.
// Expects text to be at the first digit, or at the sign character if one is present.
// If parsing succeeds, advances text past the end of the digits,
// outputs the immediate value through result and returns true.
// If parsing fails, returns false.
bool tryParseImmediateDecValue(const char** text, signed int* value);

// Parses and copies a label name.
// If parsing succeeds, advances text past the end of the label and returns a pointer to a new
// null-terminated array containing a copy of the label.
// If parsing fails, leaves text as it is and returns NULL.
char* tryCopyLabel(const char** text);

#pragma endregion

bool tryParseAssemblyLine(const char** text, struct AssemblyLine* line, struct ParsingError* error) {
  skipAllWhitespace(text);

  // Parse the label if one is present
  if (findCharOnLine(*text, ':') != NULL) {
    line->label = tryCopyLabel(text);
    if (line->label == NULL) {
      *error = PARSING_ERROR(INVALID_LABEL, *text);
      destroyAssemblyLine(line);
      skipToNextLine(text);
      return false; // Failed to parse label
    }
    skipInlineWhitespace(text);
    if (**text != ':') {
      *error = PARSING_ERROR(UNEXPECTED_CHARACTER, *text);
      destroyAssemblyLine(line);
      skipToNextLine(text);
      return false; // Expected ':' after label
    }
    skipAllWhitespace(text);
  }

  // If line starts with ".data", parse as assembly data.
  // Otherwise, parse as an assembly instruction.
  if (startsWithWordCaseInsensitive(*text, ".data")) {
    line->kind = DATA;
    if (!tryParseAssemblyData(text, &line->data, error)) {
      destroyAssemblyLine(line);
      skipToNextLine(text);
      return false; // Failed to parse assembly data
    }
  } else {
    line->kind = INSTRUCTION;
    if (!tryParseInstruction(text, &line->instruction, error)) {
      destroyAssemblyLine(line);
      skipToNextLine(text);
      return false; // Failed to parse assembly instruction
    }
  }
  assert(isEndOfLine(**text));

  // Advance to next line and return success.
  skipToNextLine(text);
  return true;
}

bool tryParseInstruction(const char** text, struct AssemblyInstruction* instruction, struct ParsingError* error) {
  // Parse the opcode
  if (!tryParseOpcode(text, &instruction->opcode)) {
    *error = PARSING_ERROR(INVALID_OPCODE, *text);
    return false; // Failed to parse opcode
  }
  skipInlineWhitespace(text);

  // Parse the parameters
  while (!isEndOfLine(**text)) {
    instruction->parameterCount++;
    instruction->parameters = realloc(instruction->parameters, instruction->parameterCount * sizeof(struct AssemblyParameter));
    if (!tryParseParameter(text, &instruction->parameters[instruction->parameterCount - 1], error)) {
      return false; // Failed to parse parameter
    }
    skipInlineWhitespace(text);
    if (**text != ',') {
      break;
    }
    (*text)++;
    skipInlineWhitespace(text);
  }

  // Check for end of line
  if (!isEndOfLine(**text)) {
    *error = PARSING_ERROR(UNEXPECTED_CHARACTER, *text);
    return false; // Expected end of line after parameter list
  }

  return true;
}

bool tryParseOpcode(const char** text, enum Opcode* opcode) {
  // Loop through the array of opcode definitions and output the first match
  for (int i = 0; i < OPCODE_COUNT; i++) {
    if (startsWithWordCaseInsensitive(*text, OPCODE_INFO[i].name)) {
      *opcode = (enum Opcode)i;
      (*text) += strlen(OPCODE_INFO[i].name);
      return true;
    }
  }
  return false;
}

bool tryParseParameter(const char** text, struct AssemblyParameter* parameter, struct ParsingError* error) {
  switch (**text) {
    case '$':  {
      // Parse as a register
      (*text)++;
      parameter->kind = REGISTER;
      if (!tryParseRegister(text, &parameter->registerName)) {
        *error = PARSING_ERROR(INVALID_REGISTER, *text);
        return false; // Failed to parse register
      }
      break;
    }
    case '@': {
      // Parse as a label reference
      (*text)++;
      parameter->kind = LABEL_REFERENCE;
      parameter->referencedLabel = tryCopyLabel(text);
      if (parameter->referencedLabel == NULL) {
        *error = PARSING_ERROR(INVALID_LABEL, *text);
        return false; // Failed to parse label
      }
      break;
    }
    default: {
      // Parse as an immediate value
      parameter->kind = IMMEDIATE_VALUE;
      
      if ((*text)[0] == '0' && (*text)[1] == 'x') {
        // Parse as a hexadecimal immediate value
        (*text) += 2;
        if (!tryParseImmediateHexValue(text, &parameter->immediateValue)) {
          *error = PARSING_ERROR(INVALID_HEX_VALUE, *text);
          return false; // Failed to parse hexadecimal immediate value
        }
      } else if (isdigit(**text) || **text == '-' || **text == '+') {
        // Parse as a decimal immediate value
        if (!tryParseImmediateDecValue(text, &parameter->immediateValue)) {
          *error = PARSING_ERROR(INVALID_INT_VALUE, *text);
          return false; // Failed to parse decimal immediate value
        }
      } else {
        *error = PARSING_ERROR(INVALID_PARAMETER, *text);
        return false; // Failed to parse as any kind of parameter
      }
      break;
    }
  }
  return true;
}

bool tryParseRegister(const char** text, enum Register* reg) {
  // Loop through the array of register names and output the first match
  for (int i = 0; i < REGISTER_COUNT; i++) {
    if (startsWithWordCaseInsensitive(*text, REGISTER_NAMES[i])) {
      *reg = (enum Register)i;
      (*text) += strlen(REGISTER_NAMES[i]);
      return true;
    }
  }
  return false;
}

bool tryParseImmediateHexValue(const char** text, signed int* value) {
  // Check that first character is valid hex
  unsigned char nibble;
  if (!tryHexToNibble(**text, &nibble)) {
    return false;
  }

  // Parse remaining hex characters
  unsigned int hexValue = nibble;
  (*text)++;
  while (tryHexToNibble(**text, &nibble)) {
    if (hexValue > ((unsigned int)UINT_MAX >> 4)) {
      return false; // Overflow
    }
    hexValue = (hexValue << 4) | (nibble & 0xF);
    (*text)++;
  }

  *value = (signed int)hexValue;
  return true;
}

bool tryParseImmediateDecValue(const char** text, signed int* value) {
  // Check for sign character (- or +)
  bool isNegative = **text == '-';
  if (isNegative || **text == '+') {
    (*text)++;
  }

  // Check that first character after sign is valid digit
  if (!isdigit(**text)) {
    return false; // Not a decimal integer
  }
  unsigned char nextDigit = **text - '0';
  (*text)++;

  // Parse remaining digits
  signed int decValue = isNegative ? -nextDigit : nextDigit;

  while (isdigit(**text)) {
    if ((isNegative && decValue < INT_MIN / 10) || (!isNegative && decValue > INT_MAX / 10)) {
      return false; // Overflow
    }
    decValue = decValue * 10;

    nextDigit = (*text)[0] - '0';
    if ((isNegative && decValue < INT_MIN + nextDigit) || (!isNegative && decValue > INT_MAX - nextDigit)) {
      return false; // Overflow
    }
    decValue += isNegative ? -nextDigit : nextDigit;
    (*text)++;
  }

  *value = decValue;
  return true;
  
  // Note on overflow handling:
  // Individual instructions have different rules about the legal range of immediate values.
  // However, the largest datatype any instruction can work with is a signed or unsigned short.
  // The ranges of both signed and unsigned shorts are fully represented in the range of signed ints.
  // Therefore, we can parse immediate values as signed ints, and allow the assembler to make narrower
  // assertions about the range of legal values for any given instruction.
}

bool tryParseAssemblyData(const char** text, struct AssemblyData* data, struct ParsingError* error) {
  *error = (struct ParsingError) {
    .message = "Not yet implemented.",
    .location = *text,
  };
  return false;
}

char* tryCopyLabel(const char** text) {
  return NULL;
}
