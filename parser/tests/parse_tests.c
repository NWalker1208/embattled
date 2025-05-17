#define UNITY_SUPPORT_TEST_CASES
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unity.h>
#include "test_utilities/text_assertions.h"
#include "parser/parse.h"

TextContents text;
TextOffset position;
uint8_t* dataBuffer;
size_t dataBufferSize;
AssemblyLine line;
ParsingError error;

void setUp() {
  text = (TextContents){0};
  position = (TextOffset){0};
  dataBuffer = NULL;
  dataBufferSize = 0;
  line = (AssemblyLine){0};
  error = (ParsingError){0};
}

void tearDown() {
  DestroyTextContents(&text);
  free(dataBuffer);
  dataBuffer = NULL;
}

#pragma region TryParseAssemblyLine

void test_TryParseAssemblyLine_should_succeedWithInstructionLine_when_lineIsValidInstruction(void) {
  // Arrange
  const char source[] = "add $x0, @reference, 1234\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedLineSpan = { {0, 0}, {0, 25} };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedLineSpan, line.sourceSpan, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(ASSEMBLY_MNEMONIC_ADD, line.instruction.mnemonic);
  TEST_ASSERT_EQUAL_size_t(MAX_ASSEMBLY_OPERANDS, line.instruction.operandCount);

  TEST_ASSERT_EQUAL(ASSEMBLY_OPERAND_REGISTER, line.instruction.operands[0].kind);
  TEST_ASSERT_EQUAL(REGISTER_X0, line.instruction.operands[0].registerName);

  TEST_ASSERT_EQUAL(ASSEMBLY_OPERAND_LABEL, line.instruction.operands[1].kind);
  TEST_ASSERT_EQUAL_TEXT_SPAN_CHARS("reference", line.instruction.operands[1].labelSpan, text);

  TEST_ASSERT_EQUAL(ASSEMBLY_OPERAND_IMMEDIATE, line.instruction.operands[2].kind);
  TEST_ASSERT_EQUAL_INT32(1234, line.instruction.operands[2].immediateValue);
}

TEST_CASE("add 0x0FFFFFFFF", 0xFFFFFFFF)
TEST_CASE("add -02147483648", -2147483648)
TEST_CASE("add +2147483647", +2147483647)
TEST_CASE("add 02147483647", 2147483647)
void test_TryParseAssemblyLine_should_succeedWithExpectedImmediateValue_when_lineIsValidInstructionWithImmediateOperand(const char* source, int32_t expectedValue) {
  // Arrange
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(ASSEMBLY_MNEMONIC_ADD, line.instruction.mnemonic);
  TEST_ASSERT_EQUAL_size_t(1, line.instruction.operandCount);

  TEST_ASSERT_EQUAL(ASSEMBLY_OPERAND_IMMEDIATE, line.instruction.operands[0].kind);
  TEST_ASSERT_EQUAL_INT32(expectedValue, line.instruction.operands[0].immediateValue);
}

void test_TryParseAssemblyLine_should_succeedWithInstructionLine_when_lineIsValidInstructionWithComment(void) {
  // Arrange
  const char source[] = "nop 0; comment\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, 5 };
  const TextSpan expectedLineSpan = { {0, 0}, {0, 5} };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedLineSpan, line.sourceSpan, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(ASSEMBLY_MNEMONIC_NOP, line.instruction.mnemonic);
  TEST_ASSERT_EQUAL_size_t(1, line.instruction.operandCount);

  TEST_ASSERT_EQUAL(ASSEMBLY_OPERAND_IMMEDIATE, line.instruction.operands[0].kind);
  TEST_ASSERT_EQUAL_HEX32(0, line.instruction.operands[0].immediateValue);
}

void test_TryParseAssemblyLine_should_succeedWithDataLine_when_lineIsValidData(void) {
  // Arrange
  const char source[] = ".data 12 34 56 78 9A BC DE F0\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedLineSpan = { {0, 0}, {0, 29} };
  const unsigned char expectedBytes[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedLineSpan, line.sourceSpan, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_DATA, line.kind);
  TEST_ASSERT_EQUAL_size_t(0, line.data.startIndex);
  TEST_ASSERT_EQUAL_size_t(sizeof(expectedBytes), line.data.byteCount);
  TEST_ASSERT_EQUAL_size_t(sizeof(expectedBytes), dataBufferSize);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedBytes, dataBuffer, sizeof(expectedBytes));
}

