#include <string.h>
#include <stdbool.h>
#include <unity.h>
#include "parser/parse.h"

struct AssemblyLine line;
struct ParsingError error;

void setUp() {
  memset(&line, 0, sizeof(line));
  memset(&error, 0, sizeof(error));
}

void tearDown() {
  destroyAssemblyLine(&line);
}

void test_tryParseAssemblyLine_should_succeedWithInstructionLine_when_lineIsValidInstruction(void) {
  // Arrange
  const char source[] = "add $x0, @reference, 0x0FFFFFFFF, -02147483648, +2147483647, 02147483647\r\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_PTR(NULL, line.label);
  TEST_ASSERT_EQUAL(INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(ADD, line.instruction.opcode);
  TEST_ASSERT_EQUAL(6, line.instruction.parameterCount);

  TEST_ASSERT_EQUAL(REGISTER, line.instruction.parameters[0].kind);
  TEST_ASSERT_EQUAL(X0, line.instruction.parameters[0].registerName);

  TEST_ASSERT_EQUAL(LABEL_REFERENCE, line.instruction.parameters[1].kind);
  TEST_ASSERT_EQUAL_STRING("reference", line.instruction.parameters[1].referencedLabel);

  TEST_ASSERT_EQUAL(IMMEDIATE_VALUE, line.instruction.parameters[2].kind);
  TEST_ASSERT_EQUAL(0x1234, line.instruction.parameters[2].immediateValue);

  TEST_ASSERT_EQUAL(IMMEDIATE_VALUE, line.instruction.parameters[3].kind);
  TEST_ASSERT_EQUAL(-5, line.instruction.parameters[3].immediateValue);

  TEST_ASSERT_EQUAL(IMMEDIATE_VALUE, line.instruction.parameters[4].kind);
  TEST_ASSERT_EQUAL(+7, line.instruction.parameters[4].immediateValue);

  TEST_ASSERT_EQUAL(IMMEDIATE_VALUE, line.instruction.parameters[5].kind);
  TEST_ASSERT_EQUAL(42, line.instruction.parameters[5].immediateValue);
}

void test_tryParseAssemblyLine_should_succeedWithDataLine_when_lineIsValidData(void) {
  // Arrange
  const char source[] = ".data 12 34 56 78 9A BC DE F0\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  const unsigned char expectedBytes[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_PTR(NULL, line.label);
  TEST_ASSERT_EQUAL(DATA, line.kind);
  TEST_ASSERT_EQUAL(10, line.data.length);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedBytes, line.data.bytes, sizeof(expectedBytes));
}

void test_tryParseAssemblyLine_should_succeedWithLabel_when_textStartsWithLabel(void) {
  // Arrange
  const char source[] = "label:\n  nop\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING("label", line.label);
  TEST_ASSERT_EQUAL(INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(NOP, line.instruction.opcode);
  TEST_ASSERT_EQUAL(0, line.instruction.parameterCount);
  TEST_ASSERT_EQUAL_PTR(NULL, line.instruction.parameters);
}

void test_tryParseAssemblyLine_should_succeedAndAdvanceToEndOfFile_when_lastLine(void) {
  // Arrange
  const char source[] = "nop";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING("label", line.label);
  TEST_ASSERT_EQUAL(INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(NOP, line.instruction.opcode);
  TEST_ASSERT_EQUAL(0, line.instruction.parameterCount);
  TEST_ASSERT_EQUAL_PTR(NULL, line.instruction.parameters);
}

void test_tryParseAssemblyLine_should_failWithInvalidLabel_when_labelIsEmpty(void) {
  // Arrange
  const char source[] = ":\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_LABEL, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidLabel_when_noValidLabelCharacters(void) {
  // Arrange
  const char source[] = "~:\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_LABEL, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidOpcode_when_firstWordIsNotValidOpcode(void) {
  // Arrange
  const char source[] = "nopbutnotreally\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_OPCODE, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidParameter_when_parameterIsNotRegisterOrHexValueOrReferenceOrInteger(void) {
  // Arrange
  const char source[] = "mov $x0, invalid\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_PARAMETER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[9], error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidParameter_when_hexParameterIsGreaterThanMaxUnsignedInteger(void) {
  // Arrange
  const char source[] = "jmp 0x100000000\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_PARAMETER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[4], error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidParameter_when_decimalParameterIsGreaterThanMaxSignedInteger(void) {
  // Arrange
  const char source[] = "jmp 2147483648\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_PARAMETER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[4], error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidParameter_when_decimalParameterIsLessThanMinSignedInteger(void) {
  // Arrange
  const char source[] = "jmp -2147483649\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_PARAMETER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[4], error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidParameter_when_lineEndsAfterComma(void) {
  // Arrange
  const char source[] = "mov $x0, \n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_PARAMETER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[9], error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidRegister_when_dollarSignFollowedByInvalidRegisterName(void) {
  // Arrange
  const char source[] = "mov $invalid, 123\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_REGISTER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[5], error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidHexValue_when_zeroExFollowedByInvalidHexDigit(void) {
  // Arrange
  const char source[] = "mov $x0, 0xg\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_HEX_VALUE, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[11], error.location);
}

void test_tryParseAssemblyLine_should_failWithInvalidByte_when_dataLineContainsInvalidByte(void) {
  // Arrange
  const char source[] = ".data FF b\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(INVALID_BYTE, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[9], error.location);
}

void test_tryParseAssemblyLine_should_failWithUnexpectedCharacter_when_invalidCharacterAfterValidParameter(void) {
  // Arrange
  const char source[] = "mov $x0, 123~\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[12], error.location);
}

void test_tryParseAssemblyLine_should_failWithUnexpectedCharacter_when_missingCommaBetweenParameters(void) {
  // Arrange
  const char source[] = "mov $x0 123\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[8], error.location);
}

void test_tryParseAssemblyLine_should_failWithUnexpectedCharacter_when_invalidCharacterAfterLabel(void) {
  // Arrange
  const char source[] = "test~:\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[4], error.location);
}

void test_tryParseAssemblyLine_should_failWithUnexpectedEndOfFile_when_noMoreText(void) {
  // Arrange
  const char source[] = "\n";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_END_OF_FILE, error.message);
  TEST_ASSERT_EQUAL_PTR(&source[1], error.location);
}

void test_tryParseAssemblyLine_should_failWithUnexpectedEndOfFile_when_atEndOfFile(void) {
  // Arrange
  const char source[] = "";

  // Act
  const char* textPtr = source;
  bool success = tryParseAssemblyLine(&textPtr, &line, &error);

  // Assert
  const char* expectedTextPtr = &source[sizeof(source) - 1];
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR(expectedTextPtr, textPtr);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_END_OF_FILE, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}
