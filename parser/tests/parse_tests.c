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
  const char source[] = "add $x0, @reference, 0x1234, -5, +7, 42\n";

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
  TEST_ASSERT_EQUAL_PTR(UNEXPECTED_CHARACTER, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
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
  TEST_ASSERT_EQUAL_PTR(UNEXPECTED_END_OF_FILE, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
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
  TEST_ASSERT_EQUAL_PTR(UNEXPECTED_END_OF_FILE, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}