void test_TryParseAssemblyLine_should_succeedWithDataLine_when_lineIsValidDataWithComment(void) {
  // Arrange
  const char source[] = ".data 12; comment\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, 8 };
  const TextSpan expectedLineSpan = { {0, 0}, {0, 8} };
  const unsigned char expectedBytes[] = { 0x12 };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedLineSpan, line.sourceSpan, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_DATA, line.kind);
  TEST_ASSERT_EQUAL_size_t(0, line.data.startIndex);
  TEST_ASSERT_EQUAL_size_t(sizeof(expectedBytes), line.data.byteCount);
  TEST_ASSERT_EQUAL_size_t(sizeof(expectedBytes), dataBufferSize);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedBytes, dataBuffer, sizeof(expectedBytes));
}

void test_TryParseAssemblyLine_should_succeedWithLabelLine_when_lineIsValidLabelWithName(void) {
  // Arrange
  const char source[] = "label:\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedLineSpan = { {0, 0}, {0, 6} };
  const TextSpan expectedNameSpan = { {0, 0}, {0, 5} };
  const TextSpan expectedAddressSpan = { {0, 5}, {0, 5} };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedLineSpan, line.sourceSpan, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_LABEL, line.kind);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedNameSpan, line.label.nameSpan, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedAddressSpan, line.label.addressSpan, text);
}

void test_TryParseAssemblyLine_should_succeedWithLabelLine_when_lineIsValidLabelWithNameAndComment(void) {
  // Arrange
  const char source[] = "label: ; comment\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, 6 };
  const TextSpan expectedLineSpan = { {0, 0}, {0, 6} };
  const TextSpan expectedNameSpan = { {0, 0}, {0, 5} };
  const TextSpan expectedAddressSpan = { {0, 5}, {0, 5} };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedLineSpan, line.sourceSpan, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_LABEL, line.kind);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedNameSpan, line.label.nameSpan, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedAddressSpan, line.label.addressSpan, text);
}

void test_TryParseAssemblyLine_should_succeedWithLabelLine_when_lineIsValidLabelWithAddress(void) {
  // Arrange
  const char source[] = "@1234:\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedLineSpan = { {0, 0}, {0, 6} };
  const TextSpan expectedNameSpan = { {0, 0}, {0, 0} };
  const TextSpan expectedAddressSpan = { {0, 1}, {0, 5} };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedLineSpan, line.sourceSpan, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_LABEL, line.kind);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedNameSpan, line.label.nameSpan, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedAddressSpan, line.label.addressSpan, text);
  TEST_ASSERT_EQUAL_UINT16(0x1234, line.label.address);
}

void test_TryParseAssemblyLine_should_succeedWithLabelLine_when_lineIsValidLabelWithNameAndAddress(void) {
  // Arrange
  const char source[] = "label@1234:\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedLineSpan = { {0, 0}, {0, 11} };
  const TextSpan expectedNameSpan = { {0, 0}, {0, 5} };
  const TextSpan expectedAddressSpan = { {0, 6}, {0, 10} };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedLineSpan, line.sourceSpan, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_LABEL, line.kind);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedNameSpan, line.label.nameSpan, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedAddressSpan, line.label.addressSpan, text);
  TEST_ASSERT_EQUAL_UINT16(0x1234, line.label.address);
}

void test_TryParseAssemblyLine_should_succeedAndAdvanceToEndOfFile_when_lastLine(void) {
  // Arrange
  const char source[] = "nop";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 1 };
  const TextSpan expectedLineSpan = { {0, 0}, {0, 3} };
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedLineSpan, line.sourceSpan, text);
  TEST_ASSERT_EQUAL(ASSEMBLY_LINE_INSTRUCTION, line.kind);
  TEST_ASSERT_EQUAL(ASSEMBLY_MNEMONIC_NOP, line.instruction.mnemonic);
  TEST_ASSERT_EQUAL(0, line.instruction.operandCount);
}

