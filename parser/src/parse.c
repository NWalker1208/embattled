#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "parser/parse.h"
#include "text_utils.h"

#pragma region Error messages

const char INVALID_LABEL_NAME[] = "Invalid label name";
const char INVALID_LABEL_ADDR[] = "Invalid address";
const char UNRECOGNIZED_MNEMONIC[] = "Unrecognized mnemonic";
const char INVALID_OPERAND[] = "Invalid operand";
const char TOO_MANY_OPERANDS[] = "Too many operands";
const char INVALID_REGISTER[] = "Invalid register";
const char INVALID_HEX_VALUE[] = "Invalid or out-of-range hexadecimal value";
const char INVALID_INT_VALUE[] = "Invalid or out-of-range integer value";
const char INVALID_HEX_BYTE[] = "Invalid hexadecimal byte";
const char UNEXPECTED_CHARACTER[] = "Unexpected character";
const char UNEXPECTED_END_OF_FILE[] = "Unexpected end of file";

#define PARSING_ERROR(_message, _start, _end) (ParsingError){.message = (_message), .sourceSpan = (TextSpan){.start = (_start), .end = (_end)}}

#pragma endregion

#pragma region Helper function signatures

// Returns true if the position is at the start of a comment.
bool isPositionStartOfComment(const TextContents* text, TextOffset position);

// Advances position to the next non-whitespace character that isn't part of a comment.
void skipAllWhitespaceAndComments(const TextContents* text, TextOffset* position);

// Parses an assembly label.
// If the current line is a valid label, advances position past the ':' character,
// outputs the label through label, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseLabel(const TextContents* text, TextOffset* position, AssemblyLabel* label, ParsingError* error);

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
bool tryParseAssemblyData(const TextContents* text, TextOffset* position, uint8_t** dataBuffer, size_t* dataBufferSize, AssemblyData* data, ParsingError* error);

// Parses an assembly mnemonic.
// Expects text to be at the start of the mnemonic.
// If parsing succeeds, advances position past the end of the mnemonic,
// outputs the mnemonic through mnemonic, and returns true.
// If parsing fails, returns false.
bool tryParseMnemonic(const TextContents* text, TextOffset* position, AssemblyMnemonic* mnemonic);

// Parses an assembly instruction operand.
// If parsing succeeds, advances position past the end of the operand,
// outputs the operand through operand, and returns true.
// If parsing fails, outputs the cause through error and returns false.
bool tryParseOperand(const TextContents* text, TextOffset* position, AssemblyOperand* operand, ParsingError* error);

// Parses a register name.
// Expects text to be at the start of the register name (past the "$" character).
// If parsing succeeds, advances position past the end of the register name,
// outputs the register through reg and returns true.
// If parsing fails, returns false.
bool tryParseRegister(const TextContents* text, TextOffset* position, Register* reg);

// Parses a hexadecimal immediate value.
// Expects text to be at the first hexadecimal digit (past the "0x" sequence).
// If parsing succeeds, advances position past the end of the hexadecimal digits,
// outputs the immediate value through result and returns true.
// If parsing fails, returns false.
bool tryParseHexadecimalValue(const TextContents* text, TextOffset* position, uint32_t* value);

// Parses a decimal immediate value.
// Expects text to be at the first digit, or at the sign character if one is present.
// If parsing succeeds, advances position past the end of the digits,
// outputs the immediate value through result and returns true.
// If parsing fails, returns false.
bool tryParseDecimalValue(const TextContents* text, TextOffset* position, int32_t* value);

// Parses a name.
// If parsing succeeds, advances position past the end of the name,
// outputs the span of the name through span, and returns true.
// If parsing fails, returns false.
bool tryParseName(const TextContents* text, TextOffset* position, TextSpan* span);

#pragma endregion

