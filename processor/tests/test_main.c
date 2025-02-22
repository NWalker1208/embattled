#include <unity.h>
#include <string.h>
#include "processor/process.h"
#include "processor/instruction.h"

unsigned char memory[MEMORY_SIZE];
struct ProcessState processState;

void setUp() {
  memset(memory, 0, sizeof(memory));
  memset(&processState, 0, sizeof(processState));
}

void tearDown() { }

void test_nop_should_doNothing() {
  // Arrange
  memory[0] = OPCODE_VALUES[NOP];

  // Act
  stepProcess(memory, &processState);

  // Assert
  TEST_ASSERT_EQUAL(0x0001, processState.ip);
  TEST_ASSERT_EACH_EQUAL_UINT8(0x00, memory, MEMORY_SIZE);
}

void test_jmp_should_jumpToAddressAndSaveReturnAddress() {
  // Arrange
  memory[0] = OPCODE_VALUES[JMP];
  memory[1] = 0x34;
  memory[2] = 0x12;

  // Act
  stepProcess(memory, &processState);

  // Assert
  TEST_ASSERT_EQUAL(0x1234, processState.ip);
  TEST_ASSERT_EQUAL(0x0003, processState.ac);
}

void test_jmz_should_jumpToAddressIfAcIsZero() {
  // Arrange
  processState.ac = 0x0000;

  memory[0] = OPCODE_VALUES[JMZ];
  memory[1] = 0x34;
  memory[2] = 0x12;

  // Act
  stepProcess(memory, &processState);

  // Assert
  TEST_ASSERT_EQUAL(0x1234, processState.ip);
  TEST_ASSERT_EQUAL(0x0000, processState.ac);
}

void test_jmz_should_doNothingIfAcIsNonZero() {
  // Arrange
  processState.ac = 0x0001;

  memory[0] = OPCODE_VALUES[JMZ];
  memory[1] = 0x34;
  memory[2] = 0x12;

  // Act
  stepProcess(memory, &processState);

  // Assert
  TEST_ASSERT_EQUAL(0x0003, processState.ip);
  TEST_ASSERT_EQUAL(0x0001, processState.ac);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_nop_should_doNothing);
  RUN_TEST(test_jmp_should_jumpToAddressAndSaveReturnAddress);
  RUN_TEST(test_jmz_should_jumpToAddressIfAcIsZero);
  RUN_TEST(test_jmz_should_doNothingIfAcIsNonZero);
  return UNITY_END();
}

/*
Example program:
  memory[0] = OPCODE_VALUES[LDIW];
  memory[1] = 0x01;
  memory[2] = 0x00;
  
  memory[3] = OPCODE_VALUES[MOV];
  memory[4] = REGISTER_IDS[X0] << 4 | REGISTER_IDS[AC];
  
  memory[5] = OPCODE_VALUES[ADD];
  memory[6] = REGISTER_IDS[AC] << 4 | REGISTER_IDS[X0];

  memory[7] = OPCODE_VALUES[PSHW];
  memory[8] = REGISTER_IDS[AC] << 4;

  memory[9] = OPCODE_VALUES[JMP];
  memory[10] = 0x05;
  memory[11] = 0x00;
*/
