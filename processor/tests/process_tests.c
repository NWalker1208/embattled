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

#pragma region Control flow instructions

void test_nop_should_doNothing(void) {
  // Arrange
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = NOP,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0001;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_jmp_should_jumpToAddressAndSaveReturnAddress(void) {
  // Arrange
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = JMP,
    .parameters.immediate.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;
  expectedEndState.registers.ac = 0x0003;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_jmz_should_jumpToAddress_when_acIsZero(void) {
  // Arrange
  processState.registers.ac = 0x0000;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = JMZ,
    .parameters.immediate.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_jmz_should_doNothing_when_acIsNonZero(void) {
  // Arrange
  processState.registers.ac = 0x0001;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = JMZ,
    .parameters.immediate.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

#pragma endregion

#pragma region Memory instructions

TEST_MATRIX([SP, AC, X0, X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11], [SP, AC, X0, X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11])
void test_mov_should_copyValueFromRegisterBToRegisterA_when_neitherRegisterIsNullOrIp(enum Register regA, enum Register regB) {
  // Arrange
  setUp();
  *getRegisterPtr(&processState.registers, regA) = 0x1234;
  *getRegisterPtr(&processState.registers, regB) = 0x5678;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = MOV,
    .parameters.registerA = regA,
    .parameters.registerB = regB,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  *getRegisterPtr(&expectedEndState.registers, regA) = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_mov_should_copyValueFromRegisterBToIp_when_registerAIsIp(void) {
  // Arrange
  processState.registers.x3 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = MOV,
    .parameters.registerA = IP,
    .parameters.registerB = X3,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_mov_should_copyValueFromIpToRegisterA_when_registerBIsIp(void) {
  // Arrange
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = MOV,
    .parameters.registerA = X3,
    .parameters.registerB = IP,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.x3 = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_mov_should_setRegisterAToZero_when_registerBIsNull(void) {
  // Arrange
  processState.registers.x7 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = MOV,
    .parameters.registerA = X7,
    .parameters.registerB = NL,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.x7 = 0x0000;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_mov_should_doNothing_when_registerAIsNull(void) {
  // Arrange
  processState.registers.x7 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = MOV,
    .parameters.registerA = NL,
    .parameters.registerB = X7,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_mov_should_doNothing_when_bothRegistersAreNull(void) {
  // Arrange
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = MOV,
    .parameters.registerA = NL,
    .parameters.registerB = NL,
  });
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldib_should_loadImmediateByteIntoAc(void) {
  // Arrange
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = LDIB,
    .parameters.immediate.u8 = 0x12,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0012;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldiw_should_loadImmediateWordIntoAc(void) {
  // Arrange
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = LDIW,
    .parameters.immediate.u16 = 0x1234,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.ac = 0x1234;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldmb_should_loadMemoryByteAtAddressIntoAc_when_immediateValueIsZero(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = LDMB,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x0,
  });
  processState.memory[0x1234] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0056;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldmb_should_loadMemoryByteAtAddressWithOffsetIntoAc_when_immediateValueIsPositive(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = LDMB,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x7,
  });
  processState.memory[0x123B] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0056;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldmb_should_loadMemoryByteAtAddressWithOffsetIntoAc_when_immediateValueIsNegative(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = LDMB,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x8, // 0x8 = -8 as a nibble
  });
  processState.memory[0x122C] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0056;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldmw_should_loadMemoryWordAtAddressIntoAc_when_immediateValueIsZero(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = LDMW,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x0,
  });
  processState.memory[0x1234] = 0x78;
  processState.memory[0x1235] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldmw_should_loadMemoryWordAtAddressWithOffsetIntoAc_when_immediateValueIsPositive(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = LDMW,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x7,
  });
  processState.memory[0x123B] = 0x78;
  processState.memory[0x123C] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldmw_should_loadMemoryWordAtAddressWithOffsetIntoAc_when_immediateValueIsNegative(void) {
  // Arrange
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = LDMW,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x8, // 0x8 = -8 as a nibble
  });
  processState.memory[0x122C] = 0x78;
  processState.memory[0x122D] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_ldmw_should_loadMemoryWordAtAddressWrappingToZero_when_addressIsMaximumValue(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.x0 = 0xFFFF;
  storeInstruction(processState.memory, 1, (struct Instruction){
    .opcode = LDMW,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x0,
  });
  processState.memory[0xFFFF] = 0x78;
  processState.memory[0x0000] = 0x56;

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.ac = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stmb_should_storeLowerAcIntoMemoryByteAtAddress_when_immediateValueIsZero(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = STMB,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x0,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x1234] = 0x78;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stmb_should_storeLowerAcIntoMemoryByteAtAddressWithOffset_when_immediateValueIsPositive(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = STMB,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x7,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x123B] = 0x78;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stmb_should_storeLowerAcIntoMemoryByteAtAddressWithOffset_when_immediateValueIsNegative(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = STMB,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x8, // 0x8 = -8 as a nibble
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x122C] = 0x78;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stmw_should_storeFullAcIntoMemoryWordAtAddress_when_immediateValueIsZero(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = STMW,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x0,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x1234] = 0x78;
  expectedEndState.memory[0x1235] = 0x56;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stmw_should_storeFullAcIntoMemoryWordAtAddressWithOffset_when_immediateValueIsPositive(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = STMW,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x7,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x123B] = 0x78;
  expectedEndState.memory[0x123C] = 0x56;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stmw_should_storeFullAcIntoMemoryWordAtAddressWithOffset_when_immediateValueIsNegative(void) {
  // Arrange
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = STMW,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x8, // 0x8 = -8 as a nibble
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.memory[0x122C] = 0x78;
  expectedEndState.memory[0x122D] = 0x56;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_stmw_should_storeFullAcIntoMemoryWordAtAddressWrappingToZero_when_addressIsMaxValue(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.ac = 0x5678;
  processState.registers.x0 = 0xFFFF;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = STMW,
    .parameters.registerA = X0,
    .parameters.immediate.u4 = 0x0,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.memory[0xFFFF] = 0x78;
  expectedEndState.memory[0x0000] = 0x56;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_pshb_should_pushLowerRegisterByteOntoStack_when_registerIsNotSp(void) {
  // Arrange
  processState.registers.sp = 0x0000;
  processState.registers.x4 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = PSHB,
    .parameters.registerA = X0,
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

void test_pshb_should_pushLowerSpOntoStackBeforeDecrement_when_registerIsSp(void) {
  // Arrange
  processState.registers.sp = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = PSHB,
    .parameters.registerA = SP,
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

void test_pshw_should_pushFullRegisterWordOntoStack_when_registerIsNotSp(void) {
  // Arrange
  processState.registers.sp = 0x0000;
  processState.registers.x4 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = PSHW,
    .parameters.registerA = X4,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0xFFFE;
  expectedEndState.memory[0xFFFE] = 0x34;
  expectedEndState.memory[0xFFFF] = 0x12;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_pshw_should_pushFullSpOntoStackBeforeDecrement_when_registerIsSp(void) {
  // Arrange
  processState.registers.sp = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = PSHW,
    .parameters.registerA = SP,
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

void test_pshw_should_pushFullRegisterOntoStackWrappingToEnd_when_stackPointerIsOne(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.sp = 0x0001;
  processState.registers.x4 = 0x1234;
  storeInstruction(processState.memory, 1, (struct Instruction){
    .opcode = PSHW,
    .parameters.registerA = X4,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.sp = 0xFFFF;
  expectedEndState.memory[0xFFFF] = 0x34;
  expectedEndState.memory[0x0000] = 0x12;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_popb_should_popStackByteIntoRegister_when_registerIsNotSp(void) {
  // Arrange
  processState.registers.sp = 0xFFFE;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = POPB,
    .parameters.registerA = X6,
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

void test_popb_should_popStackByteIntoSpAfterIncrement_when_registerIsSp(void) {
  // Arrange
  processState.registers.sp = 0xFFFE;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = POPB,
    .parameters.registerA = SP,
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

void test_popw_should_popStackWordIntoRegister_when_registerIsNotSp(void) {
  // Arrange
  processState.registers.sp = 0xFFFE;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = POPW,
    .parameters.registerA = X6,
  });
  processState.memory[0xFFFE] = 0x34;
  processState.memory[0xFFFF] = 0x12;
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.sp = 0x0000;
  expectedEndState.registers.x6 = 0x1234;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_popw_should_popStackWordIntoSpAfterIncrement_when_registerIsSp(void) {
  // Arrange
  processState.registers.sp = 0xFFFE;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = POPW,
    .parameters.registerA = SP,
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

void test_popw_should_popStackWordIntoRegisterWrappingToZero_when_stackPointerIsMaxValue(void) {
  // Arrange
  processState.registers.ip = 0x0001;
  processState.registers.sp = 0xFFFF;
  storeInstruction(processState.memory, 1, (struct Instruction){
    .opcode = POPW,
    .parameters.registerA = X6,
  });
  processState.memory[0xFFFF] = 0x34;
  processState.memory[0x0000] = 0x12;
  
  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0003;
  expectedEndState.registers.sp = 0x0001;
  expectedEndState.registers.x6 = 0x1234;
  
  // Act
  stepProcess(&processState);
  
  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

#pragma endregion

#pragma region Math and logic instructions

void test_inc_should_incrementRegisterAByImmediateValue_whenRegisterAIsNotNull(void) {
  // Arrange
  processState.registers.x1 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = INC,
    .parameters.registerA = X1,
    .parameters.immediate.u4 = 0xF,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.x1 = 0x1243;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_inc_should_doNothing_whenRegisterAIsNull(void) {
  // Arrange
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = INC,
    .parameters.registerA = NL,
    .parameters.immediate.u4 = 0xF,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_dec_should_decrementRegisterAByImmediateValue_whenRegisterAIsNotNull(void) {
  // Arrange
  processState.registers.x1 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DEC,
    .parameters.registerA = X1,
    .parameters.immediate.u4 = 0xF,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.x1 = 0x1225;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_dec_should_doNothing_whenRegisterAIsNull(void) {
  // Arrange
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DEC,
    .parameters.registerA = NL,
    .parameters.immediate.u4 = 0xF,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_add_should_setAcToRegisterAPlusRegisterB(void) {
  // Arrange
  processState.registers.x1 = 0x1234;
  processState.registers.x2 = 0x5678;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = ADD,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x68AC;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_sub_should_setAcToRegisterAMinusRegisterB(void) {
  // Arrange
  processState.registers.x1 = 0x5678;
  processState.registers.x2 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = SUB,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x4444;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_mul_should_setAcToRegisterATimesRegisterB(void) {
  // Arrange
  processState.registers.x1 = 0x1234;
  processState.registers.x2 = 0x5678;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = MUL,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0060;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_divs_should_setAcToRegisterADividedByRegisterBSigned_whenNeitherRegisterHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x5678;
  processState.registers.x2 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DIVS,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0004;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_divs_should_setAcToRegisterADividedByRegisterBSigned_whenRegisterAHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x8765;
  processState.registers.x2 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DIVS,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0xFFFA;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_divs_should_setAcToRegisterADividedByRegisterBSigned_whenRegisterBHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x5678;
  processState.registers.x2 = 0xFEDC;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DIVS,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0xFFB5;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_divs_should_setAcToRegisterADividedByRegisterBSigned_whenBothRegistersHaveMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0xBA98;
  processState.registers.x2 = 0xFEDC;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DIVS,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x003C;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_divu_should_setAcToRegisterADividedByRegisterBUnsigned_whenNeitherRegisterHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x5678;
  processState.registers.x2 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DIVU,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0004;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_divu_should_setAcToRegisterADividedByRegisterBUnsigned_whenRegisterAHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x8765;
  processState.registers.x2 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DIVU,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0007;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_divu_should_setAcToRegisterADividedByRegisterBUnsigned_whenRegisterBHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x5678;
  processState.registers.x2 = 0xFEDC;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DIVU,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0000;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_divu_should_setAcToRegisterADividedByRegisterBUnsigned_whenBothRegistersHaveMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0xBA98;
  processState.registers.x2 = 0xFEDC;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = DIVU,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0000;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_rems_should_setAcToRemainderOfRegisterADividedByRegisterBSigned_whenNeitherRegisterHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x5678;
  processState.registers.x2 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = REMS,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0DA8;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_rems_should_setAcToRemainderOfRegisterADividedByRegisterBSigned_whenRegisterAHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x8765;
  processState.registers.x2 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = REMS,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0xF49D;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_rems_should_setAcToRemainderOfRegisterADividedByRegisterBSigned_whenRegisterBHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x5678;
  processState.registers.x2 = 0xFEDC;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = REMS,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x00EC;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_rems_should_setAcToRemainderOfRegisterADividedByRegisterBSigned_whenBothRegistersHaveMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0xBA98;
  processState.registers.x2 = 0xFEDC;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = REMS,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0xFF08;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_remu_should_setAcToRemainderOfRegisterADividedByRegisterBUnsigned_whenNeitherRegisterHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x5678;
  processState.registers.x2 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = REMU,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x0DA8;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_remu_should_setAcToRemainderOfRegisterADividedByRegisterBUnsigned_whenRegisterAHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x8765;
  processState.registers.x2 = 0x1234;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = REMU,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x07F9;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_remu_should_setAcToRemainderOfRegisterADividedByRegisterBUnsigned_whenRegisterBHasMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0x5678;
  processState.registers.x2 = 0xFEDC;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = REMU,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0x5678;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

void test_remu_should_setAcToRemainderOfRegisterADividedByRegisterBUnsigned_whenBothRegistersHaveMsbSet(void) {
  // Arrange
  processState.registers.x1 = 0xBA98;
  processState.registers.x2 = 0xFEDC;
  storeInstruction(processState.memory, 0, (struct Instruction){
    .opcode = REMU,
    .parameters.registerA = X1,
    .parameters.registerB = X2,
  });

  initializeExpectedEndState();
  expectedEndState.registers.ip = 0x0002;
  expectedEndState.registers.ac = 0xBA98;

  // Act
  stepProcess(&processState);

  // Assert
  TEST_ASSERT_EQUAL_PROCESS_STATE(&expectedEndState, &processState);
}

// TODO: Write tests for other instructions

#pragma endregion

/*
Example program:
  memory[0] = LDIW;
  memory[1] = 0x01;
  memory[2] = 0x00;
  
  memory[3] = MOV;
  memory[4] = (unsigned char)X0 << 4 | (unsigned char)AC;
  
  memory[5] = ADD;
  memory[6] = (unsigned char)AC << 4 | (unsigned char)X0;

  memory[7] = PSHW;
  memory[8] = (unsigned char)AC << 4;

  memory[9] = JMP;
  memory[10] = 0x05;
  memory[11] = 0x00;
*/
