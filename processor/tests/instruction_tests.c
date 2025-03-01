#include <unity.h>
#include <string.h>
#include "custom_assertions.h"
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
  // Arrange
  unsigned short ip = 4;
  memory[ip] = (unsigned char)NOP;

  struct Instruction expectedInstruction = { .opcode = NOP };

  // Act
  struct Instruction actualInstruction = fetchInstruction(memory, &ip);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(5, ip);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoad8BitImmediateValue_whenOpcodeHasLayoutImm8(void) {
  // Arrange
  unsigned short ip = 0;
  memory[ip] = (unsigned char)LDIB;
  memory[ip + 1] = 0xFF;

  struct Instruction expectedInstruction = { .opcode = LDIB, .parameters.immediate.u16 = 0x00FF };

  // Act
  struct Instruction actualInstruction = fetchInstruction(memory, &ip);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(2, ip);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoadRegisterAAnd4BitImmediateValue_whenOpcodeHasLayoutRegAImm4(void) {
  // Arrange
  unsigned short ip = 0;
  memory[ip] = (unsigned char)LDMB;
  memory[ip + 1] = ((unsigned char)X10 << 4) | 0xF;

  struct Instruction expectedInstruction = { .opcode = LDMB, .parameters.registerA = X10, .parameters.immediate.u16 = 0x000F };

  // Act
  struct Instruction actualInstruction = fetchInstruction(memory, &ip);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(2, ip);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoadRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB(void) {
  // Arrange
  unsigned short ip = 0;
  memory[ip] = (unsigned char)ADD;
  memory[ip + 1] = ((unsigned char)X10 << 4) | ((unsigned char)X0 & 0xF);

  struct Instruction expectedInstruction = { .opcode = ADD, .parameters.registerA = X10, .parameters.registerB = X0 };

  // Act
  struct Instruction actualInstruction = fetchInstruction(memory, &ip);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(2, ip);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoad16BitImmediateValue_whenOpcodeHasLayoutImm16(void) {
  // Arrange
  unsigned short ip = 0;
  memory[ip] = (unsigned char)LDIW;
  memory[ip + 1] = 0xFF;
  memory[ip + 2] = 0xEE;

  struct Instruction expectedInstruction = { .opcode = LDIW, .parameters.immediate.u16 = 0xEEFF };

  // Act
  struct Instruction actualInstruction = fetchInstruction(memory, &ip);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(3, ip);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

// Currently unused layouts

// void test_fetchInstruction_shouldLoadRegisterAAnd12BitImmediateValue_whenOpcodeHasLayoutRegAImm12(void) {
// }

// void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd8BitImmediateValue_whenOpcodeHasLayoutRegARegBImm8(void) {
// }

// void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd16BitImmediateValue_whenOpcodeHasLayoutRegARegBImm16(void) {
// }

#pragma endregion

#pragma region storeInstruction

void test_storeInstruction_shouldSaveOpcode_whenOpcodeHasLayoutNone(void) {
  // Arrange
  struct Instruction instruction = { .opcode = NOP };

  expectedMemory[4] = (unsigned char)NOP;

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
    int bytesWritten = storeInstruction(memory, 0, instruction);
    
    // Assert
    TEST_ASSERT_EQUAL_INT(2, bytesWritten);
    TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_storeInstruction_shouldSaveRegisterAAnd4BitImmediateValue_whenOpcodeHasLayoutRegAImm4(void) {
  // Arrange
  struct Instruction instruction = { .opcode = LDMB, .parameters.registerA = X10, .parameters.immediate.u4 = 0xF };

  expectedMemory[0] = (unsigned char)LDMB;
  expectedMemory[1] = ((unsigned char)X10 << 4) | 0xF;

  // Act
  int bytesWritten = storeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(2, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_storeInstruction_shouldSaveRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB(void) {
  // Arrange
  struct Instruction instruction = { .opcode = ADD, .parameters.registerA = X10, .parameters.registerB = X0 };

  expectedMemory[0] = (unsigned char)ADD;
  expectedMemory[1] = ((unsigned char)X10 << 4) | ((unsigned char)X0 & 0xF);

  // Act
  int bytesWritten = storeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(2, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_storeInstruction_shouldSave16BitImmediateValue_whenOpcodeHasLayoutImm16(void) {
  // Arrange
  struct Instruction instruction = { .opcode = LDIW, .parameters.immediate.u16 = 0xEEFF };

  expectedMemory[0] = (unsigned char)LDIW;
  expectedMemory[1] = 0xFF;
  expectedMemory[2] = 0xEE;

  // Act
  int bytesWritten = storeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(3, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

// Currently unused layouts

// void test_storeInstruction_shouldSaveRegisterAAnd12BitImmediateValue_whenOpcodeHasLayoutRegAImm12(void) {
// }

// void test_storeInstruction_shouldSaveRegisterAAndRegisterBAnd8BitImmediateValue_whenOpcodeHasLayoutRegARegBImm8(void) {
// }

// void test_storeInstruction_shouldSaveRegisterAAndRegisterBAnd16BitImmediateValue_whenOpcodeHasLayoutRegARegBImm16(void) {
// }

#pragma endregion

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_fetchInstruction_shouldLoadOpcode_whenOpcodeHasLayoutNone);
  RUN_TEST(test_fetchInstruction_shouldLoad8BitImmediateValue_whenOpcodeHasLayoutImm8);
  RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAnd4BitImmediateValue_whenOpcodeHasLayoutRegAImm4);
  RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB);
  RUN_TEST(test_fetchInstruction_shouldLoad16BitImmediateValue_whenOpcodeHasLayoutImm16);
  // RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAnd12BitImmediateValue_whenOpcodeHasLayoutRegAImm12);
  // RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd8BitImmediateValue_whenOpcodeHasLayoutRegARegBImm8);
  // RUN_TEST(test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd16BitImmediateValue_whenOpcodeHasLayoutRegARegBImm16);
  RUN_TEST(test_storeInstruction_shouldSaveOpcode_whenOpcodeHasLayoutNone);
  RUN_TEST(test_storeInstruction_shouldSave8BitImmediateValue_whenOpcodeHasLayoutImm8);
  RUN_TEST(test_storeInstruction_shouldSaveRegisterAAnd4BitImmediateValue_whenOpcodeHasLayoutRegAImm4);
  RUN_TEST(test_storeInstruction_shouldSaveRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB);
  RUN_TEST(test_storeInstruction_shouldSave16BitImmediateValue_whenOpcodeHasLayoutImm16);
  // RUN_TEST(test_storeInstruction_shouldSaveRegisterAAnd12BitImmediateValue_whenOpcodeHasLayoutRegAImm12);
  // RUN_TEST(test_storeInstruction_shouldSaveRegisterAAndRegisterBAnd8BitImmediateValue_whenOpcodeHasLayoutRegARegBImm8);
  // RUN_TEST(test_storeInstruction_shouldSaveRegisterAAndRegisterBAnd16BitImmediateValue_whenOpcodeHasLayoutRegARegBImm16);
  return UNITY_END();
}
