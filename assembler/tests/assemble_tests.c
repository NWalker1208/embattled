#define UNITY_SUPPORT_TEST_CASES
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

void test_tryAssemble_should_outputErrorTooFewParams_when_instructionHasTooFewParams(void) {
  // Arrange
  struct AssemblyParameter parameters[] = {
    { .kind = REGISTER, .registerName = X0 },
  };
  struct AssemblyLine lines[] = {
    {
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = ADD,
        .parameterCount = 1,
        .parameters = parameters,
      },
    }
  };

  // Act
  bool success = tryAssemble(lines, 1, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(TOO_FEW_PARAMS, error.message);
  TEST_ASSERT_EQUAL_INT(0, error.lineNumber);
}

void test_tryAssemble_should_outputErrorTooManyParams_when_instructionHasTooManyParams(void) {
  // Arrange
  struct AssemblyParameter parameters[] = {
    { .kind = REGISTER, .registerName = X0 },
    { .kind = REGISTER, .registerName = X1 },
    { .kind = REGISTER, .registerName = X2 },
  };
  struct AssemblyLine lines[] = {
    {
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = ADD,
        .parameterCount = 3,
        .parameters = parameters,
      },
    }
  };

  // Act
  bool success = tryAssemble(lines, 1, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(TOO_MANY_PARAMS, error.message);
  TEST_ASSERT_EQUAL_INT(0, error.lineNumber);
}

void test_tryAssemble_should_outputErrorExpectedRegister_when_instructionHasImmediateValueInsteadOfRegister(void) {
  // Arrange
  struct AssemblyParameter parameters[] = {
    { .kind = REGISTER, .registerName = X0 },
    { .kind = IMMEDIATE_VALUE, .immediateValue = 0x1234 },
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

  // Act
  bool success = tryAssemble(lines, 1, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(EXPECTED_REGISTER, error.message);
  TEST_ASSERT_EQUAL_INT(0, error.lineNumber);
}

void test_tryAssemble_should_outputErrorExpectedImmediateValue_when_instructionHasRegisterInsteadOfUnder16BitImmediateValue(void) {
  // Arrange
  struct AssemblyParameter parameters[] = {
    { .kind = REGISTER, .registerName = X0 },
    { .kind = REGISTER, .registerName = X1 },
  };
  struct AssemblyLine lines[] = {
    {
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = INC,
        .parameterCount = 2,
        .parameters = parameters,
      },
    }
  };

  // Act
  bool success = tryAssemble(lines, 1, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(EXPECTED_IMMEDIATE_VALUE, error.message);
  TEST_ASSERT_EQUAL_INT(0, error.lineNumber);
}

void test_tryAssemble_should_outputErrorExpectedImmediateValueOrLabelRef_when_instructionHasRegisterInsteadOf16BitImmediateValue(void) {
  // Arrange
  struct AssemblyParameter parameters[] = {
    { .kind = REGISTER, .registerName = X0 },
  };
  struct AssemblyLine lines[] = {
    {
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = JMP,
        .parameterCount = 1,
        .parameters = parameters,
      },
    }
  };

  // Act
  bool success = tryAssemble(lines, 1, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(EXPECTED_IMMEDIATE_VALUE_OR_LABEL_REF, error.message);
  TEST_ASSERT_EQUAL_INT(0, error.lineNumber);
}

TEST_CASE(JMP, false, -1)
TEST_CASE(JMP, false, 0x10000)
TEST_CASE(LDIB, false, -1)
TEST_CASE(LDIB, false, 0x100)
TEST_CASE(INC, true, -1)
TEST_CASE(INC, true, 0x10)
TEST_CASE(LDMB, true, -9)
TEST_CASE(LDMB, true, 8)
void test_tryAssemble_should_outputErrorImmediateValueOutOfRange_when_instructionHasImmediateValueOutOfRange(enum Opcode opcode, bool needsRegister, signed int immediateValue) {
  // Arrange
  struct AssemblyParameter parameters[] = {
    { .kind = REGISTER, .registerName = X0 },
    { .kind = IMMEDIATE_VALUE, .immediateValue = immediateValue },
  };
  struct AssemblyLine lines[] = {
    {
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = opcode,
        .parameterCount = needsRegister ? 2 : 1,
        .parameters = needsRegister ? parameters : parameters + 1,
      },
    }
  };

  // Act
  bool success = tryAssemble(lines, 1, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(IMMEDIATE_VALUE_OUT_OF_RANGE, error.message);
  TEST_ASSERT_EQUAL_INT(0, error.lineNumber);
}

void test_tryAssemble_should_outputErrorInvalidInstruction_when_instructionHasUndefinedOpcode(void) {
  // Arrange
  struct AssemblyLine lines[] = {
    {
      .kind = INSTRUCTION,
      .instruction = {
        .opcode = OPCODE_COUNT,
      },
    }
  };

  // Act
  bool success = tryAssemble(lines, 1, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(INVALID_INSTRUCTION, error.message);
  TEST_ASSERT_EQUAL_INT(0, error.lineNumber);
}

void test_tryAssemble_should_outputErrorUndefinedLabel_when_instructionReferencesUndefinedLabel(void) {
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
    }
  };

  // Act
  bool success = tryAssemble(lines, 1, memory, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_STRING(UNDEFINED_LABEL, error.message);
  TEST_ASSERT_EQUAL_INT(0, error.lineNumber);
}
