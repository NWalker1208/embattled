#include <unity.h>
#include <string.h>
#include "processor/instruction.h"

unsigned char memory[8];
unsigned char expectedMemory[8];

void setUp() {
  memset(memory, 0xAA, sizeof(memory));
  memset(expectedMemory, 0xAA, sizeof(expectedMemory));
}

void tearDown() { }

#pragma region fetchInstruction

void test_fetchInstruction_shouldLoadOpcode_whenOpcodeHasLayoutNone(void) {

}

void test_fetchInstruction_shouldLoad8BitImmediateValue_whenOpcodeHasLayoutImm8(void) {

}

void test_fetchInstruction_shouldLoadRegisterAAnd4BitImmediateValue_whenOpcodeHasLayoutRegAImm4(void) {

}

void test_fetchInstruction_shouldLoadRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB(void) {

}

void test_fetchInstruction_shouldLoad16BitImmediateValue_whenOpcodeHasLayoutImm16(void) {

}

void test_fetchInstruction_shouldLoadRegisterAAnd12BitImmediateValue_whenOpcodeHasLayoutRegAImm12(void) {

}

void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd8BitImmediateValue_whenOpcodeHasLayoutRegARegBImm8(void) {

}

void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd16BitImmediateValue_whenOpcodeHasLayoutRegARegBImm16(void) {

}

#pragma endregion

#pragma region storeInstruction

void test_storeInstruction_shouldSaveOpcode_whenOpcodeHasLayoutNone(void) {
  // Arrange
  struct Instruction instruction = { .opcode = NOP, 0 };

  expectedMemory[4] = 0;

  // Act
  int bytesWritten = storeInstruction(memory, 4, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(1, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_storeInstruction_shouldSave8BitImmediateValue_whenOpcodeHasLayoutImm8(void) {
    // Arrange
    struct Instruction instruction = { .opcode = LDIB, .parameters.immediate.u8 = 0xFF };

    expectedMemory[0] = (unsigned char)LDIB;
    expectedMemory[1] = 0xFF;
  
    // Act
    int bytesWritten = storeInstruction(memory, 4, instruction);
    
    // Assert
    TEST_ASSERT_EQUAL_INT(2, bytesWritten);
    TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_storeInstruction_shouldSaveRegisterAAnd4BitImmediateValue_whenOpcodeHasLayoutRegAImm4(void) {

}

void test_storeInstruction_shouldSaveRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB(void) {

}

void test_storeInstruction_shouldSave16BitImmediateValue_whenOpcodeHasLayoutImm16(void) {

}

void test_storeInstruction_shouldSaveRegisterAAnd12BitImmediateValue_whenOpcodeHasLayoutRegAImm12(void) {

}

void test_storeInstruction_shouldSaveRegisterAAndRegisterBAnd8BitImmediateValue_whenOpcodeHasLayoutRegARegBImm8(void) {

}

void test_storeInstruction_shouldSaveRegisterAAndRegisterBAnd16BitImmediateValue_whenOpcodeHasLayoutRegARegBImm16(void) {

}

#pragma endregion

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_fetchInstruction_shouldLoadOpcode_whenOpcodeHasLayoutNone);
  RUN_TEST(test_fetchInstruction_shouldLoad8BitImmediateValue_whenOpcodeHasLayoutImm8);
  RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAnd4BitImmediateValue_whenOpcodeHasLayoutRegAImm4);
  RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB);
  RUN_TEST(test_fetchInstruction_shouldLoad16BitImmediateValue_whenOpcodeHasLayoutImm16);
  RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAnd12BitImmediateValue_whenOpcodeHasLayoutRegAImm12);
  RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd8BitImmediateValue_whenOpcodeHasLayoutRegARegBImm8);
  RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd16BitImmediateValue_whenOpcodeHasLayoutRegARegBImm16);
  RUN_TEST(test_storeInstruction_shouldSaveOpcode_whenOpcodeHasLayoutNone);
  RUN_TEST(test_storeInstruction_shouldSave8BitImmediateValue_whenOpcodeHasLayoutImm8);
  RUN_TEST(test_storeInstruction_shouldSaveRegisterAAnd4BitImmediateValue_whenOpcodeHasLayoutRegAImm4);
  RUN_TEST(test_storeInstruction_shouldSaveRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB);
  RUN_TEST(test_storeInstruction_shouldSave16BitImmediateValue_whenOpcodeHasLayoutImm16);
  RUN_TEST(test_storeInstruction_shouldSaveRegisterAAnd12BitImmediateValue_whenOpcodeHasLayoutRegAImm12);
  RUN_TEST(test_storeInstruction_shouldSaveRegisterAAndRegisterBAnd8BitImmediateValue_whenOpcodeHasLayoutRegARegBImm8);
  RUN_TEST(test_storeInstruction_shouldSaveRegisterAAndRegisterBAnd16BitImmediateValue_whenOpcodeHasLayoutRegARegBImm16);
  return UNITY_END();
}
