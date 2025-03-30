#pragma once
#include <unity.h>
#include "utilities/text.h"

#define TEST_ASSERT_EQUIVALENT_TEXT_OFFSET(expected, actual, text) CustomAssertEquivalentTextOffset((expected), (actual), &(text), __LINE__)
#define TEST_ASSERT_EQUIVALENT_TEXT_SPAN(expected, actual, text)   CustomAssertEquivalentTextSpan((expected), (actual), &(text), __LINE__)
#define TEST_ASSERT_EQUAL_TEXT_SPAN_CHARS(expected, actual, text)   CustomAssertEqualsTextSpanChars((expected), (actual), &(text), __LINE__)

void CustomAssertEquivalentTextOffset(TextOffset expected, TextOffset actual, const TextContents* text, const UNITY_LINE_TYPE lineNumber);
void CustomAssertEquivalentTextSpan(TextSpan expected, TextSpan actual, const TextContents* text, const UNITY_LINE_TYPE lineNumber);
void CustomAssertEqualTextSpanChars(const char* expected, TextSpan actual, const TextContents* text, const UNITY_LINE_TYPE lineNumber);
