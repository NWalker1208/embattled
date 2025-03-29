#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "parser/parse.h"
#include "text_utils.h"

#pragma region Error messages

const char* INVALID_LABEL = "Invalid label";
const char* INVALID_OPCODE = "Invalid opcode";
const char* INVALID_PARAMETER = "Invalid parameter";
const char* INVALID_REGISTER = "Invalid register";
const char* INVALID_HEX_VALUE = "Invalid or out-of-range hexadecimal value";
const char* INVALID_INT_VALUE = "Invalid or out-of-range integer value";
const char* INVALID_HEX_BYTE = "Invalid hexadecimal byte";
const char* UNEXPECTED_CHARACTER = "Unexpected character";
const char* UNEXPECTED_END_OF_FILE = "Unexpected end of file";

#define PARSING_ERROR(_message, _start, _end) (ParsingError){.message = (_message), .sourceSpan = (TextSpan){.start = (_start), .end = (_end)}}

#pragma endregion

#pragma region Helper function signatures

// Parses an assembly instruction.
// If the current line is a valid instruction, advances position to the end of the current line,
// outputs the instruction through instruction, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseInstruction(const TextContents* text, TextOffset* position, AssemblyInstruction* instruction, ParsingError* error);

// Parses hexadecimal assembly data bytes.
// Expects position to be at the first hexadecimal byte (past the '.data' sequence).
// If text contains valid hexadecimal bytes from the current position to the end of the current line,
// advances position to the end of the line, outputs the data through data, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseAssemblyData(const TextContents* text, TextOffset* position, AssemblyData* data, ParsingError* error);

// Parses an assembly instruction parameter.
// If parsing succeeds, advances position past the end of the parameter,
// outputs the parameter through parameter, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseParameter(const TextContents* text, TextOffset* position, AssemblyParameter* parameter, ParsingError* error);

// Parses an opcode name.
// Expects text to be at the start of the opcode name.
// If parsing succeeds, advances position past the end of the opcode name,
// outputs the opcode through opcode, and returns true.
// If parsing fails, returns false.
bool tryParseOpcode(const TextContents* text, TextOffset* position, enum Opcode* opcode);

// Parses a register name.
// Expects text to be at the start of the register name (past the "$" character).
// If parsing succeeds, advances position past the end of the register name,
// outputs the register through reg and returns true.
// If parsing fails, returns false.
bool tryParseRegister(const TextContents* text, TextOffset* position, enum Register* reg);

// Parses a hexadecimal immediate value.
// Expects text to be at the first hexadecimal digit (pase the "0x" sequence).
// If parsing succeeds, advances position past the end of the hexadecimal digits,
// outputs the immediate value through result and returns true.
// If parsing fails, returns false.
bool tryParseImmediateHexValue(const TextContents* text, TextOffset* position, signed int* value);

// Parses a decimal immediate value.
// Expects text to be at the first digit, or at the sign character if one is present.
// If parsing succeeds, advances position past the end of the digits,
// outputs the immediate value through result and returns true.
// If parsing fails, returns false.
bool tryParseImmediateDecValue(const TextContents* text, TextOffset* position, signed int* value);

// Parses a name.
// If parsing succeeds, advances position past the end of the name,
// outputs the span of the name through span, and returns true.
// If parsing fails, returns false.
bool tryParseName(const TextContents* text, TextOffset* position, TextSpan* span);

#pragma endregion

bool TryParseAssemblyLine(const TextContents* text, TextOffset* position, AssemblyLine* line, ParsingError* error) {
  skipAllWhitespace(text);

  // Parse the label if one is present
  if (findCharOnLine(*text, ':') != NULL) {
    line->label = tryCopyLabel(text);
    if (line->label == NULL) {
      *error = PARSING_ERROR(INVALID_LABEL, *text);
      DestroyAssemblyLine(line);
      SKIP_TO_NEXT_LINE(position);
      return false; // Failed to parse label
    }
    skipInlineWhitespace(text);
    if (**text != ':') {
      *error = PARSING_ERROR(UNEXPECTED_CHARACTER, *text);
      DestroyAssemblyLine(line);
      SKIP_TO_NEXT_LINE(position);
      return false; // Expected ':' after label
    }
    (*text)++;
    skipAllWhitespace(text);
  }

  // Check for end of file
  if (**text == '\0') {
    *error = PARSING_ERROR(UNEXPECTED_END_OF_FILE, *text);
    DestroyAssemblyLine(line);
    return false;
  }

  // If line starts with ".data", parse as assembly data.
  // Otherwise, parse as an assembly instruction.
  if (startsWithWordCaseInsensitive(*text, ".data")) {
    (*text) += 5;
    skipInlineWhitespace(text);
    line->kind = DATA;
    if (!tryParseAssemblyData(text, &line->data, error)) {
      DestroyAssemblyLine(line);
      SKIP_TO_NEXT_LINE(position);
      return false; // Failed to parse assembly data
    }
  } else {
    line->kind = INSTRUCTION;
    if (!tryParseInstruction(text, &line->instruction, error)) {
      DestroyAssemblyLine(line);
      SKIP_TO_NEXT_LINE(position);
      return false; // Failed to parse assembly instruction
    }
  }
  assert(isEndOfLineOrFile(**text));

  // Advance to next line and return success.
  skipToNextLine(text);
  return true;
}

