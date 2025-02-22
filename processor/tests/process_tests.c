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
  for (unsigned int i = 0; i < MEMORY_SIZE; i += 1) {
    // Fill memory with the lower 8 bits of the address as canary for memory access bugs
    processState.memory[i] = (unsigned char)(i & 0xFF);
  }
}

void initializeExpectedEndState() {
  memcpy(&expectedEndState, &processState, sizeof(processState));
}

void tearDown() { }

#pragma region Control flow instructions

void test_nop_should_doNothing(void) {
  // Arrange
  processState.memory[0] = OPCODE_VALUES[NOP];

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0001;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState)); // TODO: Create helper assertion that provides more informative messages
}

void test_jmp_should_jumpToAddressAndSaveReturnAddress(void) {
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

void test_jmz_should_jumpToAddress_when_acIsZero(void) {
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

void test_jmz_should_doNothing_when_acIsNonZero(void) {
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

#pragma endregion

#pragma region Memory instructions

void test_mov_should_copyValueFromRegisterBToRegisterA_when_neitherRegisterIsNull(void) {
  for (unsigned int regA = 1; regA < REGISTER_COUNT; regA++) {
    for (unsigned int regB = 1; regB < REGISTER_COUNT; regB++) {
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

void test_mov_should_setRegisterAToZero_when_registerBIsNull(void) {
  for (unsigned int regA = 1; regA < REGISTER_COUNT; regA++) {
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

void test_mov_should_doNothing_when_registerAIsNull(void) {
  for (unsigned int regB = 1; regB < REGISTER_COUNT; regB++) {
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

void test_mov_should_doNothing_when_bothRegistersAreNull(void) {
  // Arrange
  setUp();
  processState.memory[0] = OPCODE_VALUES[MOV];
  processState.memory[1] = REGISTER_CODES[NL] << 4 | REGISTER_CODES[NL];
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_ldib_should_loadImmediateByteIntoAc(void) {
  // Arrange
  processState.memory[0] = OPCODE_VALUES[LDIB];
  processState.memory[1] = 0x12;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0012;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_ldiw_should_loadImmediateWordIntoAc(void) {
  // Arrange
  processState.memory[0] = OPCODE_VALUES[LDIW];
  processState.memory[1] = 0x34;
  processState.memory[2] = 0x12;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.ac = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_ldmb_should_loadMemoryByteAtAddressIntoAc_when_immediateValueIsZero(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[LDMB];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x0;
  processState.memory[0x1234] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0056;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_ldmb_should_loadMemoryByteAtAddressWithOffsetIntoAc_when_immediateValueIsPositive(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[LDMB];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x7;
  processState.memory[0x123B] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0056;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_ldmb_should_loadMemoryByteAtAddressWithOffsetIntoAc_when_immediateValueIsNegative(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[LDMB];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x8; // 0x8 = -8 as a nibble
  processState.memory[0x122C] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0056;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_ldmw_should_loadMemoryWordAtAddressIntoAc_when_immediateValueIsZero(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[LDMW];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x0;
  processState.memory[0x1234] = 0x78;
  processState.memory[0x1235] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_ldmw_should_loadMemoryWordAtAddressWithOffsetIntoAc_when_immediateValueIsPositive(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[LDMW];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x7;
  processState.memory[0x123B] = 0x78;
  processState.memory[0x123C] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_ldmw_should_loadMemoryWordAtAddressWithOffsetIntoAc_when_immediateValueIsNegative(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[LDMW];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x8; // 0x8 = -8 as a nibble
  processState.memory[0x122C] = 0x78;
  processState.memory[0x122D] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_stmb_should_storeLowerAcIntoMemoryByteAtAddress_when_immediateValueIsZero(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[STMB];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x0;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x1234] = 0x78;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_stmb_should_storeLowerAcIntoMemoryByteAtAddressWithOffset_when_immediateValueIsPositive(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[STMB];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x7;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x123B] = 0x78;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_stmb_should_storeLowerAcIntoMemoryByteAtAddressWithOffset_when_immediateValueIsNegative(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[STMB];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x8; // 0x8 = -8 as a nibble

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x122C] = 0x78;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_stmw_should_storeFullAcIntoMemoryWordAtAddress_when_immediateValueIsZero(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[STMW];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x0;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x1234] = 0x78;
  expectedEndState.memory[0x1235] = 0x56;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_stmw_should_storeFullAcIntoMemoryWordAtAddressWithOffset_when_immediateValueIsPositive(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[STMW];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x7;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x123B] = 0x78;
  expectedEndState.memory[0x123C] = 0x56;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_stmw_should_storeFullAcIntoMemoryWordAtAddressWithOffset_when_immediateValueIsNegative(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  processState.memory[0] = OPCODE_VALUES[STMW];
  processState.memory[1] = REGISTER_CODES[X0] << 4 | 0x8; // 0x8 = -8 as a nibble

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x122C] = 0x78;
  expectedEndState.memory[0x122D] = 0x56;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_pshb_should_pushLowerRegisterByteOntoStack_when_registerIsNotSp(void) {
  for (unsigned int i = 1; i < REGISTER_COUNT; i++) {
    if (i == SP) { continue; }

    // Arrange
    setUp();
    processState.registers.sp = 0x0000;
    *getRegisterPtr(&processState.registers, (enum Register)i) = 0x1234;
    processState.memory[0] = OPCODE_VALUES[PSHB];
    processState.memory[1] = REGISTER_CODES[i] << 4;
  
    initializeExpectedEndState();
    expectedEndState.registers.ip = 0x0002;
    expectedEndState.registers.sp = 0xFFFF;
    expectedEndState.memory[0xFFFF] = 0x34;
  
    // Act
    stepProcess(&processState);
  
    // Assert
    TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
  }
}

void test_pshb_should_pushLowerSpOntoStackBeforeDecrement_when_registerIsSp(void) {
  // Arrange
  processState.registers.sp = 0x1234;
  processState.memory[0] = OPCODE_VALUES[PSHB];
  processState.memory[1] = REGISTER_CODES[SP] << 4;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0x1233;
  expectedEndState.memory[0x1233] = 0x34;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_pshw_should_pushFullRegisterWordOntoStack_when_registerIsNotSp(void) {
  for (unsigned int i = 1; i < REGISTER_COUNT; i++) {
    if (i == SP) { continue; }

    // Arrange
    setUp();
    processState.registers.sp = 0x0000;
    *getRegisterPtr(&processState.registers, (enum Register)i) = 0x1234;
    processState.memory[0] = OPCODE_VALUES[PSHW];
    processState.memory[1] = REGISTER_CODES[i] << 4;
  
    initializeExpectedEndState();
    expectedEndState.registers.ip = 0x0002;
    expectedEndState.registers.sp = 0xFFFE;
    expectedEndState.memory[0xFFFE] = 0x34;
    expectedEndState.memory[0xFFFF] = 0x12;
  
    // Act
    stepProcess(&processState);
  
    // Assert
    TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
  }
}

void test_pshw_should_pushFullSpOntoStackBeforeDecrement_when_registerIsSp(void) {
  // Arrange
  processState.registers.sp = 0x1234;
  processState.memory[0] = OPCODE_VALUES[PSHW];
  processState.memory[1] = REGISTER_CODES[SP] << 4;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0x1232;
  expectedEndState.memory[0x1232] = 0x34;
  expectedEndState.memory[0x1233] = 0x12;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_popb_should_popStackByteIntoRegister_when_registerIsNotSp(void) {
  for (unsigned int i = 1; i < REGISTER_COUNT; i++) {
    if (i == SP) { continue; }

    // Arrange
    setUp();
    processState.registers.sp = 0xFFFE;
    processState.memory[0] = OPCODE_VALUES[POPB];
    processState.memory[1] = REGISTER_CODES[i] << 4;
    processState.memory[0xFFFE] = 0x34;
    processState.memory[0xFFFF] = 0x12;
  
    initializeExpectedEndState();
    expectedEndState.registers.ip = 0x0002;
    expectedEndState.registers.sp = 0xFFFF;
    *getRegisterPtr(&expectedEndState.registers, (enum Register)i) = 0x0034; // If the register is ip, this replaces the value above
  
    // Act
    stepProcess(&processState);
  
    // Assert
    TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
  }
}

void test_popb_should_popStackByteIntoSpAfterIncrement_when_registerIsSp(void) {
  // Arrange
  processState.registers.sp = 0xFFFE;
  processState.memory[0] = OPCODE_VALUES[POPB];
  processState.memory[1] = REGISTER_CODES[SP] << 4;
  processState.memory[0xFFFE] = 0x34;
  processState.memory[0xFFFF] = 0x12;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0x0034;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

void test_popw_should_popStackWordIntoRegister_when_registerIsNotSp(void) {
  for (unsigned int i = 1; i < REGISTER_COUNT; i++) {
    if (i == SP) { continue; }

    // Arrange
    setUp();
    processState.registers.sp = 0xFFFE;
    processState.memory[0] = OPCODE_VALUES[POPW];
    processState.memory[1] = REGISTER_CODES[i] << 4;
    processState.memory[0xFFFE] = 0x34;
    processState.memory[0xFFFF] = 0x12;
  
    initializeExpectedEndState();
    expectedEndState.registers.ip = 0x0002;
    expectedEndState.registers.sp = 0x0000;
    *getRegisterPtr(&expectedEndState.registers, (enum Register)i) = 0x1234; // If the register is ip, this replaces the value above
  
    // Act
    stepProcess(&processState);
  
    // Assert
    TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
  }
}

void test_popw_should_popStackWordIntoSpAfterIncrement_when_registerIsSp(void) {
  // Arrange
  processState.registers.sp = 0xFFFE;
  processState.memory[0] = OPCODE_VALUES[POPW];
  processState.memory[1] = REGISTER_CODES[SP] << 4;
  processState.memory[0xFFFE] = 0x34;
  processState.memory[0xFFFF] = 0x12;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_MEMORY(&expectedEndState, &processState, sizeof(processState));
}

#pragma endregion

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_nop_should_doNothing);
  RUN_TEST(test_jmp_should_jumpToAddressAndSaveReturnAddress);
  RUN_TEST(test_jmz_should_jumpToAddress_when_acIsZero);
  RUN_TEST(test_jmz_should_doNothing_when_acIsNonZero);
  RUN_TEST(test_mov_should_copyValueFromRegisterBToRegisterA_when_neitherRegisterIsNull);
  RUN_TEST(test_mov_should_setRegisterAToZero_when_registerBIsNull);
  RUN_TEST(test_mov_should_doNothing_when_registerAIsNull);
  RUN_TEST(test_mov_should_doNothing_when_bothRegistersAreNull);
  RUN_TEST(test_ldib_should_loadImmediateByteIntoAc);
  RUN_TEST(test_ldiw_should_loadImmediateWordIntoAc);
  RUN_TEST(test_ldmb_should_loadMemoryByteAtAddressIntoAc_when_immediateValueIsZero);
  RUN_TEST(test_ldmb_should_loadMemoryByteAtAddressWithOffsetIntoAc_when_immediateValueIsPositive);
  RUN_TEST(test_ldmb_should_loadMemoryByteAtAddressWithOffsetIntoAc_when_immediateValueIsNegative);
  RUN_TEST(test_ldmw_should_loadMemoryWordAtAddressIntoAc_when_immediateValueIsZero);
  RUN_TEST(test_ldmw_should_loadMemoryWordAtAddressWithOffsetIntoAc_when_immediateValueIsPositive);
  RUN_TEST(test_ldmw_should_loadMemoryWordAtAddressWithOffsetIntoAc_when_immediateValueIsNegative);
  RUN_TEST(test_stmb_should_storeLowerAcIntoMemoryByteAtAddress_when_immediateValueIsZero);
  RUN_TEST(test_stmb_should_storeLowerAcIntoMemoryByteAtAddressWithOffset_when_immediateValueIsPositive);
  RUN_TEST(test_stmb_should_storeLowerAcIntoMemoryByteAtAddressWithOffset_when_immediateValueIsNegative);
  RUN_TEST(test_stmw_should_storeFullAcIntoMemoryWordAtAddress_when_immediateValueIsZero);
  RUN_TEST(test_stmw_should_storeFullAcIntoMemoryWordAtAddressWithOffset_when_immediateValueIsPositive);
  RUN_TEST(test_stmw_should_storeFullAcIntoMemoryWordAtAddressWithOffset_when_immediateValueIsNegative);
  RUN_TEST(test_pshb_should_pushLowerRegisterByteOntoStack_when_registerIsNotSp);
  RUN_TEST(test_pshb_should_pushLowerSpOntoStackBeforeDecrement_when_registerIsSp);
  RUN_TEST(test_pshw_should_pushFullRegisterWordOntoStack_when_registerIsNotSp);
  RUN_TEST(test_pshw_should_pushFullSpOntoStackBeforeDecrement_when_registerIsSp);
  RUN_TEST(test_popb_should_popStackByteIntoRegister_when_registerIsNotSp);
  RUN_TEST(test_popb_should_popStackByteIntoSpAfterIncrement_when_registerIsSp);
  RUN_TEST(test_popw_should_popStackWordIntoRegister_when_registerIsNotSp);
  RUN_TEST(test_popw_should_popStackWordIntoSpAfterIncrement_when_registerIsSp);
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
