#define UNITY_SUPPORT_TEST_CASES
#include <unity.h>
#include <string.h>
#include "custom_assertions.h"
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

#pragma region Control flow

void test_nop_should_doNothing(void) {
  // Arrange
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_NOP,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0001;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_jmp_r_should_jumpToAddressAndSaveReturnAddress(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_JMP_R,
    .operands.registerA = REGISTER_X0,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;
  expectedEndState.registers.rt = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_jmp_i_should_jumpToAddressAndSaveReturnAddress(void) {
  // Arrange
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_JMP_I,
    .operands.immediateA.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;
  expectedEndState.registers.rt = 0x0003;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_jmz_r_should_jumpToAddress_when_registerAIsZero(void) {
  // Arrange
  processState.registers.x0 = 0;
  processState.registers.x1 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_JMZ_R,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_jmz_r_should_doNothing_when_registerAIsNonZero(void) {
  // Arrange
  processState.registers.x0 = 0x0001;
  processState.registers.x1 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_JMZ_R,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_jmz_i_should_jumpToAddress_when_registerAIsZero(void) {
  // Arrange
  processState.registers.x0 = 0x0000;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_JMZ_I,
    .operands.registerA = REGISTER_X0,
    .operands.immediateA.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_jmz_i_should_doNothing_when_registerAIsNonZero(void) {
  // Arrange
  processState.registers.x0 = 0x0001;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_JMZ_I,
    .operands.registerA = REGISTER_X0,
    .operands.immediateA.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

#pragma endregion

#pragma region Memory

TEST_MATRIX(
  [REGISTER_SP, REGISTER_RT, REGISTER_X0, REGISTER_X1, REGISTER_X2, REGISTER_X3, REGISTER_X4, REGISTER_X5, REGISTER_X6, REGISTER_X7, REGISTER_X8, REGISTER_X9, REGISTER_X10, REGISTER_X11],
  [REGISTER_SP, REGISTER_RT, REGISTER_X0, REGISTER_X1, REGISTER_X2, REGISTER_X3, REGISTER_X4, REGISTER_X5, REGISTER_X6, REGISTER_X7, REGISTER_X8, REGISTER_X9, REGISTER_X10, REGISTER_X11])
void test_set_r_should_copyValueFromRegisterBToRegisterA_when_neitherRegisterIsNullOrIp(Register regA, Register regB) {
  // Arrange
  setUp();
  *getRegisterPtr(&processState.registers, regA) = 0x1234;
  *getRegisterPtr(&processState.registers, regB) = 0x5678;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SET_R,
    .operands.registerA = regA,
    .operands.registerB = regB,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  *getRegisterPtr(&expectedEndState.registers, regA) = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_set_r_should_copyValueFromRegisterBToIp_when_registerAIsIp(void) {
  // Arrange
  processState.registers.x3 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SET_R,
    .operands.registerA = REGISTER_IP,
    .operands.registerB = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_set_r_should_copyValueFromIpToRegisterA_when_registerBIsIp(void) {
  // Arrange
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SET_R,
    .operands.registerA = REGISTER_X3,
    .operands.registerB = REGISTER_IP,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.x3 = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_set_r_should_setRegisterAToZero_when_registerBIsNull(void) {
  // Arrange
  processState.registers.x7 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SET_R,
    .operands.registerA = REGISTER_X7,
    .operands.registerB = REGISTER_NL,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.x7 = 0x0000;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_set_r_should_doNothing_when_registerAIsNull(void) {
  // Arrange
  processState.registers.x7 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SET_R,
    .operands.registerA = REGISTER_NL,
    .operands.registerB = REGISTER_X7,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_set_r_should_doNothing_when_bothRegistersAreNull(void) {
  // Arrange
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SET_R,
    .operands.registerA = REGISTER_NL,
    .operands.registerB = REGISTER_NL,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_MATRIX([REGISTER_IP, REGISTER_SP, REGISTER_RT, REGISTER_X0, REGISTER_X1, REGISTER_X2, REGISTER_X3, REGISTER_X4, REGISTER_X5, REGISTER_X6, REGISTER_X7, REGISTER_X8, REGISTER_X9, REGISTER_X10, REGISTER_X11])
void test_set_i_should_copyImmediateValueToRegisterA_when_registerAIsNotNull(Register regA) {
  // Arrange
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SET_I,
    .operands.registerA = regA,
    .operands.immediateA.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  *getRegisterPtr(&expectedEndState.registers, regA) = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldb_r_should_loadMemoryByteAtAddressInRegisterBIntoRegisterA(void) {
  // Arrange
  processState.registers.x1 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_LDB_R,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
  });
  processState.memory[0x1234] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.x0 = 0x0056;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldb_i_should_loadMemoryByteAtAddressInImmediateAIntoRegisterA(void) {
  // Arrange
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_LDB_I,
    .operands.registerA = REGISTER_X0,
    .operands.immediateA.u16 = 0x1234,
  });
  processState.memory[0x1234] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x0 = 0x0056;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldw_r_should_loadMemoryWordAtAddressInRegisterBIntoRegisterA(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.x1 = 0xFFFF;
  writeInstruction(processState.memory, 1, (Instruction){
    .opcode = OPCODE_LDW_R,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
  });
  processState.memory[0xFFFF] = 0x78;
  processState.memory[0x0000] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x0 = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldw_i_should_loadMemoryWordAtAddressInImmediateAIntoRegisterA(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  writeInstruction(processState.memory, 1, (Instruction){
    .opcode = OPCODE_LDW_I,
    .operands.registerA = REGISTER_X0,
    .operands.immediateA.u16 = 0xFFFF,
  });
  processState.memory[0xFFFF] = 0x78;
  processState.memory[0x0000] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0005;
  expectedEndState.registers.x0 = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stb_rr_should_storeLowerRegisterAIntoMemoryByteAtAddressInRegisterB(void) {
  // Arrange
  processState.registers.x0 = 0xABCD;
  processState.registers.x1 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_STB_RR,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x1234] = 0xCD;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stb_ri_should_storeLowerRegisterAIntoMemoryByteAtAddressInImmediateA(void) {
  // Arrange
  processState.registers.x0 = 0xABCD;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_STB_RI,
    .operands.registerA = REGISTER_X0,
    .operands.immediateA.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.memory[0x1234] = 0xCD;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stb_ir_should_storeImmediateAIntoMemoryByteAtAddressInRegisterA(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_STB_IR,
    .operands.immediateA.u16 = 0xABCD,
    .operands.registerA = REGISTER_X0,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.memory[0x1234] = 0xCD;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stb_ii_should_storeImmediateAIntoMemoryByteAtAddressInImmediateB(void) {
  // Arrange
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_STB_II,
    .operands.immediateA.u16 = 0xABCD,
    .operands.immediateB.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.memory[0x1234] = 0xCD;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stw_rr_should_storeRegisterAIntoMemoryWordAtAddressInRegisterB(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.x2 = 0x1234;
  processState.registers.x3 = 0xFFFF;
  writeInstruction(processState.memory, 1, (Instruction){
    .opcode = OPCODE_STW_RR,
    .operands.registerA = REGISTER_X2,
    .operands.registerB = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.memory[0xFFFF] = 0x34;
  expectedEndState.memory[0x0000] = 0x12;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stw_ri_should_storeRegisterAIntoMemoryWordAtAddressInImmediateA(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.x2 = 0x1234;
  writeInstruction(processState.memory, 1, (Instruction){
    .opcode = OPCODE_STW_RI,
    .operands.registerA = REGISTER_X2,
    .operands.immediateA.u16 = 0xFFFF,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0005;
  expectedEndState.memory[0xFFFF] = 0x34;
  expectedEndState.memory[0x0000] = 0x12;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stw_ir_should_storeImmediateAIntoMemoryWordAtAddressInRegisterA(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.x3 = 0xFFFF;
  writeInstruction(processState.memory, 1, (Instruction){
    .opcode = OPCODE_STW_IR,
    .operands.immediateA.u16 = 0x1234,
    .operands.registerA = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0005;
  expectedEndState.memory[0xFFFF] = 0x34;
  expectedEndState.memory[0x0000] = 0x12;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stw_ii_should_storeImmediateAIntoMemoryWordAtAddressInImmediateB(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  writeInstruction(processState.memory, 1, (Instruction){
    .opcode = OPCODE_STW_II,
    .operands.immediateA.u16 = 0x1234,
    .operands.immediateB.u16 = 0xFFFF,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0006;
  expectedEndState.memory[0xFFFF] = 0x34;
  expectedEndState.memory[0x0000] = 0x12;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_pshb_should_pushLowerRegisterAByteOntoStack_when_registerAIsNotSp(void) {
  // Arrange
  processState.registers.sp = 0x0000;
  processState.registers.x4 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_PSHB,
    .operands.registerA = REGISTER_X4,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0xFFFF;
  expectedEndState.memory[0xFFFF] = 0x34;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_pshb_should_pushLowerSpOntoStackBeforeDecrement_when_registerAIsSp(void) {
  // Arrange
  processState.registers.sp = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_PSHB,
    .operands.registerA = REGISTER_SP,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0x1233;
  expectedEndState.memory[0x1233] = 0x34;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0000, 0xFFFE, 0xFFFF)
TEST_CASE(0x0001, 0xFFFF, 0x0000)
void test_pshw_should_pushRegisterAOntoStack_when_registerAIsNotSp(uint16_t initialSp, uint16_t lowerAddressWritten, uint16_t upperAddressWritten) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.sp = initialSp;
  processState.registers.x4 = 0x1234;
  writeInstruction(processState.memory, 1, (Instruction){
    .opcode = OPCODE_PSHW,
    .operands.registerA = REGISTER_X4,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.sp = lowerAddressWritten;
  expectedEndState.memory[lowerAddressWritten] = 0x34;
  expectedEndState.memory[upperAddressWritten] = 0x12;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_pshw_should_pushSpOntoStackBeforeDecrement_when_registerAIsSp(void) {
  // Arrange
  processState.registers.sp = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_PSHW,
    .operands.registerA = REGISTER_SP,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0x1232;
  expectedEndState.memory[0x1232] = 0x34;
  expectedEndState.memory[0x1233] = 0x12;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_popb_should_popStackByteIntoRegisterA_when_registerAIsNotSp(void) {
  // Arrange
  processState.registers.sp = 0xFFFE;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_POPB,
    .operands.registerA = REGISTER_X6,
  });
  processState.memory[0xFFFE] = 0x34;
  processState.memory[0xFFFF] = 0x12;
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0xFFFF;
  expectedEndState.registers.x6 = 0x0034;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_popb_should_popStackByteIntoSpAfterIncrement_when_registerAIsSp(void) {
  // Arrange
  processState.registers.sp = 0xFFFE;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_POPB,
    .operands.registerA = REGISTER_SP,
  });
  processState.memory[0xFFFE] = 0x34;
  processState.memory[0xFFFF] = 0x12;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0x0034;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0xFFFE, 0xFFFF, 0x0000)
TEST_CASE(0xFFFF, 0x0000, 0x0001)
void test_popw_should_popStackWordIntoRegisterA_when_registerAIsNotSp(uint16_t lowerAddressRead, uint16_t upperAddressRead, uint16_t expectedSp) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.sp = lowerAddressRead;
  writeInstruction(processState.memory, 1, (Instruction){
    .opcode = OPCODE_POPW,
    .operands.registerA = REGISTER_X6,
  });
  processState.memory[lowerAddressRead] = 0x34;
  processState.memory[upperAddressRead] = 0x12;
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.sp = expectedSp;
  expectedEndState.registers.x6 = 0x1234;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_popw_should_popStackWordIntoSpAfterIncrement_when_registerAIsSp(void) {
  // Arrange
  processState.registers.sp = 0xFFFE;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_POPW,
    .operands.registerA = REGISTER_SP,
  });
  processState.memory[0xFFFE] = 0x34;
  processState.memory[0xFFFF] = 0x12;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

#pragma endregion

#pragma region Math

void test_add_r_should_setRegisterAToRegisterBPlusRegisterC(void) {
  // Arrange
  processState.registers.x2 = 0x1234;
  processState.registers.x3 = 0x5678;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_ADD_R,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.registerC = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x1 = 0x68AC;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_add_i_should_setRegisterAToRegisterBPlusImmediateA(void) {
  // Arrange
  processState.registers.x2 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_ADD_I,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.immediateA.u16 = 0x5678,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = 0x68AC;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_sub_rr_should_setRegisterAToRegisterBMinusRegisterC(void) {
  // Arrange
  processState.registers.x2 = 0x5678;
  processState.registers.x3 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SUB_RR,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.registerC = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x1 = 0x4444;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_sub_ri_should_setRegisterAToRegisterBMinusImmediateA(void) {
  // Arrange
  processState.registers.x2 = 0x5678;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SUB_RI,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.immediateA.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = 0x4444;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_sub_ir_should_setRegisterAToImmediateAMinusRegisterB(void) {
  // Arrange
  processState.registers.x2 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_SUB_IR,
    .operands.registerA = REGISTER_X1,
    .operands.immediateA.u16 = 0x5678,
    .operands.registerB = REGISTER_X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = 0x4444;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_mul_r_should_setRegisterAToRegisterBTimesRegisterC(void) {
  // Arrange
  processState.registers.x2 = 0x1234;
  processState.registers.x3 = 0x5678;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_MUL_R,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.registerC = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x1 = 0x0060;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_mul_i_should_setRegisterAToRegisterBTimesImmediateA(void) {
  // Arrange
  processState.registers.x2 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_MUL_I,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.immediateA.u16 = 0x5678,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = 0x0060;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0004)
TEST_CASE(0x8765, 0x1234, 0xFFFA)
TEST_CASE(0x5678, 0xFEDC, 0xFFB5)
TEST_CASE(0xBA98, 0xFEDC, 0x003C)
TEST_CASE(0x0001, 0x0000, 0x7FFF)
TEST_CASE(0xFFFF, 0x0000, 0x8000)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_divs_rr_should_setRegisterAToRegisterBSignedDividedByRegisterCSigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueA;
  processState.registers.x3 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_DIVS_RR,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.registerC = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0004)
TEST_CASE(0x8765, 0x1234, 0xFFFA)
TEST_CASE(0x5678, 0xFEDC, 0xFFB5)
TEST_CASE(0xBA98, 0xFEDC, 0x003C)
TEST_CASE(0x0001, 0x0000, 0x7FFF)
TEST_CASE(0xFFFF, 0x0000, 0x8000)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_divs_ri_should_setRegisterAToRegisterBSignedDividedByImmediateASigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_DIVS_RI,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.immediateA.u16 = valueB,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0004)
TEST_CASE(0x8765, 0x1234, 0xFFFA)
TEST_CASE(0x5678, 0xFEDC, 0xFFB5)
TEST_CASE(0xBA98, 0xFEDC, 0x003C)
TEST_CASE(0x0001, 0x0000, 0x7FFF)
TEST_CASE(0xFFFF, 0x0000, 0x8000)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_divs_ir_should_setRegisterAToImmediateASignedDividedByRegisterBSigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_DIVS_IR,
    .operands.registerA = REGISTER_X1,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0004)
TEST_CASE(0x8765, 0x1234, 0x0007)
TEST_CASE(0x5678, 0xFEDC, 0x0000)
TEST_CASE(0xBA98, 0xFEDC, 0x0000)
TEST_CASE(0x0001, 0x0000, 0xFFFF)
TEST_CASE(0xFFFF, 0x0000, 0xFFFF)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_divu_rr_should_setRegisterAToRegisterBUnsignedDividedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueA;
  processState.registers.x3 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_DIVU_RR,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.registerC = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0004)
TEST_CASE(0x8765, 0x1234, 0x0007)
TEST_CASE(0x5678, 0xFEDC, 0x0000)
TEST_CASE(0xBA98, 0xFEDC, 0x0000)
TEST_CASE(0x0001, 0x0000, 0xFFFF)
TEST_CASE(0xFFFF, 0x0000, 0xFFFF)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_divu_ri_should_setRegisterAToRegisterBUnsignedDividedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_DIVU_RI,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.immediateA.u16 = valueB,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0004)
TEST_CASE(0x8765, 0x1234, 0x0007)
TEST_CASE(0x5678, 0xFEDC, 0x0000)
TEST_CASE(0xBA98, 0xFEDC, 0x0000)
TEST_CASE(0x0001, 0x0000, 0xFFFF)
TEST_CASE(0xFFFF, 0x0000, 0xFFFF)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_divu_ir_should_setRegisterAToRegisterBUnsignedDividedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_DIVU_IR,
    .operands.registerA = REGISTER_X1,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0DA8)
TEST_CASE(0x8765, 0x1234, 0xF49D)
TEST_CASE(0x5678, 0xFEDC, 0x00EC)
TEST_CASE(0xBA98, 0xFEDC, 0xFF08)
TEST_CASE(0x0001, 0x0000, 0x0001)
TEST_CASE(0xFFFF, 0x0000, 0xFFFF)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_rems_rr_should_setRegisterAToRemainderOfRegisterBSignedDividedByRegisterCSigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueA;
  processState.registers.x3 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_REMS_RR,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.registerC = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0DA8)
TEST_CASE(0x8765, 0x1234, 0xF49D)
TEST_CASE(0x5678, 0xFEDC, 0x00EC)
TEST_CASE(0xBA98, 0xFEDC, 0xFF08)
TEST_CASE(0x0001, 0x0000, 0x0001)
TEST_CASE(0xFFFF, 0x0000, 0xFFFF)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_rems_ri_should_setRegisterAToRemainderOfRegisterBSignedDividedByImmediateASigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_REMS_RI,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.immediateA.u16 = valueB,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0DA8)
TEST_CASE(0x8765, 0x1234, 0xF49D)
TEST_CASE(0x5678, 0xFEDC, 0x00EC)
TEST_CASE(0xBA98, 0xFEDC, 0xFF08)
TEST_CASE(0x0001, 0x0000, 0x0001)
TEST_CASE(0xFFFF, 0x0000, 0xFFFF)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_rems_ir_should_setRegisterAToRemainderOfImmediateASignedDividedByRegisterBSigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_REMS_IR,
    .operands.registerA = REGISTER_X1,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0DA8)
TEST_CASE(0x8765, 0x1234, 0x07F9)
TEST_CASE(0x5678, 0xFEDC, 0x5678)
TEST_CASE(0xBA98, 0xFEDC, 0xBA98)
TEST_CASE(0x0001, 0x0000, 0x0001)
TEST_CASE(0xFFFF, 0x0000, 0xFFFF)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_remu_rr_should_setRegisterAToRemainderOfRegisterBDividedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueA;
  processState.registers.x3 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_REMU_RR,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.registerC = REGISTER_X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0DA8)
TEST_CASE(0x8765, 0x1234, 0x07F9)
TEST_CASE(0x5678, 0xFEDC, 0x5678)
TEST_CASE(0xBA98, 0xFEDC, 0xBA98)
TEST_CASE(0x0001, 0x0000, 0x0001)
TEST_CASE(0xFFFF, 0x0000, 0xFFFF)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_remu_ri_should_setRegisterAToRemainderOfRegisterBDividedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_REMU_RI,
    .operands.registerA = REGISTER_X1,
    .operands.registerB = REGISTER_X2,
    .operands.immediateA.u16 = valueB,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x1234, 0x0DA8)
TEST_CASE(0x8765, 0x1234, 0x07F9)
TEST_CASE(0x5678, 0xFEDC, 0x5678)
TEST_CASE(0xBA98, 0xFEDC, 0xBA98)
TEST_CASE(0x0001, 0x0000, 0x0001)
TEST_CASE(0xFFFF, 0x0000, 0xFFFF)
TEST_CASE(0x0000, 0x0000, 0x0000)
void test_remu_ir_should_setRegisterAToRemainderOfRegisterBDividedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x2 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_REMU_IR,
    .operands.registerA = REGISTER_X1,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x1 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

#pragma endregion

#pragma region Bitwise logic

void test_and_r_should_setRegisterAToRegisterBBitwiseAndRegisterC(void) {
  // Arrange
  processState.registers.x5 = 0x1234;
  processState.registers.x6 = 0x8765;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_AND_R,
    .operands.registerA = REGISTER_X4,
    .operands.registerB = REGISTER_X5,
    .operands.registerC = REGISTER_X6,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x4 = 0x0224;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_and_i_should_setRegisterAToRegisterBBitwiseAndImmediateA(void) {
  // Arrange
  processState.registers.x5 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_AND_I,
    .operands.registerA = REGISTER_X4,
    .operands.registerB = REGISTER_X5,
    .operands.immediateA.u16 = 0x8765,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x4 = 0x0224;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ior_r_should_setRegisterAToRegisterBBitwiseInclusiveOrRegisterC(void) {
  // Arrange
  processState.registers.x5 = 0x1234;
  processState.registers.x6 = 0x8765;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_IOR_R,
    .operands.registerA = REGISTER_X4,
    .operands.registerB = REGISTER_X5,
    .operands.registerC = REGISTER_X6,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x4 = 0x9775;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ior_i_should_setRegisterAToRegisterBBitwiseInclusiveOrRegisterC(void) {
  // Arrange
  processState.registers.x5 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_IOR_I,
    .operands.registerA = REGISTER_X4,
    .operands.registerB = REGISTER_X5,
    .operands.immediateA.u16 = 0x8765,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x4 = 0x9775;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_xor_r_should_setRegisterAToRegisterBBitwiseExclusiveOrRegisterC(void) {
  // Arrange
  processState.registers.x5 = 0x1234;
  processState.registers.x6 = 0x8765;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_XOR_R,
    .operands.registerA = REGISTER_X4,
    .operands.registerB = REGISTER_X5,
    .operands.registerC = REGISTER_X6,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x4 = 0x9551;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_xor_i_should_setRegisterAToRegisterBBitwiseExclusiveOrImmediateA(void) {
  // Arrange
  processState.registers.x5 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_XOR_I,
    .operands.registerA = REGISTER_X4,
    .operands.registerB = REGISTER_X5,
    .operands.immediateA.u16 = 0x8765,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x4 = 0x9551;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x0005, 0xCF00)
TEST_CASE(0x5678, 0xFFFB, 0x0000)
void test_lsh_rr_should_setRegisterAToRegisterBLeftShiftedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x8 = valueA;
  processState.registers.x9 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_LSH_RR,
    .operands.registerA = REGISTER_X7,
    .operands.registerB = REGISTER_X8,
    .operands.registerC = REGISTER_X9,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x7 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x0005, 0xCF00)
TEST_CASE(0x5678, 0xFFFB, 0xC000)
TEST_CASE(0x5678, 0xFFFF, 0x0000)
TEST_CASE(0x5679, 0xFFFF, 0x8000)
void test_lsh_ri_should_setRegisterAToRegisterBLeftShiftedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x8 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_LSH_RI,
    .operands.registerA = REGISTER_X7,
    .operands.registerB = REGISTER_X8,
    .operands.immediateA.u16 = valueB,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x7 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x5678, 0x0005, 0xCF00)
TEST_CASE(0x5678, 0xFFFB, 0x0000)
void test_lsh_ir_should_setRegisterAToRegisterBLeftShiftedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x8 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_LSH_IR,
    .operands.registerA = REGISTER_X7,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X8,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x7 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x4321, 0x0005, 0x0219)
TEST_CASE(0x8765, 0x0005, 0xFC3B)
TEST_CASE(0x4321, 0xFFFB, 0x0000)
TEST_CASE(0x8765, 0xFFFB, 0xFFFF)
void test_rshs_rr_should_setRegisterAToRegisterBRightShiftedMsbExtendedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x8 = valueA;
  processState.registers.x9 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_RSHS_RR,
    .operands.registerA = REGISTER_X7,
    .operands.registerB = REGISTER_X8,
    .operands.registerC = REGISTER_X9,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x7 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x4321, 0x0005, 0x0219)
TEST_CASE(0x8765, 0x0005, 0xFC3B)
TEST_CASE(0x4321, 0xFFFB, 0x0008)
TEST_CASE(0x8765, 0xFFFB, 0xFFF0)
TEST_CASE(0x4321, 0xFFFF, 0x0000)
TEST_CASE(0x8765, 0xFFFF, 0xFFFF)
void test_rshs_ri_should_setRegisterAToRegisterBRightShiftedMsbExtendedByImmediateAUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x8 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_RSHS_RI,
    .operands.registerA = REGISTER_X7,
    .operands.registerB = REGISTER_X8,
    .operands.immediateA.u16 = valueB,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x7 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x4321, 0x0005, 0x0219)
TEST_CASE(0x8765, 0x0005, 0xFC3B)
TEST_CASE(0x4321, 0xFFFB, 0x0000)
TEST_CASE(0x8765, 0xFFFB, 0xFFFF)
void test_rshs_ir_should_setRegisterAToImmediateARightShiftedMsbExtendedByRegisterBUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x8 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_RSHS_IR,
    .operands.registerA = REGISTER_X7,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X8,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x7 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x4321, 0x0005, 0x0219)
TEST_CASE(0x8765, 0x0005, 0x043B)
TEST_CASE(0x4321, 0xFFFB, 0x0000)
TEST_CASE(0x8765, 0xFFFB, 0x0000)
void test_rshu_rr_should_setRegisterAtoRegisterBRightShiftedZeroExtendedByRegisterCUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x8 = valueA;
  processState.registers.x9 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_RSHU_RR,
    .operands.registerA = REGISTER_X7,
    .operands.registerB = REGISTER_X8,
    .operands.registerC = REGISTER_X9,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x7 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x4321, 0x0005, 0x0219)
TEST_CASE(0x8765, 0x0005, 0x043B)
TEST_CASE(0x4321, 0xFFFB, 0x0008)
TEST_CASE(0x8765, 0xFFFB, 0x0010)
TEST_CASE(0x4321, 0xFFFF, 0x0000)
TEST_CASE(0x8765, 0xFFFF, 0x0001)
void test_rshu_ri_should_setRegisterAtoRegisterBRightShiftedZeroExtendedByImmediateAUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x8 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_RSHU_RI,
    .operands.registerA = REGISTER_X7,
    .operands.registerB = REGISTER_X8,
    .operands.immediateA.u16 = valueB,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x7 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x4321, 0x0005, 0x0219)
TEST_CASE(0x8765, 0x0005, 0x043B)
TEST_CASE(0x4321, 0xFFFB, 0x0000)
TEST_CASE(0x8765, 0xFFFB, 0x0000)
void test_rshu_ir_should_setRegisterAToImmediateARightShiftedZeroExtendedByRegisterBUnsigned(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x8 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_RSHU_IR,
    .operands.registerA = REGISTER_X7,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X8,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x7 = expectedOutput;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

#pragma endregion

#pragma region Comparison

void test_ceq_r_should_setRegisterAToOne_when_registerBEqualsRegisterC(void) {
  // Arrange
  processState.registers.x1 = 0x1234;
  processState.registers.x2 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CEQ_R,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
    .operands.registerC = REGISTER_X2,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x0 = 1;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ceq_r_should_setRegisterAToZero_when_registerBDoesNotEqualRegisterC(void) {
  // Arrange
  processState.registers.x0 = 0xFFFF;
  processState.registers.x1 = 0x1234;
  processState.registers.x2 = 0x8765;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CEQ_R,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
    .operands.registerC = REGISTER_X2,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x0 = 0;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ceq_i_should_setRegisterAToOne_when_registerBEqualsImmediateA(void) {
  // Arrange
  processState.registers.x1 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CEQ_I,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
    .operands.immediateA.u16 = 0x1234,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x0 = 1;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ceq_i_should_setRegisterAToZero_when_registerBDoesNotEqualImmediateA(void) {
  // Arrange
  processState.registers.x0 = 0xFFFF;
  processState.registers.x1 = 0x1234;
  processState.registers.x2 = 0x8765;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CEQ_I,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
    .operands.immediateA.u16 = 0x8765,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x0 = 0;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_cne_r_should_setRegisterAToOne_when_registerBDoesNotEqualRegisterC(void) {
  // Arrange
  processState.registers.x1 = 0x1234;
  processState.registers.x2 = 0x8765;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CNE_R,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
    .operands.registerC = REGISTER_X2,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x0 = 1;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_cne_r_should_setRegisterAToZero_when_registerBEqualsRegisterC(void) {
  // Arrange
  processState.registers.x0 = 0xFFFF;
  processState.registers.x1 = 0x1234;
  processState.registers.x2 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CNE_R,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
    .operands.registerC = REGISTER_X2,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x0 = 0;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_cne_i_should_setRegisterAToOne_when_registerBDoesNotEqualImmediateA(void) {
  // Arrange
  processState.registers.x1 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CNE_I,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
    .operands.immediateA.u16 = 0x8765,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x0 = 1;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_cne_i_should_setRegisterAToZero_when_registerBEqualsImmediateA(void) {
  // Arrange
  processState.registers.x0 = 0xFFFF;
  processState.registers.x1 = 0x1234;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CNE_I,
    .operands.registerA = REGISTER_X0,
    .operands.registerB = REGISTER_X1,
    .operands.immediateA.u16 = 0x1234,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x0 = 0;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0000, 0x0001, 1)
TEST_CASE(0xFFFF, 0x0000, 1)
TEST_CASE(0xFFFE, 0xFFFF, 1)
TEST_CASE(0x0001, 0x0000, 0)
TEST_CASE(0x0000, 0xFFFF, 0)
TEST_CASE(0xFFFF, 0xFFFE, 0)
TEST_CASE(0x0000, 0x0000, 0)
TEST_CASE(0x0001, 0x0001, 0)
TEST_CASE(0xFFFF, 0xFFFF, 0)
void test_clts_rr_should_setRegisterAToOneIfRegisterBSignedIsLessThanRegisterCSignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x4 = valueA;
  processState.registers.x5 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CLTS_RR,
    .operands.registerA = REGISTER_X3,
    .operands.registerB = REGISTER_X4,
    .operands.registerC = REGISTER_X5,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0000, 0x0001, 1)
TEST_CASE(0xFFFF, 0x0000, 1)
TEST_CASE(0xFFFE, 0xFFFF, 1)
TEST_CASE(0x0001, 0x0000, 0)
TEST_CASE(0x0000, 0xFFFF, 0)
TEST_CASE(0xFFFF, 0xFFFE, 0)
TEST_CASE(0x0000, 0x0000, 0)
TEST_CASE(0x0001, 0x0001, 0)
TEST_CASE(0xFFFF, 0xFFFF, 0)
void test_clts_ri_should_setRegisterAToOneIfRegisterBSignedIsLessThanImmediateASignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x4 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CLTS_RI,
    .operands.registerA = REGISTER_X3,
    .operands.registerB = REGISTER_X4,
    .operands.immediateA.u16 = valueB,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0000, 0x0001, 1)
TEST_CASE(0xFFFF, 0x0000, 1)
TEST_CASE(0xFFFE, 0xFFFF, 1)
TEST_CASE(0x0001, 0x0000, 0)
TEST_CASE(0x0000, 0xFFFF, 0)
TEST_CASE(0xFFFF, 0xFFFE, 0)
TEST_CASE(0x0000, 0x0000, 0)
TEST_CASE(0x0001, 0x0001, 0)
TEST_CASE(0xFFFF, 0xFFFF, 0)
void test_clts_ir_should_setRegisterAToOneIfImmediateASignedIsLessThanRegisterBSignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x4 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CLTS_IR,
    .operands.registerA = REGISTER_X3,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X4,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0000, 0x0001, 1)
TEST_CASE(0x0000, 0xFFFF, 1)
TEST_CASE(0xFFFE, 0xFFFF, 1)
TEST_CASE(0x0001, 0x0000, 0)
TEST_CASE(0xFFFF, 0x0000, 0)
TEST_CASE(0xFFFF, 0xFFFE, 0)
TEST_CASE(0x0000, 0x0000, 0)
TEST_CASE(0x0001, 0x0001, 0)
TEST_CASE(0xFFFF, 0xFFFF, 0)
void test_cltu_rr_should_setRegisterAToOneIfRegisterBUnsignedIsLessThanRegisterCUnsignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x4 = valueA;
  processState.registers.x5 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CLTU_RR,
    .operands.registerA = REGISTER_X3,
    .operands.registerB = REGISTER_X4,
    .operands.registerC = REGISTER_X5,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0000, 0x0001, 1)
TEST_CASE(0x0000, 0xFFFF, 1)
TEST_CASE(0xFFFE, 0xFFFF, 1)
TEST_CASE(0x0001, 0x0000, 0)
TEST_CASE(0xFFFF, 0x0000, 0)
TEST_CASE(0xFFFF, 0xFFFE, 0)
TEST_CASE(0x0000, 0x0000, 0)
TEST_CASE(0x0001, 0x0001, 0)
TEST_CASE(0xFFFF, 0xFFFF, 0)
void test_cltu_ri_should_setRegisterAToOneIfRegisterBUnsignedIsLessThanImmediateAUnsignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x4 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CLTU_RI,
    .operands.registerA = REGISTER_X3,
    .operands.registerB = REGISTER_X4,
    .operands.immediateA.u16 = valueB,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0000, 0x0001, 1)
TEST_CASE(0x0000, 0xFFFF, 1)
TEST_CASE(0xFFFE, 0xFFFF, 1)
TEST_CASE(0x0001, 0x0000, 0)
TEST_CASE(0xFFFF, 0x0000, 0)
TEST_CASE(0xFFFF, 0xFFFE, 0)
TEST_CASE(0x0000, 0x0000, 0)
TEST_CASE(0x0001, 0x0001, 0)
TEST_CASE(0xFFFF, 0xFFFF, 0)
void test_cltu_ir_should_setRegisterAToOneIfImmediateAUnsignedIsLessThanRegisterBUnsignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x4 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CLTU_IR,
    .operands.registerA = REGISTER_X3,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X4,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0001, 0x0000, 1)
TEST_CASE(0x0000, 0xFFFF, 1)
TEST_CASE(0xFFFF, 0xFFFE, 1)
TEST_CASE(0x0000, 0x0000, 1)
TEST_CASE(0x0001, 0x0001, 1)
TEST_CASE(0xFFFF, 0xFFFF, 1)
TEST_CASE(0x0000, 0x0001, 0)
TEST_CASE(0xFFFF, 0x0000, 0)
TEST_CASE(0xFFFE, 0xFFFF, 0)
void test_cges_rr_should_setRegisterAToOneIfRegisterBSignedIsGreaterThanOrEqualToRegisterCSignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x4 = valueA;
  processState.registers.x5 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CGES_RR,
    .operands.registerA = REGISTER_X3,
    .operands.registerB = REGISTER_X4,
    .operands.registerC = REGISTER_X5,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0001, 0x0000, 1)
TEST_CASE(0x0000, 0xFFFF, 1)
TEST_CASE(0xFFFF, 0xFFFE, 1)
TEST_CASE(0x0000, 0x0000, 1)
TEST_CASE(0x0001, 0x0001, 1)
TEST_CASE(0xFFFF, 0xFFFF, 1)
TEST_CASE(0x0000, 0x0001, 0)
TEST_CASE(0xFFFF, 0x0000, 0)
TEST_CASE(0xFFFE, 0xFFFF, 0)
void test_cges_ri_should_setRegisterAToOneIfRegisterBSignedIsGreaterThanOrEqualToRegisterCSignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x4 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CGES_RI,
    .operands.registerA = REGISTER_X3,
    .operands.registerB = REGISTER_X4,
    .operands.immediateA.u16 = valueB,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0001, 0x0000, 1)
TEST_CASE(0x0000, 0xFFFF, 1)
TEST_CASE(0xFFFF, 0xFFFE, 1)
TEST_CASE(0x0000, 0x0000, 1)
TEST_CASE(0x0001, 0x0001, 1)
TEST_CASE(0xFFFF, 0xFFFF, 1)
TEST_CASE(0x0000, 0x0001, 0)
TEST_CASE(0xFFFF, 0x0000, 0)
TEST_CASE(0xFFFE, 0xFFFF, 0)
void test_cges_ir_should_setRegisterAToOneIfRegisterBSignedIsGreaterThanOrEqualToRegisterCSignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x4 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CGES_IR,
    .operands.registerA = REGISTER_X3,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X4,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0001, 0x0000, 1)
TEST_CASE(0xFFFF, 0x0000, 1)
TEST_CASE(0xFFFF, 0xFFFE, 1)
TEST_CASE(0x0000, 0x0000, 1)
TEST_CASE(0x0001, 0x0001, 1)
TEST_CASE(0xFFFF, 0xFFFF, 1)
TEST_CASE(0x0000, 0x0001, 0)
TEST_CASE(0x0000, 0xFFFF, 0)
TEST_CASE(0xFFFE, 0xFFFF, 0)
void test_cgeu_rr_should_setRegisterAToOne_when_registerBUnsignedIsGreaterThanOrEqualToRegisterCUnsignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x1 = valueA;
  processState.registers.x2 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CGEU_RR,
    .operands.registerA = REGISTER_X3,
    .operands.registerB = REGISTER_X4,
    .operands.registerC = REGISTER_X5,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0001, 0x0000, 1)
TEST_CASE(0xFFFF, 0x0000, 1)
TEST_CASE(0xFFFF, 0xFFFE, 1)
TEST_CASE(0x0000, 0x0000, 1)
TEST_CASE(0x0001, 0x0001, 1)
TEST_CASE(0xFFFF, 0xFFFF, 1)
TEST_CASE(0x0000, 0x0001, 0)
TEST_CASE(0x0000, 0xFFFF, 0)
TEST_CASE(0xFFFE, 0xFFFF, 0)
void test_cgeu_ri_should_setRegisterAToOne_when_registerBUnsignedIsGreaterThanOrEqualToRegisterCUnsignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x1 = valueA;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CGEU_RI,
    .operands.registerA = REGISTER_X3,
    .operands.registerB = REGISTER_X4,
    .operands.immediateA.u16 = valueB,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

TEST_CASE(0x0001, 0x0000, 1)
TEST_CASE(0xFFFF, 0x0000, 1)
TEST_CASE(0xFFFF, 0xFFFE, 1)
TEST_CASE(0x0000, 0x0000, 1)
TEST_CASE(0x0001, 0x0001, 1)
TEST_CASE(0xFFFF, 0xFFFF, 1)
TEST_CASE(0x0000, 0x0001, 0)
TEST_CASE(0x0000, 0xFFFF, 0)
TEST_CASE(0xFFFE, 0xFFFF, 0)
void test_cgeu_ir_should_setRegisterAToOne_when_registerBUnsignedIsGreaterThanOrEqualToRegisterCUnsignedAndZeroOtherwise(uint16_t valueA, uint16_t valueB, uint16_t expectedOutput) {
  // Arrange
  processState.registers.x1 = valueB;
  writeInstruction(processState.memory, 0, (Instruction){
    .opcode = OPCODE_CGEU_IR,
    .operands.registerA = REGISTER_X3,
    .operands.immediateA.u16 = valueA,
    .operands.registerB = REGISTER_X4,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0004;
  expectedEndState.registers.x3 = expectedOutput;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

#pragma endregion
