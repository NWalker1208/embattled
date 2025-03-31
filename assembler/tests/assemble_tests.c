#define UNITY_SUPPORT_TEST_CASES
#include <unity.h>
#include <string.h>
#include "test_utilities/text_assertions.h"
#include "assembler/assemble.h"
#include "parser/parse.h"
#include "processor/process.h"
#include "processor/instruction.h"

TextContents sourceText;
AssemblyProgram program;
unsigned char memory[MEMORY_SIZE];
unsigned char expectedMemory[MEMORY_SIZE];
AssemblingError error;

void setUp() {
  sourceText = (TextContents){0};
  program = (AssemblyProgram){0};
  memset(memory, 0x00, sizeof(memory));
  memset(expectedMemory, 0x00, sizeof(expectedMemory));
  error = (AssemblingError){0};
}

void tearDown() {
  DestroyTextContents(&sourceText);
  DestroyAssemblyProgram(&program);
}

void initializeProgram(const char* source) {
  sourceText = InitTextContentsAsCopyCStr(source);
  ParsingErrorList parsingErrors = {0};
  if (!TryParseAssemblyProgram(&sourceText, &program, &parsingErrors)) {
    TEST_FAIL_MESSAGE("Failed to parse input assembly file for test.");
  }
}

void test_TryAssembleProgram_should_outputInstructionBytes_when_lineIsValidAssemblyInstruction(void) {
  // Arrange
  initializeProgram("add $x0, $x1");

  storeInstruction(expectedMemory, 0x0000, (struct Instruction){ 
    .opcode = ADD,
    .parameters.registerA = X0,
    .parameters.registerB = X1
  });

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_TryAssembleProgram_should_outputDataBytes_when_lineIsValidAssemblyData(void) {
  // Arrange
  initializeProgram(".data 12 34 56 78 9A BC DE F0");

  unsigned char bytes[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
  memcpy(expectedMemory, bytes, sizeof(bytes));

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_TryAssembleProgram_should_outputInstructionWithReference_when_instructionReferencesInstruction(void) {
  // Arrange
  initializeProgram(
    "  jmp @test\n"
    "test:\n"
    "  nop"
  );

  storeInstruction(expectedMemory, 0x0000, (struct Instruction){ 
    .opcode = JMP,
    .parameters.immediate.u16 = 0x0003,
  });
  storeInstruction(expectedMemory, 0x0003, (struct Instruction){
    .opcode = NOP,
  });

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_TryAssembleProgram_should_outputInstructionWithReference_when_instructionReferencesData(void) {
  // Arrange
  initializeProgram(
    "  ldiw @test\n"
    "test:\n"
    "  .data 12 34 56 78 9A BC DE F0"
  );

  storeInstruction(expectedMemory, 0x0000, (struct Instruction){ 
    .opcode = LDIW,
    .parameters.immediate.u16 = 0x0003,
  });
  unsigned char bytes[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
  memcpy(&expectedMemory[3], bytes, sizeof(bytes));

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_TryAssembleProgram_should_outputErrorTooFewParams_when_instructionHasTooFewParams(void) {
  // Arrange
  initializeProgram("add $x0");

  TextSpan expectedErrorSpan = {{0, 0}, {0, 7}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(TOO_FEW_PARAMS, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

void test_TryAssembleProgram_should_outputErrorTooManyParams_when_instructionHasTooManyParams(void) {
  // Arrange
  initializeProgram("add $x0, $x1, $x2");

  TextSpan expectedErrorSpan = {{0, 0}, {0, 17}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(TOO_MANY_PARAMS, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

void test_TryAssembleProgram_should_outputErrorExpectedRegister_when_instructionHasImmediateValueInsteadOfRegister(void) {
  // Arrange
  initializeProgram("add $x0, 0x1234");
  
  TextSpan expectedErrorSpan = {{0, 9}, {0, 15}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(EXPECTED_REGISTER, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

void test_TryAssembleProgram_should_outputErrorExpectedImmediateValue_when_instructionHasRegisterInsteadOfUnder16BitImmediateValue(void) {
  // Arrange
  initializeProgram("inc $x0, $x1");
  
  TextSpan expectedErrorSpan = {{0, 9}, {0, 12}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(EXPECTED_IMMEDIATE_VALUE, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

void test_TryAssembleProgram_should_outputErrorExpectedImmediateValueOrLabelRef_when_instructionHasRegisterInsteadOf16BitImmediateValue(void) {
  // Arrange
  initializeProgram("jmp $x0");

  TextSpan expectedErrorSpan = {{0, 4}, {0, 7}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(EXPECTED_IMMEDIATE_VALUE_OR_LABEL_REF, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

TEST_CASE("jmp -1", 4, 6)
TEST_CASE("jmp 0x10000", 4, 11)
TEST_CASE("ldib -1", 5, 7)
TEST_CASE("ldib 0x100", 5, 10)
TEST_CASE("inc $x0, -1", 9, 11)
TEST_CASE("inc $x0, 0x10", 9, 13)
TEST_CASE("ldmb $x0, -9", 10, 12)
TEST_CASE("ldmb $x0, 8", 10, 11)
void test_TryAssembleProgram_should_outputErrorImmediateValueOutOfRange_when_instructionHasImmediateValueOutOfRange(const char* source, size_t errorSpanStartCol, size_t errorSpanEndCol) {
  // Arrange
  initializeProgram(source);

  TextSpan expectedErrorSpan = {{0, errorSpanStartCol}, {0, errorSpanEndCol}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(IMMEDIATE_VALUE_OUT_OF_RANGE, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

void test_TryAssembleProgram_should_outputErrorInvalidInstruction_when_instructionHasUndefinedOpcode(void) {
  // Arrange
  initializeProgram("nop");
  program.lines[0].instruction.opcode = OPCODE_COUNT; // Not a valid opcode

  TextSpan expectedErrorSpan = {{0, 0}, {0, 3}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(INVALID_INSTRUCTION, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

void test_TryAssembleProgram_should_outputErrorUndefinedLabel_when_instructionReferencesUndefinedLabel(void) {
  // Arrange
  initializeProgram("jmp @test");

  TextSpan expectedErrorSpan = {{0, 4}, {0, 9}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(UNDEFINED_LABEL_NAME, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}
