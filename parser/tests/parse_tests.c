#include <string.h>
#include <stdbool.h>
#include <unity.h>
#include "text_assertions.h"
#include "parser/parse.h"

TextContents text;
TextOffset position;
AssemblyLine line;
ParsingError error;

void setUp() {
  text = (TextContents){0};
  position = (TextOffset){0};
  line = (AssemblyLine){0};
  error = (ParsingError){0};
}

void tearDown() {
  DestroyTextContents(&text);
  DestroyAssemblyLine(&line);
}

#pragma region TryParseAssemblyLine

void test_TryParseAssemblyLine_should_succeedWithInstructionLine_when_lineIsValidInstruction(void) {
  // Arrange
  char source[] = "add $x0, @reference, 0x0FFFFFFFF, -02147483648, +2147483647, 02147483647\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &line, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(((TextOffset){0, sizeof(source) - 2}), position, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(ADD, line.instruction.opcode);
  TEST_ASSERT_EQUAL_size_t(6, line.instruction.parameterCount);

  TEST_ASSERT_EQUAL(ASSEMBLY_PARAM_REGISTER, line.instruction.parameters[0].kind);
  TEST_ASSERT_EQUAL(X0, line.instruction.parameters[0].registerName);

  TEST_ASSERT_EQUAL(ASSEMBLY_PARAM_LABEL, line.instruction.parameters[1].kind);
  TEST_ASSERT_EQUAL_TEXT_SPAN_CHARS("reference", line.instruction.parameters[1].labelSpan, text);

  TEST_ASSERT_EQUAL(ASSEMBLY_PARAM_IMMEDIATE, line.instruction.parameters[2].kind);
  TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, line.instruction.parameters[2].immediateValue);

  TEST_ASSERT_EQUAL(ASSEMBLY_PARAM_IMMEDIATE, line.instruction.parameters[3].kind);
  TEST_ASSERT_EQUAL_INT(-2147483648, line.instruction.parameters[3].immediateValue);

  TEST_ASSERT_EQUAL(ASSEMBLY_PARAM_IMMEDIATE, line.instruction.parameters[4].kind);
  TEST_ASSERT_EQUAL_INT(+2147483647, line.instruction.parameters[4].immediateValue);

  TEST_ASSERT_EQUAL(ASSEMBLY_PARAM_IMMEDIATE, line.instruction.parameters[5].kind);
  TEST_ASSERT_EQUAL_INT(2147483647, line.instruction.parameters[5].immediateValue);
}

void test_TryParseAssemblyLine_should_succeedWithDataLine_when_lineIsValidData(void) {
  // Arrange
  const char source[] = ".data 12 34 56 78 9A BC DE F0\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  const unsigned char expectedBytes[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_PTR(NULL, line.label);
  TEST_ASSERT_EQUAL(DATA, line.kind);
  TEST_ASSERT_EQUAL(8, line.data.length);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedBytes, line.data.bytes, sizeof(expectedBytes));
}

void test_TryParseAssemblyLine_should_succeedWithLabel_when_textStartsWithLabel(void) {
  // Arrange
  const char source[] = "label:\n  nop\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING("label", line.label);
  TEST_ASSERT_EQUAL(INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(NOP, line.instruction.opcode);
  TEST_ASSERT_EQUAL(0, line.instruction.parameterCount);
  TEST_ASSERT_EQUAL_PTR(NULL, line.instruction.parameters);
}

void test_TryParseAssemblyLine_should_succeedAndAdvanceToEndOfFile_when_lastLine(void) {
  // Arrange
  const char source[] = "nop";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_PTR(NULL, line.label);
  TEST_ASSERT_EQUAL(INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(NOP, line.instruction.opcode);
  TEST_ASSERT_EQUAL(0, line.instruction.parameterCount);
  TEST_ASSERT_EQUAL_PTR(NULL, line.instruction.parameters);
}

void test_TryParseAssemblyLine_should_failWithInvalidLabel_when_labelIsEmpty(void) {
  // Arrange
  const char source[] = ":\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_LABEL, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidLabel_when_noValidLabelCharacters(void) {
  // Arrange
  const char source[] = "~:\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_LABEL, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidOpcode_when_firstWordIsNotValidOpcode(void) {
  // Arrange
  const char source[] = "nopbutnotreally\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_OPCODE, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidParameter_when_parameterIsNotRegisterOrHexValueOrReferenceOrInteger(void) {
  // Arrange
  const char source[] = "mov $x0, invalid\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_PARAMETER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[9], error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidParameter_when_lineEndsAfterComma(void) {
  // Arrange
  const char source[] = "mov $x0, \n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_PARAMETER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[9], error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidRegister_when_dollarSignFollowedByInvalidRegisterName(void) {
  // Arrange
  const char source[] = "mov $invalid, 123\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_REGISTER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[5], error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidHexValue_when_zeroExFollowedByInvalidHexDigit(void) {
  // Arrange
  const char source[] = "mov $x0, 0xg\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_HEX_VALUE, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[11], error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidHexValue_when_hexParameterIsGreaterThanMaxUnsignedInteger(void) {
  // Arrange
  const char source[] = "jmp 0x100000000\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_HEX_VALUE, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[6], error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidIntValue_when_integerParameterIsGreaterThanMaxSignedInteger(void) {
  // Arrange
  const char source[] = "jmp 2147483648\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_INT_VALUE, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[4], error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidIntValue_when_integerParameterIsLessThanMinSignedInteger(void) {
  // Arrange
  const char source[] = "jmp -2147483649\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_INT_VALUE, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[4], error.location);
}

void test_TryParseAssemblyLine_should_failWithInvalidByte_when_dataLineContainsInvalidByte(void) {
  // Arrange
  const char source[] = ".data FF b\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_HEX_BYTE, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[10], error.location);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedCharacter_when_invalidCharacterAfterValidParameter(void) {
  // Arrange
  const char source[] = "mov $x0, 123~\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[12], error.location);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedCharacter_when_missingCommaBetweenParameters(void) {
  // Arrange
  const char source[] = "mov $x0 123\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[8], error.location);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedCharacter_when_invalidCharacterAfterLabel(void) {
  // Arrange
  const char source[] = "test~:\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[4], error.location);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedEndOfFile_when_noMoreText(void) {
  // Arrange
  const char source[] = "\n";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_END_OF_FILE, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[1], error.location);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedEndOfFile_when_atEndOfFile(void) {
  // Arrange
  const char source[] = "";

  // Act
  const char* textPtr = source;
  bool success = TryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_END_OF_FILE, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}

#pragma endregion
