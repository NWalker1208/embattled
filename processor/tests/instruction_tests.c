#include <unity.h>
#include <string.h>
#include "custom_assertions.h"
#include "processor/instruction.h"

uint8_t memory[8];
uint8_t expectedMemory[8];

void setUp() {
  memset(memory, 0xAA, sizeof(memory));
  memset(expectedMemory, 0xAA, sizeof(expectedMemory));
}

void tearDown() { }

#pragma region fetchInstruction

void test_fetchInstruction_shouldLoadOpcode_whenOpcodeHasLayoutNone(void) {
  // Arrange
  uint16_t ip = 4;
  memory[ip] = (uint8_t)OPCODE_NOP;

  Instruction expectedInstruction = { .opcode = OPCODE_NOP };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(1, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoadRegisterA_whenOpcodeHasLayoutRegA(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_JMP_R;
  memory[ip + 1] = ((uint8_t)REGISTER_X10 << 4);

  Instruction expectedInstruction = { .opcode = OPCODE_JMP_R, .operands.registerA = REGISTER_X10 };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(2, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoadRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_SET_R;
  memory[ip + 1] = ((uint8_t)REGISTER_X10 << 4) | ((uint8_t)REGISTER_X0 & 0xF);

  Instruction expectedInstruction = { .opcode = OPCODE_SET_R, .operands = { .registerA = REGISTER_X10, .registerB = REGISTER_X0 } };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(2, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoad16BitImmediateValueA_whenOpcodeHasLayoutImmA16(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_JMP_I;
  memory[ip + 1] = 0xCD;
  memory[ip + 2] = 0xAB;

  Instruction expectedInstruction = { .opcode = OPCODE_JMP_I, .operands.immediateA.u16 = 0xABCD };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(3, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoadRegisterAAnd8BitImmediateValueA_whenOpcodeHasLayoutRegAImmA8(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_STB_IR;
  memory[ip + 1] = 0xAB;
  memory[ip + 2] = ((uint8_t)REGISTER_X10 << 4);

  Instruction expectedInstruction = { .opcode = OPCODE_STB_IR, .operands = { .registerA = REGISTER_X10, .immediateA.u16 = 0x00AB } };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(3, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd4BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA4(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_LSH_RI;
  memory[ip + 1] = ((uint8_t)REGISTER_X10 << 4) | ((uint8_t)REGISTER_X0 & 0xF);
  memory[ip + 2] = 0xAB;

  Instruction expectedInstruction = { .opcode = OPCODE_LSH_RI, .operands =
    { .registerA = REGISTER_X10, .registerB = REGISTER_X0, .immediateA.u16 = 0x000B } };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(3, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAndRegisterC_whenOpcodeHasLayoutRegARegBRegC(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_ADD_R;
  memory[ip + 1] = ((uint8_t)REGISTER_X8 << 4);
  memory[ip + 2] = ((uint8_t)REGISTER_X10 << 4) | ((uint8_t)REGISTER_X0 & 0xF);

  Instruction expectedInstruction = { .opcode = OPCODE_ADD_R, .operands =
    { .registerA = REGISTER_X10, .registerB = REGISTER_X0, .registerC = REGISTER_X8 } };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(3, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoad8BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA8ImmB16(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_STB_II;
  memory[ip + 1] = 0xAB;
  memory[ip + 2] = 0xEF;
  memory[ip + 3] = 0xCD;

  Instruction expectedInstruction = { .opcode = OPCODE_STB_II, .operands = { .immediateA.u16 = 0x00AB, .immediateB.u16 = 0xCDEF } };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(4, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoadRegisterAAnd16BitImmediateValueA_whenOpcodeHasLayoutRegAImmA16(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_STW_RI;
  memory[ip + 1] = 0xCD;
  memory[ip + 2] = 0xAB;
  memory[ip + 3] = ((uint8_t)REGISTER_X6 << 4);

  Instruction expectedInstruction = { .opcode = OPCODE_STW_RI, .operands = { .registerA = REGISTER_X6, .immediateA.u16 = 0xABCD } };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(4, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd16BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA16(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_MUL_I;
  memory[ip + 1] = 0x34;
  memory[ip + 2] = 0x12;
  memory[ip + 3] = ((uint8_t)REGISTER_X10 << 4) | ((uint8_t)REGISTER_X0 & 0xF);

  Instruction expectedInstruction = { .opcode = OPCODE_MUL_I, .operands =
    { .registerA = REGISTER_X10, .registerB = REGISTER_X0, .immediateA.u16 = 0x1234 } };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(4, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

void test_fetchInstruction_shouldLoad16BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA16ImmB16(void) {
  // Arrange
  uint16_t ip = 0;
  memory[ip] = (uint8_t)OPCODE_STW_II;
  memory[ip + 1] = 0x34;
  memory[ip + 2] = 0x12;
  memory[ip + 3] = 0x78;
  memory[ip + 4] = 0x56;

  Instruction expectedInstruction = { .opcode = OPCODE_STW_II, .operands = { .immediateA.u16 = 0x1234, .immediateB.u16 = 0x5678 } };

  // Act
  Instruction actualInstruction = { 0 };
  uint16_t bytesRead = fetchInstruction(memory, ip, &actualInstruction);
  
  // Assert
  TEST_ASSERT_EQUAL_UINT16(5, bytesRead);
  TEST_ASSERT_EQUAL_INSTRUCTION(&expectedInstruction, &actualInstruction);
}

#pragma endregion

#pragma region storeInstruction

void test_storeInstruction_shouldSaveOpcode_whenOpcodeHasLayoutNone(void) {
  // Arrange
  struct Instruction instruction = { .opcode = NOP };

  expectedMemory[4] = (unsigned char)NOP;

  // Act
  unsigned short bytesWritten = storeInstruction(memory, 4, instruction);
  
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
    unsigned short bytesWritten = storeInstruction(memory, 0, instruction);
    
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
  unsigned short bytesWritten = storeInstruction(memory, 0, instruction);
  
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
  unsigned short bytesWritten = storeInstruction(memory, 0, instruction);
  
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
  unsigned short bytesWritten = storeInstruction(memory, 0, instruction);
  
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

void test_storeInstruction_shouldReturnZero_whenOpcodeIsInvalid(void) {
  // Arrange
  struct Instruction instruction = { .opcode = OPCODE_COUNT };

  // Act
  unsigned short bytesWritten = storeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(0, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

#pragma endregion