void test_TryParseAssemblyLine_should_failWithTooManyOperands_when_instructionHasMoreThanMaxAssemblyOperands(void) {
  // Arrange
  const char source[] = "add $x0, @reference, 1234, 0xFF\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 27}, {0, 31} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(TOO_MANY_OPERANDS, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidLabelName_when_labelNameIsEmpty(void) {
  // Arrange
  const char source[] = ":\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 0}, {0, 0} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_LABEL_NAME, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidLabelAddress_when_labelAddressIsEmpty(void) {
  // Arrange
  const char source[] = "@:\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 1}, {0, 1} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_LABEL_ADDR, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidLabelAddress_when_labelAddressIsTooLarge(void) {
  // Arrange
  const char source[] = "@10000:\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 1}, {0, 6} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_LABEL_ADDR, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidOpcode_when_firstWordIsNotValidOpcode(void) {
  // Arrange
  const char source[] = "nopbutnotreally\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 0}, {0, 15} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(UNRECOGNIZED_MNEMONIC, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidParameter_when_parameterIsNotRegisterOrHexValueOrReferenceOrInteger(void) {
  // Arrange
  const char source[] = "set $x0, invalid\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 9}, {0, 16} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_OPERAND, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidParameter_when_lineEndsAfterComma(void) {
  // Arrange
  const char source[] = "set $x0, \n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 9}, {0, 9} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_OPERAND, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidRegister_when_dollarSignFollowedByInvalidRegisterName(void) {
  // Arrange
  const char source[] = "set $invalid, 123\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 4}, {0, 12} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_REGISTER, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidHexValue_when_zeroExFollowedByInvalidHexDigit(void) {
  // Arrange
  const char source[] = "set $x0, 0xg\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 9}, {0, 12} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_HEX_VALUE, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidHexValue_when_hexParameterIsGreaterThanMaxUnsignedInteger(void) {
  // Arrange
  const char source[] = "jmp 0x100000000\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 4}, {0, 15} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_HEX_VALUE, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidIntValue_when_integerParameterIsGreaterThanMaxSignedInteger(void) {
  // Arrange
  const char source[] = "jmp 2147483648\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 4}, {0, 14} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_INT_VALUE, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidIntValue_when_integerParameterIsLessThanMinSignedInteger(void) {
  // Arrange
  const char source[] = "jmp -2147483649\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 4}, {0, 15} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_INT_VALUE, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithInvalidByte_when_dataLineContainsInvalidByte(void) {
  // Arrange
  const char source[] = ".data FF b\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 9}, {0, 10} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(INVALID_HEX_BYTE, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedCharacter_when_invalidCharacterAfterValidParameter(void) {
  // Arrange
  const char source[] = "set $x0, 123~\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 12}, {0, 13} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedCharacter_when_missingCommaBetweenParameters(void) {
  // Arrange
  const char source[] = "set $x0 123\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 8}, {0, 9} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedCharacter_when_invalidCharacterAfterLabel(void) {
  // Arrange
  const char source[] = "test~:\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 4}, {0, 5} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedEndOfFile_when_onlyEmptyLine(void) {
  // Arrange
  const char source[] = "\n";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 1 };
  const TextSpan expectedErrorSpan = { {0, 1}, {0, 1} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_END_OF_FILE, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

void test_TryParseAssemblyLine_should_failWithUnexpectedEndOfFile_when_atEndOfFile(void) {
  // Arrange
  const char source[] = "";
  text = InitTextContentsAsCopyCStr(source);

  // Act
  bool success = TryParseAssemblyLine(&text, &position, &dataBuffer, &dataBufferSize, &line, &error);

  // Assert
  const TextOffset expectedPosition = { 0, sizeof(source) - 2 };
  const TextSpan expectedErrorSpan = { {0, 0}, {0, 0} };
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expectedPosition, position, text);
  TEST_ASSERT_EQUAL_STRING(UNEXPECTED_END_OF_FILE, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, text);
}

#pragma endregion
