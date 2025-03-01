#include <unity.h>
#include "processor/instruction.h"

unsigned char memory[8];
unsigned char expectedMemory[8];

void setUp() {
  expectedMemory[0] = memory[0] = 0x12;
  expectedMemory[1] = memory[1] = 0x34;
  expectedMemory[2] = memory[2] = 0x56;
  expectedMemory[3] = memory[3] = 0x78;
  expectedMemory[4] = memory[4] = 0x9A;
  expectedMemory[5] = memory[5] = 0xBC;
  expectedMemory[6] = memory[6] = 0xDE;
  expectedMemory[7] = memory[7] = 0xF0;
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

}

void test_storeInstruction_shouldSave8BitImmediateValue_whenOpcodeHasLayoutImm8(void) {

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