bool TryParseAssemblyProgram(const TextContents* text, AssemblyProgram* program, ParsingErrorList* errors) {
  TextOffset position = { 0, 0 };
  TextOffset end = GetTextContentsEnd(text);
  
  bool anyErrors = false;
  program->lines = NULL;
  program->lineCount = 0;
  program->dataBuffer = NULL;
  size_t dataBufferSize = 0;

  TextOffset previous = end;
  while (CompareTextOffsets(text, position, end) < 0) {
    assert(CompareTextOffsets(text, position, previous) != 0);
    previous = position;

    skipAllWhitespaceAndComments(text, &position);
    if (CompareTextOffsets(text, position, end) >= 0) { break; }

    AssemblyLine currentLine = { 0 };
    ParsingError currentError = { 0 };
    if (TryParseAssemblyLine(text, &position, &program->dataBuffer, &dataBufferSize, &currentLine, &currentError)) {
      program->lines = realloc(program->lines, sizeof(AssemblyLine) * (program->lineCount + 1));
      program->lines[program->lineCount] = currentLine;
      program->lineCount++;
    } else {
      anyErrors = true;
      if (errors->errorCount == MAX_ERRORS) {
        errors->moreErrors = true;
      } else {
        errors->errors[errors->errorCount] = currentError;
        errors->errorCount++;
      }
    }
  }
  
  return !anyErrors;
}

bool TryParseAssemblyLine(const TextContents* text, TextOffset* position, uint8_t** dataBuffer, size_t* dataBufferSize, AssemblyLine* line, ParsingError* error) {
  skipAllWhitespace(text, position);
  TextOffset start = *position;

  // Check for end-of-file
  if (CompareTextOffsets(text, *position, GetTextContentsEnd(text)) >= 0) {
    *error = PARSING_ERROR(UNEXPECTED_END_OF_FILE, *position, *position);
    return false;
  }

  // If the line contains a ':', parse as a label.
  if (lineContainsChar(text, *position, ':')) {
    line->kind = ASSEMBLY_LINE_LABEL;
    if (!tryParseLabel(text, position, &line->label, error)) {
      // Error already set by tryParseLabel
      skipToEndOfLine(text, position);
      return false; // Failed to parse assembly instruction
    }

  // If the line starts with ".data", parse as assembly data.
  } else if (startsWithWordCaseInsensitive(text, *position, ".data")) {
    position->column += 5;
    NormalizeTextOffset(text, position);
    skipInlineWhitespace(text, position);

    line->kind = ASSEMBLY_LINE_DATA;
    if (!tryParseAssemblyData(text, position, dataBuffer, dataBufferSize, &line->data, error)) {
      // Error already set by tryParseAssemblyData
      skipToEndOfLine(text, position);
      return false; // Failed to parse assembly data
    }

  // Otherwise, parse as an assembly instruction.
  } else {
    line->kind = ASSEMBLY_LINE_INSTRUCTION;
    if (!tryParseInstruction(text, position, &line->instruction, error)) {
      // Error already set by tryParseInstruction
      skipToEndOfLine(text, position);
      return false; // Failed to parse assembly instruction
    }
  }
  assert(line->kind == ASSEMBLY_LINE_LABEL ||
         CompareTextOffsets(text, *position, GetTextContentsEndOfLine(text, start)) == 0 ||
         isPositionStartOfComment(text, *position));

  line->sourceSpan = (TextSpan){start, *position};
  return true;
}

bool isPositionStartOfComment(const TextContents* text, TextOffset position) {
  return GetCharAtTextOffset(text, position) == ';';
}

void skipAllWhitespaceAndComments(const TextContents* text, TextOffset* position) {
  while (true) {
    skipAllWhitespace(text, position);
    if (isPositionStartOfComment(text, *position)) {
      skipToEndOfLine(text, position);
    } else {
      break;
    }
  }
}

bool isWhiteSpaceOrColon(char c) { return isAnyWhitespace(c) || c == ':'; }
bool isWhiteSpaceOrAtOrColon(char c) { return isWhiteSpaceOrColon(c) || c == '@'; }

bool tryParseLabel(const TextContents* text, TextOffset* position, AssemblyLabel* label, ParsingError* error) {
  if (GetCharAtTextOffset(text, *position) != '@') {
    TextOffset start = *position;
    if (!tryParseName(text, position, &label->nameSpan)) {
      skipToNextSatisfies(text, position, isWhiteSpaceOrAtOrColon);
      *error = PARSING_ERROR(INVALID_LABEL_NAME, start, *position);
      return false; // Failed to parse label name
    }
  } else {
    label->nameSpan = (TextSpan){*position, *position};
  }

  if (GetCharAtTextOffset(text, *position) == '@') {
    IncrementTextOffset(text, position);

    TextOffset start = *position;
    unsigned int hexValue;
    if (!tryParseHexadecimalValue(text, position, &hexValue) || hexValue > 0xFFFF) {
      skipToNextSatisfies(text, position, isWhiteSpaceOrColon);
      *error = PARSING_ERROR(INVALID_LABEL_ADDR, start, *position);
      return false; // Failed to parse label address
    }
    label->address = (unsigned short)hexValue;
    label->addressSpan = (TextSpan){start, *position};
  } else {
    label->addressSpan = (TextSpan){*position, *position};
    label->address = 0;
  }

  if (GetCharAtTextOffset(text, *position) != ':') {
    TextOffset end = *position; IncrementTextOffset(text, &end);
    *error = PARSING_ERROR(UNEXPECTED_CHARACTER, *position, end);
    return false; // Unexpected character in label
  }

  IncrementTextOffset(text, position);
  //skipInlineWhitespace(text, position);

  return true;
}

