#pragma once
#include <unity.h>
#include "processor/process.h"

#define TEST_ASSERT_EQUAL_PROCESS_STATE(expected, actual) CustomAssertEqualProcessState((expected), (actual), __LINE__)

void CustomAssertEqualProcessState(struct ProcessState* expected, struct ProcessState* actual, const UNITY_LINE_TYPE lineNumber);
