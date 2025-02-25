#include "custom_assertions.h"

void CustomAssertEqualProcessState(struct ProcessState* expected, struct ProcessState* actual, const UNITY_LINE_TYPE lineNumber) {
  // Assert registers individually
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.ip, actual->registers.ip, lineNumber, "IP register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.ac, actual->registers.ac, lineNumber, "AC register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.sp, actual->registers.sp, lineNumber, "SP register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x0, actual->registers.x0, lineNumber, "X0 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x1, actual->registers.x1, lineNumber, "X1 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x2, actual->registers.x2, lineNumber, "X2 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x3, actual->registers.x3, lineNumber, "X3 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x4, actual->registers.x4, lineNumber, "X4 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x5, actual->registers.x5, lineNumber, "X5 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x6, actual->registers.x6, lineNumber, "X6 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x7, actual->registers.x7, lineNumber, "X7 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x8, actual->registers.x8, lineNumber, "X8 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x9, actual->registers.x9, lineNumber, "X9 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x10, actual->registers.x10, lineNumber, "X10 register of actual state differs from expected state.");
  UNITY_TEST_ASSERT_EQUAL_INT(expected->registers.x11, actual->registers.x11, lineNumber, "X11 register of actual state differs from expected state.");

  // Assert memory
  UNITY_TEST_ASSERT_EQUAL_MEMORY(expected->memory, actual->memory, MEMORY_SIZE, lineNumber, "Memory of actual state differs from expected state.");
}