bool tryParseInstruction(const TextContents* text, TextOffset* position, AssemblyInstruction* instruction, ParsingError* error) {
  TextOffset endOfLine = GetTextContentsEndOfLine(text, *position);

  // Parse the opcode
  TextOffset start = *position;
  if (!tryParseMnemonic(text, position, &instruction->mnemonic)) {
    skipToNextWhitespace(text, position);
    *error = PARSING_ERROR(UNRECOGNIZED_MNEMONIC, start, *position);
    return false; // Failed to parse opcode
  }
  skipInlineWhitespace(text, position);

  // Parse the parameters, if present
  if (CompareTextOffsets(text, *position, endOfLine) < 0) {
    instruction->operandCount = 0;
    while (true) {
      AssemblyOperand operand = { 0 };
      if (!tryParseOperand(text, position, &operand, error)) {
        // Error already set by tryParseOperand
        return false; // Failed to parse operand
      }
      
      if (instruction->operandCount >= MAX_ASSEMBLY_OPERANDS) {
        *error = PARSING_ERROR(TOO_MANY_OPERANDS, operand.sourceSpan.start, operand.sourceSpan.end);
        return false;
      }
      
      instruction->operands[instruction->operandCount] = operand;
      instruction->operandCount++;
      
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

    // Check for comment or end of line/file after parameter list
    if (CompareTextOffsets(text, *position, endOfLine) < 0 && !isPositionStartOfComment(text, *position)) {
      TextOffset end = *position; IncrementTextOffset(text, &end);
      *error = PARSING_ERROR(UNEXPECTED_CHARACTER, *position, end);
      return false; // Expected end of line or file
    }

    assert(CompareTextOffsets(text, *position, endOfLine) == 0 || isPositionStartOfComment(text, *position));
  }  

  return true;
}

bool tryParseMnemonic(const TextContents* text, TextOffset* position, AssemblyMnemonic* mnemonic) {
  // Loop through the array of opcode definitions and output the first match
  for (AssemblyMnemonic m = 0; m < ASSEMBLY_MNEMONIC_COUNT; m++) {
    const char* identifier = getAssemblyMnemonicInfo(m)->identifier;
    if (startsWithWordCaseInsensitive(text, *position, identifier)) {
      *mnemonic = m;
      position->column += strlen(identifier);
      NormalizeTextOffset(text, position);
      return true;
    }
  }
  return false;
}

bool isPastParameter(char c) { return isAnyWhitespace(c) || c == ',' || c == ';'; }

bool tryParseOperand(const TextContents* text, TextOffset* position, AssemblyOperand* operand, ParsingError* error) {
  TextOffset start = *position;
  char c = GetCharAtTextOffset(text, *position);
  switch (c) {
    case '$':  {
      // Parse as a register
      IncrementTextOffset(text, position);
      operand->kind = ASSEMBLY_OPERAND_REGISTER;
      if (!tryParseRegister(text, position, &operand->registerName)) {
        skipToNextSatisfies(text, position, isPastParameter);
        *error = PARSING_ERROR(INVALID_REGISTER, start, *position);
        return false; // Failed to parse register
      }
      break;
    }
    case '@': {
      // Parse as a label reference
      IncrementTextOffset(text, position);
      operand->kind = ASSEMBLY_OPERAND_LABEL;
      if (!tryParseName(text, position, &operand->labelSpan)) {
        skipToNextSatisfies(text, position, isPastParameter);
        *error = PARSING_ERROR(INVALID_REGISTER, start, *position);
        return false; // Failed to parse label
      }
      break;
    }
    default: {
      // Parse as an immediate value
      operand->kind = ASSEMBLY_OPERAND_IMMEDIATE;
      
      if (c == '0' && GetCharAtTextOffset(text, (TextOffset){position->line, position->column + 1}) == 'x') {
        // Parse as a hexadecimal immediate value
        position->column += 2;
        NormalizeTextOffset(text, position);
        uint32_t hexValue;
        if (!tryParseHexadecimalValue(text, position, &hexValue)) {
          skipToNextSatisfies(text, position, isPastParameter);
          *error = PARSING_ERROR(INVALID_HEX_VALUE, start, *position);
          return false; // Failed to parse hexadecimal immediate value
        }
        operand->immediateValue = (int32_t)hexValue;
      } else if (isdigit(c) || c == '-' || c == '+') {
        // Parse as a decimal immediate value
        if (!tryParseDecimalValue(text, position, &operand->immediateValue)) {
          skipToNextSatisfies(text, position, isPastParameter);
          *error = PARSING_ERROR(INVALID_INT_VALUE, start, *position);
          return false; // Failed to parse decimal immediate value
        }
      } else {
        skipToNextSatisfies(text, position, isPastParameter);
        *error = PARSING_ERROR(INVALID_OPERAND, start, *position);
        return false; // Failed to parse as any kind of parameter
      }
      break;
    }
  }
  operand->sourceSpan = (TextSpan){start, *position};
  return true;
}

bool tryParseRegister(const TextContents* text, TextOffset* position, Register* reg) {
  // Loop through the array of register names and output the first match
  for (Register r = 0; r < REGISTER_COUNT; r++) {
    const char* identifier = getRegisterIdentifier(r);
    if (startsWithWordCaseInsensitive(text, *position, identifier)) {
      *reg = r;
      position->column += strlen(identifier);
      NormalizeTextOffset(text, position);
      return true;
    }
  }
  return false;
}

bool tryParseHexadecimalValue(const TextContents* text, TextOffset* position, uint32_t* value) {
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

  *value = hexValue;
  return true;
}

bool tryParseDecimalValue(const TextContents* text, TextOffset* position, int32_t* value) {
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

bool tryParseAssemblyData(const TextContents* text, TextOffset* position, uint8_t** dataBuffer, size_t* dataBufferSize, AssemblyData* data, ParsingError* error) {
  uint8_t* tempBuffer = NULL;
  size_t tempBufferSize = 0;
  
  TextOffset endOfLine = GetTextContentsEndOfLine(text, *position);
  while (CompareTextOffsets(text, *position, endOfLine) < 0 && !isPositionStartOfComment(text, *position)) {
    TextOffset start = *position;

    // Parse the next hexadecimal byte
    unsigned char nibble;
    if (!tryHexToNibble(GetCharAtTextOffset(text, *position), &nibble)) {
      skipToNextWhitespace(text, position);
      *error = PARSING_ERROR(INVALID_HEX_BYTE, start, *position);
      free(tempBuffer);
      return false;
    }
    IncrementTextOffset(text, position);
    unsigned char byte = nibble << 4;

    if (!tryHexToNibble(GetCharAtTextOffset(text, *position), &nibble)) {
      skipToNextWhitespace(text, position);
      *error = PARSING_ERROR(INVALID_HEX_BYTE, start, *position);
      free(tempBuffer);
      return false;
    }
    IncrementTextOffset(text, position);
    byte |= nibble;

    tempBufferSize++;
    tempBuffer = realloc(tempBuffer, sizeof(uint8_t) * tempBufferSize);
    tempBuffer[tempBufferSize - 1] = byte;

    skipInlineWhitespace(text, position);
  }

  data->startIndex = *dataBufferSize;
  data->byteCount = tempBufferSize;

  *dataBufferSize += tempBufferSize;
  *dataBuffer = realloc(*dataBuffer, sizeof(uint8_t) * *dataBufferSize);
  memcpy(&(*dataBuffer)[data->startIndex], tempBuffer, sizeof(uint8_t) * tempBufferSize);

  free(tempBuffer);

  return true;
}

bool tryParseName(const TextContents* text, TextOffset* position, TextSpan* span) {
  char c = GetCharAtTextOffset(text, *position);
  if (!isWordChar(c) || isdigit(c)) {
    return false;
  }

  TextOffset start = *position;
  while (isWordChar(GetCharAtTextOffset(text, *position))) {
    IncrementTextOffset(text, position);
  }

  *span = (TextSpan){ .start = start, .end = *position };
  return true;
}