bool tryParseInstruction(const TextContents* text, TextOffset* position, AssemblyInstruction* instruction, ParsingError* error) {
  TextOffset endOfLine = GetTextContentsEndOfLine(text, position->line);

  // Parse the opcode
  TextOffset start = *position;
  if (!tryParseOpcode(text, position, &instruction->opcode)) {
    skipToNextWhitespace(text, position);
    *error = PARSING_ERROR(INVALID_OPCODE, start, *position);
    return false; // Failed to parse opcode
  }
  skipInlineWhitespace(text, position);

  // Parse the parameters, if present
  if (CompareTextOffsets(text, *position, endOfLine) < 0) {
    instruction->parameterCount = 0;
    instruction->parameters = NULL;
    while (true) {
      instruction->parameterCount++;
      instruction->parameters = realloc(instruction->parameters, instruction->parameterCount * sizeof(AssemblyParameter));
      AssemblyParameter* nextParam = &instruction->parameters[instruction->parameterCount - 1];
      *nextParam = (AssemblyParameter){ 0 }; // Initialize all fields to 0

      if (!tryParseParameter(text, position, nextParam, error)) {
        // Error already set by tryParseParameter
        return false; // Failed to parse parameter
      }
      skipInlineWhitespace(text, position);

      if (GetCharAtTextOffset(text, *position) == ',') {
        // If there's a comma, we expect to find another parameter
        IncrementTextOffset(text, position);
        skipInlineWhitespace(text, position);
      } else {
        // Otherwise, this was the last parameter
        break;
      }
    }

    // Check for end of line or file after parameter list
    if (CompareTextOffsets(text, *position, endOfLine) < 0) {
      assert(CompareTextOffsets(text, *position, endOfLine) == 0);
      TextOffset end = *position; IncrementTextOffset(text, &end);
      *error = PARSING_ERROR(UNEXPECTED_CHARACTER, *position, end);
      return false; // Expected end of line or file
    }
  }  

  return true;
}

bool tryParseOpcode(const TextContents* text, TextOffset* position, enum Opcode* opcode) {
  // Loop through the array of opcode definitions and output the first match
  for (int i = 0; i < OPCODE_COUNT; i++) {
    if (startsWithWordCaseInsensitive(text, *position, OPCODE_INFO[i].name)) {
      *opcode = (enum Opcode)i;
      position->column += strlen(OPCODE_INFO[i].name);
      NormalizeTextOffset(text, position);
      return true;
    }
  }
  return false;
}

bool tryParseParameter(const TextContents* text, TextOffset* position, AssemblyParameter* parameter, ParsingError* error) {
  TextOffset start = *position;
  char c = GetCharAtTextOffset(text, *position);
  switch (c) {
    case '$':  {
      // Parse as a register
      IncrementTextOffset(text, position);
      parameter->kind = ASSEMBLY_PARAM_REGISTER;
      if (!tryParseRegister(text, position, &parameter->registerName)) {
        skipToNextWhitespace(text, position);
        *error = PARSING_ERROR(INVALID_REGISTER, start, *position);
        return false; // Failed to parse register
      }
      break;
    }
    case '@': {
      // Parse as a label reference
      IncrementTextOffset(text, position);
      parameter->kind = ASSEMBLY_PARAM_LABEL;
      if (!tryParseName(text, position, &parameter->labelSpan)) {
        skipToNextWhitespace(text, position);
        *error = PARSING_ERROR(INVALID_REGISTER, start, *position);
        return false; // Failed to parse label
      }
      break;
    }
    default: {
      // Parse as an immediate value
      parameter->kind = ASSEMBLY_PARAM_IMMEDIATE;
      
      if (c == '0' && GetCharAtTextOffset(text, (TextOffset){position->line, position->column + 1}) == 'x') {
        // Parse as a hexadecimal immediate value
        position->column += 2;
        NormalizeTextOffset(text, position);
        if (!tryParseImmediateHexValue(text, position, &parameter->immediateValue)) {
          skipToNextWhitespace(text, position);
          *error = PARSING_ERROR(INVALID_HEX_VALUE, start, *position);
          return false; // Failed to parse hexadecimal immediate value
        }
      } else if (isdigit(c) || c == '-' || c == '+') {
        // Parse as a decimal immediate value
        if (!tryParseImmediateDecValue(text, position, &parameter->immediateValue)) {
          skipToNextWhitespace(text, position);
          *error = PARSING_ERROR(INVALID_INT_VALUE, start, *position);
          return false; // Failed to parse decimal immediate value
        }
      } else {
        skipToNextWhitespace(text, position);
        *error = PARSING_ERROR(INVALID_PARAMETER, start, *position);
        return false; // Failed to parse as any kind of parameter
      }
      break;
    }
  }
  parameter->sourceSpan = (TextSpan){start, *position};
  return true;
}

