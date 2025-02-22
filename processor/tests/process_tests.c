#include <unity.h>
#include <string.h>
#include "processor/process.h"
#include "processor/opcode.h"
#include "processor/register.h"
#include "processor/instruction.h"

struct ProcessState processState;
struct ProcessState expectedEndState;

void setUp() {
  // Reset registers and memory
  memset(&processState.registers, 0, sizeof(processState.registers));
  for (unsigned short i = 0; i < MEMORY_SIZE; i += 1) {
    // Fill memory with the lower 8 bits of the address as canary for memory access bugs
    processState.memory[i] = (unsigned char)(i & 0xFF);
  }
}

void initializeExpectedEndState() {
  memcpy(&expectedEndState, &processState, sizeof(processState));
}

void tearDown() { }

void test_nop_should_doNothing() {
  // Arrange
  processState.memory[0] = OPCODE_VALUES[NOP];

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0001;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_jmp_should_jumpToAddressAndSaveReturnAddress() {
  // Arrange
  processState.memory[0] = OPCODE_VALUES[JMP];
  processState.memory[1] = 0x34;
  processState.memory[2] = 0x12;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;
  expectedEndState.registers.ac = 0x0003;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_jmz_should_jumpToAddress_when_acIsZero() {
  // Arrange
  processState.registers.ac = 0x0000;
  processState.memory[0] = OPCODE_VALUES[JMZ];
  processState.memory[1] = 0x34;
  processState.memory[2] = 0x12;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_jmz_should_doNothing_when_acIsNonZero() {
  // Arrange
  processState.registers.ac = 0x0001;
  processState.memory[0] = OPCODE_VALUES[JMZ];
  processState.memory[1] = 0x34;
  processState.memory[2] = 0x12;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_mov_should_copyValueFromRegisterBToRegisterA_when_neitherRegisterIsNull() {
  for (int regA = 1; regA < REGISTER_COUNT; regA++) {
    for (int regB = 1; regB < REGISTER_COUNT; regB++) {
      // Arrange
      setUp();
      *getRegisterPtr(&processState.registers, (enum Register)regB) = 0x5678;
      *getRegisterPtr(&processState.registers, (enum Register)regA) = 0x1234;
      processState.memory[0] = OPCODE_VALUES[MOV];
      processState.memory[1] = REGISTER_CODES[regA] << 4 | REGISTER_CODES[regB];

      initializeExpectedEndState();
      *getRegisterPtr(&expectedEndState.registers, (enum Register)regA) = 0x5678;
      expectedEndState.registers.ip = 0x0002;

      // Act
      stepProcess(&processState);

      // Assert
      TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
    }
  }
}

void test_mov_should_setRegisterAToZero_when_registerAIsNotNullAndRegisterBIsNull() {
  for (int regA = 1; regA < REGISTER_COUNT; regA++) {
    // Arrange
    setUp();
    *getRegisterPtr(&processState.registers, (enum Register)regA) = 0x1234;
    processState.memory[0] = OPCODE_VALUES[MOV];
    processState.memory[1] = REGISTER_CODES[regA] << 4 | REGISTER_CODES[NL];
    
    initializeExpectedEndState();
    *getRegisterPtr(&expectedEndState.registers, (enum Register)regA) = 0x0000;
    expectedEndState.registers.ip = 0x0002;

    // Act
    stepProcess(&processState);

    // Assert
    TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
  }
}

void test_mov_should_doNothing_when_registerAIsNull() {
  for (int regB = 0; regB < REGISTER_COUNT; regB++) {
    // Arrange
    setUp();
    *getRegisterPtr(&processState.registers, (enum Register)regB) = 0x1234;
    processState.memory[0] = OPCODE_VALUES[MOV];
    processState.memory[1] = REGISTER_CODES[NL] << 4 | REGISTER_CODES[regB];
    
    initializeExpectedEndState();
    expectedEndState.registers.ip = 0x0002;

    // Act
    stepProcess(&processState);

    // Assert
    TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
  }
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_nop_should_doNothing);
  RUN_TEST(test_jmp_should_jumpToAddressAndSaveReturnAddress);
  RUN_TEST(test_jmz_should_jumpToAddress_when_acIsZero);
  RUN_TEST(test_jmz_should_doNothing_when_acIsNonZero);
  RUN_TEST(test_mov_should_copyValueFromRegisterBToRegisterA_when_neitherRegisterIsNull);
  RUN_TEST(test_mov_should_setRegisterAToZero_when_registerAIsNotNullAndRegisterBIsNull);
  RUN_TEST(test_mov_should_doNothing_when_registerAIsNull);
  return UNITY_END();
}

/*
Example program:
  memory[0] = OPCODE_VALUES[LDIW];
  memory[1] = 0x01;
  memory[2] = 0x00;
  
  memory[3] = OPCODE_VALUES[MOV];
  memory[4] = REGISTER_CODES[X0] << 4 | REGISTER_CODES[AC];
  
  memory[5] = OPCODE_VALUES[ADD];
  memory[6] = REGISTER_CODES[AC] << 4 | REGISTER_CODES[X0];

  memory[7] = OPCODE_VALUES[PSHW];
  memory[8] = REGISTER_CODES[AC] << 4;

  memory[9] = OPCODE_VALUES[JMP];
  memory[10] = 0x05;
  memory[11] = 0x00;
*/
