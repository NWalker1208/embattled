#pragma once
#include <unity.h>
#include "processor/instruction.h"
#include "processor/process.h"

#define TEST_ASSERT_EQUAL_INSTRUCTION(expected, actual)   CustomAssertEqualInstruction((expected), (actual), __LINE__)
#define TEST_ASSERT_EQUAL_PROCESS_STATE(expected, actual) CustomAssertEqualProcessState((expected), (actual), __LINE__)

void CustomAssertEqualInstruction(struct Instruction* expected, struct Instruction* actual, const UNITY_LINE_TYPE lineNumber);
void CustomAssertEqualProcessState(struct ProcessState* expected, struct ProcessState* actual, const UNITY_LINE_TYPE lineNumber);
