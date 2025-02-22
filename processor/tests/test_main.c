#include <unity.h>
#include "processor/process.h"
#include "processor/instruction.h"

unsigned char memory[MEMORY_SIZE];
struct ProcessState processState;

void setUp() {
  memset(memory, 0, sizeof(memory));
  memset(&processState, 0, sizeof(processState));
}

void tearDown() { }

void test_ldiw() {
  memory[0] = OPCODE_VALUES[LDIW];
  memory[1] = 0x01;
  memory[2] = 0x00;

  stepProcess(memory, &processState);

  TEST_ASSERT_EQUAL(0x0100, processState.ac);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_ldiw);
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
