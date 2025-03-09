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

  storeInstruction(expectedMemory, 0x0000, (struct Instruction){ 
    .opcode = ADD,
    .parameters.registerA = X0,
    .parameters.registerB = X1
  });

  // Act
  bool success = tryAssemble(lines, 1, memory, &error);

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
  bool success = tryAssemble(lines, 1, memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_tryAssemble_should_outputInstructionWithReference_when_instructionReferencesInstruction(void) {
  // Arrange
  struct AssemblyParameter parameters[] = {
    { .kind = LABEL_REFERENCE, .referencedLabel = "test" },
  };
  struct AssemblyLine lines[] = {
    {
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = JMP,
        .parameterCount = 1,
        .parameters = parameters,
      },
    },
    {
      .label = "test",
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = NOP
      },
    },
  };

  storeInstruction(expectedMemory, 0x0000, (struct Instruction){ 
    .opcode = JMP,
    .parameters.immediate.u16 = 0x0003,
  });
  storeInstruction(expectedMemory, 0x0003, (struct Instruction){
    .opcode = NOP,
  });

  // Act
  bool success = tryAssemble(lines, 2, memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}

void test_tryAssemble_should_outputInstructionWithReference_when_instructionReferencesData(void) {
  // Arrange
  struct AssemblyParameter parameters[] = {
    { .kind = LABEL_REFERENCE, .referencedLabel = "test" },
  };
  struct AssemblyLine lines[] = {
    {
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = LDIW,
        .parameterCount = 1,
        .parameters = parameters,
      },
    },
    {
      .label = "test",
      .kind = DATA,
      .data = {
        .length = 2,
        .bytes = "Hi",
      }
    },
  };

  storeInstruction(expectedMemory, 0x0000, (struct Instruction){ 
    .opcode = LDIW,
    .parameters.immediate.u16 = 0x0003,
  });
  memcpy(&expectedMemory[3], "Hi", 2);

  // Act
  bool success = tryAssemble(lines, 2, memory, &error);

  // Assert
  TEST_ASSERT_TRUE_MESSAGE(success, error.message);
  TEST_ASSERT_EQUAL_MEMORY(expectedMemory, memory, sizeof(memory));
}