bool tryParseRegister(const TextContents* text, TextOffset* position, enum Register* reg) {
  // Loop through the array of register names and output the first match
  for (int i = 0; i < REGISTER_COUNT; i++) {
    if (startsWithWordCaseInsensitive(text, *position, REGISTER_NAMES[i])) {
      *reg = (enum Register)i;
      position->column += strlen(REGISTER_NAMES[i]);
      NormalizeTextOffset(text, position);
      return true;
    }
  }
  return false;
}

bool tryParseImmediateHexValue(const TextContents* text, TextOffset* position, signed int* value) {
  // Check that first character is valid hex
  unsigned char nibble;
  if (!tryHexToNibble(GetCharAtTextOffset(text, *position), &nibble)) {
    return false;
  }
  IncrementTextOffset(text, position);

  // Parse remaining hex characters
  unsigned int hexValue = nibble;
  while (tryHexToNibble(GetCharAtTextOffset(text, *position), &nibble)) {
    if (hexValue > ((unsigned int)UINT_MAX >> 4)) {
      return false; // Overflow
    }
    hexValue = (hexValue << 4) | (nibble & 0xF);
    IncrementTextOffset(text, position);
  }

  *value = (signed int)hexValue;
  return true;
}

bool tryParseImmediateDecValue(const TextContents* text, TextOffset* position, signed int* value) {
  // Check for sign character (- or +)
  char c = GetCharAtTextOffset(text, *position);
  bool isNegative = c == '-';
  if (isNegative || c == '+') {
    IncrementTextOffset(text, position);
  }

  // Check that first character after sign is valid digit
  if (!isdigit(c = GetCharAtTextOffset(text, *position))) {
    return false; // Not a decimal integer
  }
  unsigned char nextDigit = c - '0';
  IncrementTextOffset(text, position);

  // Parse remaining digits
  signed int decValue = isNegative ? -nextDigit : nextDigit;

  while (isdigit(c = GetCharAtTextOffset(text, *position))) {
    if ((isNegative && decValue < INT_MIN / 10) || (!isNegative && decValue > INT_MAX / 10)) {
      return false; // Overflow
    }
    decValue = decValue * 10;

    nextDigit = c - '0';
    if ((isNegative && decValue < INT_MIN + nextDigit) || (!isNegative && decValue > INT_MAX - nextDigit)) {
      return false; // Overflow
    }
    decValue += isNegative ? -nextDigit : nextDigit;
    IncrementTextOffset(text, position);
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

bool tryParseAssemblyData(const TextContents* text, TextOffset* position, AssemblyData* data, ParsingError* error) {
  data->length = 0;
  data->bytes = NULL;
  TextOffset endOfLine = GetTextContentsEndOfLine(text, position->line);
  while (CompareTextOffsets(text, *position, endOfLine) < 0) {
    TextOffset start = *position;

    // Parse the next hexadecimal byte
    unsigned char nibble;
    if (!tryHexToNibble(GetCharAtTextOffset(text, *position), &nibble)) {
      skipToNextWhitespace(text, position);
      *error = PARSING_ERROR(INVALID_HEX_BYTE, start, *position);
      return false;
    }
    IncrementTextOffset(text, position);
    unsigned char byte = nibble << 4;

    if (!tryHexToNibble(GetCharAtTextOffset(text, *position), &nibble)) {
      skipToNextWhitespace(text, position);
      *error = PARSING_ERROR(INVALID_HEX_BYTE, start, *position);
      return false;
    }
    IncrementTextOffset(text, position);
    byte |= nibble;

    data->length++;
    data->bytes = realloc(data->bytes, sizeof(unsigned char) * data->length);
    data->bytes[data->length - 1] = byte;

    skipInlineWhitespace(text, position);
  }

  return true;
}

bool tryParseName(const TextContents* text, TextOffset* position, TextSpan* span) {
  char c = GetCharAtTextOffset(text, *position);
  if (!isWordChar(c) || isdigit(c)) {
    return false;
  }

  TextOffset start = *position;
  size_t len = 1;
  while (isWordChar(GetCharAtTextOffset(text, *position))) {
    IncrementTextOffset(text, position);
  }

  *span = (TextSpan){ .start = start, .end = *position };
  return true;
}
