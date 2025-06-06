#include <unity.h>
#include <string.h>
#include "utilities/text.h"

TextContents text;

void setUp() {
  text = (TextContents){ 0 };
}

void tearDown() {
  DestroyTextContents(&text);
}

#pragma region InitTextContents

void test_InitTextContentsAsCopy_should_returnLines_when_givenStringWithUnixLineEnding() {
  // Arrange
  char chars[] = "first line\nsecond line";

  // Act
  text = InitTextContentsAsCopy(chars, sizeof(chars) - 1);

  // Assert
  TEST_ASSERT_TRUE(IsTextContentsInitialized(&text));
  TEST_ASSERT_EQUAL(2, text.lineCount);
  TEST_ASSERT_EQUAL_size_t(strlen("first line"), text.lines[0].length);
  TEST_ASSERT_EQUAL_STRING("first line", &text.chars[text.lines[0].startIndex]);
  TEST_ASSERT_EQUAL_size_t(strlen("second line"), text.lines[1].length);
  TEST_ASSERT_EQUAL_STRING("second line", &text.chars[text.lines[1].startIndex]);
}

void test_InitTextContentsAsCopy_should_returnLines_when_givenStringWithWindowsLineEnding() {
  // Arrange
  char chars[] = "first line\r\nsecond line";

  // Act
  text = InitTextContentsAsCopy(chars, sizeof(chars) - 1);

  // Assert
  TEST_ASSERT_TRUE(IsTextContentsInitialized(&text));
  TEST_ASSERT_EQUAL(2, text.lineCount);
  TEST_ASSERT_EQUAL_size_t(strlen("first line"), text.lines[0].length);
  TEST_ASSERT_EQUAL_STRING("first line", &text.chars[text.lines[0].startIndex]);
  TEST_ASSERT_EQUAL_size_t(strlen("second line"), text.lines[1].length);
  TEST_ASSERT_EQUAL_STRING("second line", &text.chars[text.lines[1].startIndex]);
}

void test_InitTextContentsAsCopy_should_returnEmptyLastLine_when_givenStringWithTrailingNewline() {
  // Arrange
  char chars[] = "first line\nsecond line\n";

  // Act
  text = InitTextContentsAsCopy(chars, sizeof(chars) - 1);

  // Assert
  TEST_ASSERT_TRUE(IsTextContentsInitialized(&text));
  TEST_ASSERT_EQUAL(3, text.lineCount);
  TEST_ASSERT_EQUAL_size_t(strlen("first line"), text.lines[0].length);
  TEST_ASSERT_EQUAL_STRING("first line", &text.chars[text.lines[0].startIndex]);
  TEST_ASSERT_EQUAL_size_t(strlen("second line"), text.lines[1].length);
  TEST_ASSERT_EQUAL_STRING("second line", &text.chars[text.lines[1].startIndex]);
  TEST_ASSERT_EQUAL_size_t(0, text.lines[2].length);
  TEST_ASSERT_EQUAL_STRING("", &text.chars[text.lines[2].startIndex]);
}

void test_InitTextContentsAsCopy_should_returnIgnoreNullCharacter_when_givenStringContainingNull() {
  // Arrange
  char chars[] = "null\0char";

  // Act
  text = InitTextContentsAsCopy(chars, sizeof(chars) - 1);

  // Assert
  TEST_ASSERT_TRUE(IsTextContentsInitialized(&text));
  TEST_ASSERT_EQUAL(1, text.lineCount);
  TEST_ASSERT_EQUAL_size_t(sizeof(chars) - 1, text.lines[0].length);
  TEST_ASSERT_EQUAL_CHAR_ARRAY(chars, &text.chars[text.lines[0].startIndex], sizeof(chars));
}

#pragma endregion

#pragma region CompareTextSpans

void test_CompareTextSpans_should_returnZero_when_givenIdenticalSpans() {
  // Arrange
  text = InitTextContentsAsCopyCStr("some text");
  TextSpan span = {{0, 0}, {0, 4}};

  // Act
  int result = CompareTextSpans(&text, span, &text, span);

  // Assert
  TEST_ASSERT_EQUAL_INT(0, result);
}

void test_CompareTextSpans_should_returnZero_when_givenSpansWithIdenticalChars() {
  // Arrange
  text = InitTextContentsAsCopyCStr("text text");
  TextSpan spanA = {{0, 0}, {0, 4}};
  TextSpan spanB = {{0, 5}, {0, 9}};

  // Act
  int result = CompareTextSpans(&text, spanA, &text, spanB);

  // Assert
  TEST_ASSERT_EQUAL_INT(0, result);
}

void test_CompareTextSpans_should_returnNegativeInt_when_spanACharsAreLessThanSpanBChars() {
  // Arrange
  text = InitTextContentsAsCopyCStr("abc bcd");
  TextSpan spanA = {{0, 0}, {0, 3}};
  TextSpan spanB = {{0, 4}, {0, 7}};

  // Act
  int result = CompareTextSpans(&text, spanA, &text, spanB);

  // Assert
  TEST_ASSERT_LESS_THAN(0, result);
}

void test_CompareTextSpans_should_returnPositiveInt_when_spanACharsAreGreaterThanSpanBChars() {
  // Arrange
  text = InitTextContentsAsCopyCStr("bcd abc");
  TextSpan spanA = {{0, 0}, {0, 3}};
  TextSpan spanB = {{0, 4}, {0, 7}};

  // Act
  int result = CompareTextSpans(&text, spanA, &text, spanB);

  // Assert
  TEST_ASSERT_GREATER_THAN(0, result);
}

#pragma endregion
