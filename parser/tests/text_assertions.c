#include "text_assertions.h"

void CustomAssertEquivalentTextOffset(TextOffset expected, TextOffset actual, const TextContents* text, const UNITY_LINE_TYPE lineNumber) {
  UNITY_TEST_ASSERT(CompareTextOffsets(text, expected, actual) == 0, lineNumber, "Actual offset is not equivalent to expected offset.");
}

void CustomAssertEquivalentTextSpan(TextSpan expected, TextSpan actual, const TextContents* text, const UNITY_LINE_TYPE lineNumber) {
  UNITY_TEST_ASSERT(CompareTextOffsets(text, expected.start, actual.start) == 0, lineNumber, "Actual span start is not equivalent to expected span start.");
  UNITY_TEST_ASSERT(CompareTextOffsets(text, expected.end, actual.end) == 0, lineNumber, "Actual span end is not equivalent to expected span end.");
}

void CustomAssertEqualTextSpanChars(const char* expected, TextSpan actual, const TextContents* text, const UNITY_LINE_TYPE lineNumber) {
  TextContents tempContents = InitTextContentsAsCopyCStr(expected);
  TextSpan tempSpan = { .start = {0, 0}, .end = GetTextContentsEnd(&tempContents) };
  bool result = CompareTextSpans(&tempContents, tempSpan, text, actual) == 0;
  DestroyTextContents(&tempContents);

  UNITY_TEST_ASSERT(result, lineNumber, "Actual span does not have the expected characters.");
}
