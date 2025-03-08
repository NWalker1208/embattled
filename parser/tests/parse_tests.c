#include <string.h>
#include <stdbool.h>
#include <unity.h>
#include "parser/parse.h"

struct AssemblyLine line;
struct ParsingError error;

void setUp() {
  memset(&line, 0, sizeof(line));
  memset(&error, 0, sizeof(error));
}

void tearDown() {
  destroyAssemblyLine(&line);
}

void test_tryParseAssemblyLine_should_returnFalse_whenLineIsEmpty(void) {
  // Arrange
  const char* source = "";
  const char* text = source;

  // Act
  bool success = tryParseAssemblyLine(&text, &line, &error);

  // Assert
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_EQUAL_PTR_MESSAGE(source, text, "Text should not have been advanced.");
  TEST_ASSERT_EQUAL_PTR(UNEXPECTED_END_OF_FILE, error.message);
  TEST_ASSERT_EQUAL_PTR(source, error.location);
}
