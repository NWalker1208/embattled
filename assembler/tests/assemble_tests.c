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
  initializeProgram("add $x0, $x1, $x2");

  writeInstruction(expectedMemory, 0x0000, (struct Instruction){ 
    .opcode = OPCODE_ADD_R,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
    .operands.registerC = REGISTER_X2
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

  writeInstruction(expectedMemory, 0x0000, (struct Instruction){ 
    .opcode = OPCODE_JMP_I,
    .operands.immediateA.u16 = 0x0003,
  });
  writeInstruction(expectedMemory, 0x0003, (struct Instruction){
    .opcode = OPCODE_NOP,
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
    "  set $x0, @test\n"
    "test:\n"
    "  .data 12 34 56 78 9A BC DE F0"
  );

  writeInstruction(expectedMemory, 0x0000, (struct Instruction){ 
    .opcode = OPCODE_SET_I,
    .operands.registerA = REGISTER_X0,
    .operands.immediateA.u16 = 0x0004,
  });
  unsigned char bytes[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
  memcpy(&expectedMemory[4], bytes, sizeof(bytes));

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_TryAssembleProgram_should_outputErrorNoMatchingOverload_when_instructionHasIncorrectNumberOfParams(void) {
  // Arrange
  initializeProgram("add $x0, $x1");

  TextSpan expectedErrorSpan = {{0, 0}, {0, 12}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(NO_MATCHING_OVERLOAD, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

void test_TryAssembleProgram_should_outputErrorNoMatchingOverload_when_instructionHasImmediateValueInsteadOfRegister(void) {
  // Arrange
  initializeProgram("add 0x1234, $x0, $x1");
  
  TextSpan expectedErrorSpan = {{0, 0}, {0, 20}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(NO_MATCHING_OVERLOAD, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

TEST_CASE("add $x0, $x1, -32769",  IMMEDIATE_VALUE_OUT_OF_RANGE_16_BIT, 14, 20)
TEST_CASE("add $x0, $x1, 0x10000", IMMEDIATE_VALUE_OUT_OF_RANGE_16_BIT, 14, 21)
TEST_CASE("jmp -1",                IMMEDIATE_VALUE_OUT_OF_RANGE_16_BIT_UNSIGNED, 4, 6)
TEST_CASE("jmp 0x10000",           IMMEDIATE_VALUE_OUT_OF_RANGE_16_BIT_UNSIGNED, 4, 11)
TEST_CASE("divs $x0, $x1, -32769", IMMEDIATE_VALUE_OUT_OF_RANGE_16_BIT_SIGNED, 15, 21)
TEST_CASE("divs $x0, $x1, 0x8000", IMMEDIATE_VALUE_OUT_OF_RANGE_16_BIT_SIGNED, 15, 21)
TEST_CASE("stb -129, $x0",         IMMEDIATE_VALUE_OUT_OF_RANGE_8_BIT, 4, 8)
TEST_CASE("stb 257, $x0",          IMMEDIATE_VALUE_OUT_OF_RANGE_8_BIT, 4, 7)
TEST_CASE("rshs $x0, $x1, -1",     IMMEDIATE_VALUE_OUT_OF_RANGE_4_BIT_UNSIGNED, 15, 17)
TEST_CASE("rshs $x0, $x1, 16",     IMMEDIATE_VALUE_OUT_OF_RANGE_4_BIT_UNSIGNED, 15, 17)
void test_TryAssembleProgram_should_outputErrorImmediateValueOutOfRange_when_instructionHasImmediateValueOutOfRange(const char* source, const char* expectedErrorMessage, size_t errorSpanStartCol, size_t errorSpanEndCol) {
  // Arrange
  initializeProgram(source);

  TextSpan expectedErrorSpan = {{0, errorSpanStartCol}, {0, errorSpanEndCol}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(expectedErrorMessage, error.message);
  TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expectedErrorSpan, error.sourceSpan, sourceText);
}

void test_TryAssembleProgram_should_outputErrorNoMatchingOverload_when_instructionHasUndefinedMnemonic(void) {
  // Arrange
  initializeProgram("nop");
  program.lines[0].instruction.mnemonic = ASSEMBLY_MNEMONIC_COUNT; // Not a valid mnemonic

  TextSpan expectedErrorSpan = {{0, 0}, {0, 3}};

  // Act
  bool success = TryAssembleProgram(&sourceText, &program, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(NO_MATCHING_OVERLOAD, error.message);
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
