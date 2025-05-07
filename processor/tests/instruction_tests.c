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

#pragma region writeInstruction

void test_writeInstruction_shouldSaveOpcode_whenOpcodeHasLayoutNone(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_NOP };

  expectedMemory[4] = (uint8_t)OPCODE_NOP;

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 4, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(1, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSaveRegisterA_whenOpcodeHasLayoutRegA(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_JMP_R, .operands.registerA = REGISTER_X10 };

  expectedMemory[0] = (uint8_t)OPCODE_JMP_R;
  expectedMemory[1] = ((uint8_t)REGISTER_X10 << 4);

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(2, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSaveRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_SET_R, .operands = { .registerA = REGISTER_X10, .registerB = REGISTER_X0 } };

  expectedMemory[0] = (uint8_t)OPCODE_SET_R;
  expectedMemory[1] = ((uint8_t)REGISTER_X10 << 4) | ((uint8_t)REGISTER_X0 & 0xF);

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(2, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSave16BitImmediateValueA_whenOpcodeHasLayoutImmA16(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_JMP_I, .operands.immediateA.u16 = 0xABCD };

  expectedMemory[0] = (uint8_t)OPCODE_JMP_I;
  expectedMemory[1] = 0xCD;
  expectedMemory[2] = 0xAB;

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(3, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSaveRegisterAAnd8BitImmediateValueA_whenOpcodeHasLayoutRegAImmA8(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_STB_IR, .operands = { .registerA = REGISTER_X10, .immediateA.u16 = 0x00AB } };

  expectedMemory[0] = (uint8_t)OPCODE_STB_IR;
  expectedMemory[1] = 0xAB;
  expectedMemory[2] = ((uint8_t)REGISTER_X10 << 4);

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(3, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSaveRegisterAAndRegisterBAnd4BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA4(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_LSH_RI, .operands =
    { .registerA = REGISTER_X10, .registerB = REGISTER_X0, .immediateA.u16 = 0x000B } };

  expectedMemory[0] = (uint8_t)OPCODE_LSH_RI;
  expectedMemory[1] = ((uint8_t)REGISTER_X10 << 4) | ((uint8_t)REGISTER_X0 & 0xF);
  expectedMemory[2] = 0xAB;

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(3, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSaveRegisterAAndRegisterBAndRegisterC_whenOpcodeHasLayoutRegARegBRegC(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_ADD_R, .operands =
    { .registerA = REGISTER_X10, .registerB = REGISTER_X0, .registerC = REGISTER_X8 } };

  expectedMemory[0] = (uint8_t)OPCODE_ADD_R;
  expectedMemory[1] = ((uint8_t)REGISTER_X8 << 4);
  expectedMemory[2] = ((uint8_t)REGISTER_X10 << 4) | ((uint8_t)REGISTER_X0 & 0xF);

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(3, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSave8BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA8ImmB16(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_STB_II, .operands = { .immediateA.u16 = 0x00AB, .immediateB.u16 = 0xCDEF } };

  expectedMemory[0] = (uint8_t)OPCODE_STB_II;
  expectedMemory[1] = 0xAB;
  expectedMemory[2] = 0xEF;
  expectedMemory[3] = 0xCD;

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(4, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSaveRegisterAAnd16BitImmediateValueA_whenOpcodeHasLayoutRegAImmA16(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_STW_RI, .operands = { .registerA = REGISTER_X6, .immediateA.u16 = 0xABCD } };

  expectedMemory[0] = (uint8_t)OPCODE_STW_RI;
  expectedMemory[1] = 0xCD;
  expectedMemory[2] = 0xAB;
  expectedMemory[3] = ((uint8_t)REGISTER_X6 << 4);

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(4, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSaveRegisterAAndRegisterBAnd16BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA16(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_MUL_I, .operands =
    { .registerA = REGISTER_X10, .registerB = REGISTER_X0, .immediateA.u16 = 0x1234 } };

  expectedMemory[0] = (uint8_t)OPCODE_MUL_I;
  expectedMemory[1] = 0x34;
  expectedMemory[2] = 0x12;
  expectedMemory[3] = ((uint8_t)REGISTER_X10 << 4) | ((uint8_t)REGISTER_X0 & 0xF);

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(4, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldSave16BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA16ImmB16(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_STW_II, .operands = { .immediateA.u16 = 0x1234, .immediateB.u16 = 0x5678 } };

  expectedMemory[0] = (uint8_t)OPCODE_STW_II;
  expectedMemory[1] = 0x34;
  expectedMemory[2] = 0x12;
  expectedMemory[3] = 0x78;
  expectedMemory[4] = 0x56;

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(5, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_writeInstruction_shouldReturnZero_whenOpcodeIsInvalid(void) {
  // Arrange
  Instruction instruction = { .opcode = OPCODE_COUNT };

  // Act
  uint16_t bytesWritten = writeInstruction(memory, 0, instruction);
  
  // Assert
  TEST_ASSERT_EQUAL_INT(0, bytesWritten);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

#pragma endregion
