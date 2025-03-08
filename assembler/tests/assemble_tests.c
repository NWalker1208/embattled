#include <unity.h>
#include <string.h>
#include "assembler/assemble.h"
#include "processor/process.h"
#include "processor/instruction.h"

unsigned char memory[MEMORY_SIZE];
unsigned char expectedMemory[MEMORY_SIZE];
struct AssemblingError error;

void setUp() {
  memset(memory, 0x00, sizeof(memory));
  memset(expectedMemory, 0x00, sizeof(expectedMemory));
  memset(&error, 0, sizeof(error));
}

void test_tryAssemble_should_outputInstructionBytes_when_lineIsValidAssemblyInstruction(void) {
  // Arrange
  struct AssemblyParameter parameters[] = {
    { .kind = REGISTER, .registerName = X0 },
    { .kind = REGISTER, .registerName = X1 },
  };
  struct AssemblyLine lines[] = {
    {
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = ADD,
        .parameterCount = 2,
        .parameters = parameters,
      },
    }
  };

  storeInstruction(expectedMemory, 0, (struct Instruction){ 
    .opcode = ADD,
    .parameters.registerA = X0,
    .parameters.registerB = X1
  });

  // Act
  bool success = tryAssemble(lines, sizeof(lines), memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_tryAssemble_should_outputDataBytes_when_lineIsValidAssemblyData(void) {
  // Arrange
  unsigned char bytes[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
  struct AssemblyLine lines[] = {
    {
      .kind = DATA,
      .data = {
        .length = sizeof(bytes),
        .bytes = bytes,
      },
    }
  };

  memcpy(expectedMemory, bytes, sizeof(bytes));

  // Act
  bool success = tryAssemble(lines, sizeof(lines), memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}
