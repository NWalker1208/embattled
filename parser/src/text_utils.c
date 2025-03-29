#include <stdlib.h>
#include <ctype.h>
#include "text_utils.h"

bool isWordChar(char c) {
  return isalnum(c) || c == '_';
}

bool startsWithWordCaseInsensitive(const TextContents* text, TextOffset start, const char* word) {
  TextOffset textEnd = GetTextContentsEnd(text);
  while (*word != '\0' && CompareTextOffsets(text, start, textEnd) < 0) {
    if (tolower(GetCharAtTextOffset(text, start)) != tolower(*word)) {
      return false;
    }
    IncrementTextOffset(text, &start);
    word++;
  }
  return *word == '\0' && !isWordChar(GetCharAtTextOffset(text, start));
}

bool skipInlineWhitespace(const TextContents* text, TextOffset* position) {
  bool skippedAny = false;
  while (isInlineWhitespace(GetCharAtTextOffset(text, *position))) {
    IncrementTextOffset(text, position);
    skippedAny = true;
  }
  return skippedAny;
}

void skipAllWhitespace(const TextContents* text, TextOffset* position) {
  while (isAnyWhitespace(GetCharAtTextOffset(text, *position))) {
    IncrementTextOffset(text, position);
  }
}

void skipToNextWhitespace(const TextContents* text, TextOffset* position) {
  while (!isAnyWhitespace(GetCharAtTextOffset(text, *position))) {
    IncrementTextOffset(text, position);
  }
}

bool lineContainsChar(const TextContents* text, TextOffset start, char c) {
  while (!IsTextOffsetEndOfLine(text, start)) {
    if (GetCharAtTextOffset(text, start) == c) {
      return true;
    }
    IncrementTextOffset(text, &start);
  }
  return false;
}

bool tryHexToNibble(char c, unsigned char* result) {
  if ('0' <= c && c <= '9') {
    *result = c - '0';
    return true;
  } else if ('a' <= c && c <= 'f') {
    *result = c - 'a' + 10;
    return true;
  } else if ('A' <= c && c <= 'F') {
    *result = c - 'A' + 10;
    return true;
  } else {
    return false;
  }
}
